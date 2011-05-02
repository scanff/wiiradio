#include "globals.h"
#include "options.h"
#include "id3.h"
#include "application.h"
#include "local.h"
#include "localfiles.h"

static SDL_mutex* file_access_m = 0;

local_player::local_player(app_wiiradio* _theapp) :
    theapp(_theapp),
    m_id3(_theapp),
    filepos(0),
    fileendpos(0),
    current_fp(0)

{
    strcpy(last_track_title,"last");
    strcpy(track_title,"");
    strcpy(time_elap_str,"00:00");

     file_access_m = SDL_CreateMutex();
}


local_player::~local_player()
{
    SDL_DestroyMutex(file_access_m);
}

void local_player::close()
{
    if(current_fp)
    {
        fclose(current_fp);
        current_fp = 0;
    }
}

u32 local_player::get_size()
{
    return fileendpos;
}

void local_player::seek(const u32 pos)
{
    u32 p = pos;

    if (p > fileendpos) p = fileendpos;

    SDL_mutexP(file_access_m);

    fseek ( current_fp , 0 , SEEK_SET );
    fseek ( current_fp , p , SEEK_SET );

    SDL_mutexV(file_access_m);
}

int local_player::read(u8* buf,int size)
{


    SDL_mutexP(file_access_m);

    int rd = 0;

    if(current_fp)
    {
        rd = fread(buf,1,size,current_fp);
        if(rd > 0) bytes_done += rd;
        else done = true;

        if (bytes_done >= fileendpos) done = true;
    }

    SDL_mutexV(file_access_m);

    return rd;

}

int local_player::open(const char* ifilename)
{
#ifdef _WII_
	int i = 0;
    char device[TINY_MEM+1];

    for(i =0; i < TINY_MEM - 1;i++)
    {
        device[i] = ifilename[i];
        if (ifilename[i] == ':')
        {
            break;
        }
    }
    device[i+1] = '\0';

    if(!theapp->check_media((const char*)device))
        return 0;

#endif

    close();

    bytes_done = 0;
    done = false;

    strcpy(last_track_title,track_title); // Last
    strcpy(track_title,ifilename);

    current_fp = fopen(ifilename,"rb");

    if(!current_fp) return 0;


   // if (theapp->settings.oripmusic) theapp->settings.oripmusic = 0; // Don't rip what we already have

    fseek ( current_fp , 0 , SEEK_SET );
    fseek ( current_fp , 0 , SEEK_END );
    fileendpos = ftell(current_fp);
    if (fileendpos <= 0)
    {
        fclose(current_fp);
        return 0;
    }
    fseek ( current_fp , 0 , SEEK_SET );

    filepos = m_id3.newfile(current_fp,fileendpos);
    fileendpos -= filepos;

    if(filepos)
    {
        display_name = m_id3.artist;
        display_name += " - ";
        display_name += m_id3.title;
    }
    else
    {
        display_name = ifilename;
    }


    theapp->GetFileDir()->set_playing_dir();

    fseek ( current_fp , 0 , SEEK_SET );
    fseek ( current_fp , filepos , SEEK_SET );


    return 1;

}
