#include "globals.h"
#include "options.h"
#include "id3.h"
#include "local.h"


local_player::local_player() :
    buffered(0),
    pre_buffer(40000),
    buffer_size(2000000),
    buffer(0),
    bufferring(true),
    last_chunk(0),
    buffers_recvd(0),
    buffers_sent(0),
    lastread(0),
    filepos(0),
    fileendpos(0),
    filesentpos(0),
    at_end(false)

{
    memset(track_title,0,SMALL_MEM);
    strcpy(last_track_title,"last");

    buffer = (char*)memalign(32,buffer_size);
    if(!buffer) exit(0);


    memset(buffer,0,buffer_size);
};


local_player::~local_player()
{
    free(buffer);
};


void inline local_player::check_filename(const char* ifilename)
{
    strcpy(track_title,ifilename);

    if(!strcmp(ifilename,last_track_title))
        return;

    // filename changed
    reset();

    strcpy(last_track_title,track_title); // Last

}

void local_player::reset()
{
    buffered        = 0;
    bufferring      = true;
    last_chunk      = 0;
    buffers_recvd   = 0;
    buffers_sent    = 0;
    lastread        = 0;
    filepos         = 0;
    fileendpos      = 0;
    filesentpos     = 0;
    at_end          = false;
    memset(buffer,0,buffer_size);
    display_name    = "";
}

int local_player::local_read(const char* ifilename)
{
    if((SDL_GetTicks()-lastread) < 50)
        return 0;

    lastread = SDL_GetTicks();

    check_filename(ifilename);

    if ((filepos+READ_SIZE_FILE >= buffer_size) && (buffers_recvd != buffers_sent))
        return 0; // we could overwrite our data


    /*
        This method of reading the file seems backwards
        but it is the only way I've found to make this work
        on wii.
    */
    FILE* wtf = 0;

    wtf = fopen(ifilename,"rb");
    if(!wtf)
    {
        return -1;
    }

    if (!fileendpos)
    {
        if (g_oripmusic) g_oripmusic = 0; // Don't rip what we already have
        fseek ( wtf , 0 , SEEK_SET );
        fseek ( wtf , 0 , SEEK_END );
        fileendpos = ftell(wtf);
        fseek ( wtf , 0 , SEEK_SET );

        filepos = id3::newfile(wtf,fileendpos);
        fileendpos -= filepos;

        if(filepos)
        {
            display_name = id3::artist;
            display_name += " - ";
            display_name += id3::title;
        }
        else
        {
            display_name = ifilename;
        }

        fseek ( wtf , 0 , SEEK_SET );
    }

    fseek ( wtf , filepos , SEEK_SET );

    int len = fread(localdata,1,READ_SIZE_FILE,wtf);

    if (len > 0)
    {
        buffer_data(localdata,len);
        filepos+=len;
    }

    status = PLAYING;

    fclose(wtf);

    return 0;
}

void inline local_player::buffer_data(const char* net_buffer,const int len)
{

    // Get the audio data
    unsigned long new_buffer_size = buffered + len;

    if (new_buffer_size > buffer_size)
    {
        int end_bytes = buffer_size - buffered;

        // copy to the end of the buffer
        if (end_bytes > 0) memcpy(buffer+buffered,net_buffer,end_bytes);

        long remain = len-end_bytes;

        // copy to the start of the buffer
        if (remain > 0 )
        {
            memcpy(buffer,net_buffer+end_bytes,remain);
            buffered = remain;

        }
        else buffered = 0;


        buffers_recvd++; // total number of buffers we've gotten for this stream
    }
    else
    {
        // keep copying to the buffer
        memcpy(buffer+buffered,net_buffer,len);
        buffered += len;
    }

    if (buffered >= pre_buffer) bufferring = false; // got enough data, change the state to allow playback

};

int local_player::check_atend(int len)
{
    if(!fileendpos) return 0; // file end has not been set yet

    filesentpos += len;
    if (filesentpos >= fileendpos)
    {
        at_end = true;
        return ((filesentpos - fileendpos) >= 0 ? (filesentpos - fileendpos) : 0) ;
    }

    return len;
}
// return a chunk of data
int local_player::get_buffer(void* cbuf,int len)
{
    if (len <= 0 || at_end) return 0;
    if (len > BUFFER_CHUNK) len = BUFFER_CHUNK;

    //need to restart
    if ((last_chunk + len) > buffer_size)
    {
        len = (buffer_size - last_chunk); // send whats left

        if (len <= 0)
        {
            buffers_sent++;
            last_chunk = 0;
            at_end = true;
            return 0; // have to reset the stream :(
        }

        memcpy(cbuf,buffer+last_chunk,len); // copy remaining

        // update and reset
        buffers_sent++;
        last_chunk = 0;

        return check_atend(len);

    }
    else if ((last_chunk + len) > buffered)
    {
        if (buffers_recvd == buffers_sent)
        {
            at_end = true;
            return 0;
        }


    }

    if (len > 0)
    {
        memcpy(cbuf,buffer+last_chunk,len);
        last_chunk += len;
    }

    return check_atend(len);

};
