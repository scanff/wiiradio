#include "globals.h"
#include "ripper.h"
#include <sys/stat.h>
#include <dirent.h>

stream_ripper::stream_ripper() : rip_file(0)
{
    // check dir
    if(!opendir (make_path("wr_music_rip")))
    {
#ifdef _WIN32
        // no mode on windows
        mkdir(make_path("wr_music_rip"));
#else
        mkdir(make_path("wr_music_rip"),0777);
#endif
    }

    memset(curren_file,0,MED_MEM);
}

stream_ripper::~stream_ripper()
{
    close_file();
}

const char* stream_ripper::friendly_name(const char* s)
{
    static char newname[SMALL_MEM];
    memset(newname,0,SMALL_MEM);
    const char remove[12] = { ',','.','/','\\',':','*','"','\'','?','<','>','|' };
    int len = strlen(s);
    int start = 0;

    // limit the file name len
    if(len > 50)
    {
        start = len-50;
        len = 50;
    }

    // remove un-friendly chars
    loopi(len)
    {
        loopj(12)
        {
            if ((s[i] < 0x20) || (s[i] == remove[j]))
            {
                newname[i] = 0x20;
                break;
            }else newname[i] = s[i+start];
        }
    }



    return newname;
}

void stream_ripper::new_file(char* name)
{
    if (*name == 0)
        return;

    if (strcmp(curren_file,name) == 0)
        return; // same track !

    strcpy(curren_file,name);

    close_file();

    char path[MED_MEM];

    sprintf(path,"%s%s.mp3",make_path("wr_music_rip/"),friendly_name(name));

    // -- Check the file does not exsit
    int t = 0;
    struct stat fi;
    while(stat((const char*)path,&fi),t++)
    {
        // -- TRY AGAIN
        sprintf(path,"%s%s-%d.mp3",make_path("wr_music_rip/"),friendly_name(name),t);
        if(t > 20) break;
    }

    rip_file = fopen(path,"wb");

}

void stream_ripper::close_file()
{
    if (!rip_file) return;

    fclose(rip_file);
    rip_file = 0;
}

void stream_ripper::save_data(char* data,int len)
{
    if (!rip_file) return;

    fwrite(data,len,1,rip_file);
}

