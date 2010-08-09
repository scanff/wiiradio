#ifndef _LOCAL_PLAYER_H_
#define _LOCAL_PLAYER_H_

class local_player
{
public:


    char            track_title[SMALL_MEM]; // track info
    char            last_track_title[SMALL_MEM]; // track info - last

    // vars
    unsigned long   metaint_pos;
    unsigned long   buffered;
    unsigned long   pre_buffer; // buffer before playback ...
    unsigned long   buffer_size;
    char*           buffer;
    bool            bufferring;

    unsigned long   last_chunk;
    unsigned long   buffers_recvd;
    unsigned long   buffers_sent;

    unsigned long   lastread;
    unsigned long   filepos;
    unsigned long   fileendpos;
    unsigned long   filesentpos;
    bool            at_end;


public:

    local_player() :
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


    ~local_player()
    {
        free(buffer);
    };

    void reset()
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
    }

    void local_read(const char* filename)
    {
        if((SDL_GetTicks()-lastread) < 100) return;
        lastread = SDL_GetTicks();


        if ((filepos+4000 >= buffer_size) && (buffers_recvd != buffers_sent))
            return; // we could overwrite our data


        /*
            This method of reading the file seems backwards
            but it is the only way I've found to make this work
            on wii.
        */
        /*static*/ FILE* wtf = 0;

        wtf = fopen(filename,"rb");
        if(!wtf) return;

        if (!fileendpos)
        {
            fseek ( wtf , 0 , SEEK_SET );
            fseek ( wtf , 0 , SEEK_END );
            fileendpos = ftell(wtf);
            fseek ( wtf , 0 , SEEK_SET );
        }

        fseek ( wtf , filepos , SEEK_SET );

        /*static*/ char localdata[4000];
        int len = fread(localdata,1,4000,wtf);

        if (len > 0)
            buffer_data(localdata,len);

        filepos+=len;
        fclose(wtf);

        return;
    }

    void inline buffer_data(char* net_buffer,int len)
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

    int inline check_atend(int len)
    {
        if(!fileendpos) return len; // file end has not been set yet

        filesentpos+=len;
        if (filesentpos>=fileendpos) at_end = true;

        return len;
    }
    // return a chunk of data
    int get_buffer(void* cbuf,int len)
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
                // reset ... need to rebuffer !!!!
                // check your signal strength :P
                //icy_stream_reset();

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

};

#endif

