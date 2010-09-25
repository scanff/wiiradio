#include "globals.h"
#include "ripper.h"


#include <sys/stat.h>
#include <dirent.h>

stream_ripper::stream_ripper() : rip_file(0)
{
    // check dir
    if(!opendir (make_path("music")))
    {
#ifdef _WIN32
        // no mode on windows
        mkdir(make_path("music"));
#else
        mkdir(make_path("music"),0777);
#endif
    }

    memset(curren_file,0,MED_MEM);
};

stream_ripper::~stream_ripper()
{
    close_file();
};

void stream_ripper::new_file(char* name)
{
    if (*name == 0)
        return;

    if (strcmp(curren_file,name) == 0)
        return; // same track !

    strcpy(curren_file,name);

    close_file();

    char path[MED_MEM];

    sprintf(path,"%s%s.mp3",make_path("music/"),name);

    // -- Check the file does not exsit
     int t = 0;
     struct stat fi;
     while(stat((const char*)path,&fi),t++)
     {
        // -- TRY AGAIN
        sprintf(path,"%s%s(%d).mp3",make_path("music/"),name,t);
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

    fwrite(data,1,len,rip_file);
}

