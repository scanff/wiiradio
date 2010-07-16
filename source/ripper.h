#ifndef RIPPER_H_INCLUDED
#define RIPPER_H_INCLUDED

class stream_ripper
{
public:

    FILE*   rip_file;
    char    curren_file[MED_MEM];

    stream_ripper() : rip_file(0)
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

    ~stream_ripper()
    {
        close_file();
    };

    void new_file(char* name)
    {
        if (*name == 0)
            return;

        if (strcmp(curren_file,name) == 0)
            return; // same track !

        strcpy(curren_file,name);

        close_file();

        char path[MED_MEM];

        sprintf(path,"%s%s.mp3",make_path("music/"),name);

        rip_file = fopen(path,"wb");

    }

    void close_file()
    {
        if (!rip_file) return;

        fclose(rip_file);
        rip_file = 0;
    }

    void save_data(char* data,int len)
    {
        if (!rip_file) return;

        fwrite(data,1,len,rip_file);
    }

};


#endif // RIPPER_H_INCLUDED
