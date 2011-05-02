#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <dirent.h>
#include <sys/stat.h>
#include "application.h"

class langs
{
    public:

    #define MAX_LANGS  (50)

    char*   lang_list[MAX_LANGS]; // cap @ 50 right now
    int     total_lang;
    int     current_lang;
    char    current_name[SMALL_MEM];

    app_wiiradio* theapp;

    langs(app_wiiradio* _theapp) :  total_lang(0), current_lang(0), theapp(_theapp)
    {
        loopi(MAX_LANGS) lang_list[i] = 0;
        find_langs();
    };

    ~langs()
    {

        loopi(MAX_LANGS)
        {
            if(lang_list[i])
            {
                delete [] lang_list[i];
                lang_list[i] = 0;
            }
        }

    }

    void parser_lang_file(char* data)
    {
        char name[SMALL_MEM] = {0};
        char value[SMALL_MEM] = {0};
        char line[MED_MEM] = {0};

        char* start = data;
        char* end = 0;
        char* ptr = data;

        int i = 0;
        while(*ptr)
        {
            i = 0;
            // -- get a line
            while((*ptr != '\n' && *ptr != '\r') && (*ptr) && (i<MED_MEM))
                line[i++] = *ptr++;

            // -- parse the line
            end = start = line;
            memset(name,0,SMALL_MEM);
            memset(value,0,SMALL_MEM);

            for(int p=0;p<=i;p++)
            {
                if (line[p] == '/')
                {
                    end = line+p;
                    if (end == start) break;
                }

                if (line[p] == '=')
                {
                    strncpy(name,start,end-start);
                    start=end+1;
                }

                if (p==i)
                {
                    strncpy(value,start,end-start);
                }

                end++;
            }
            if (*name && *value)
                theapp->GetVariables()->add_var(name,value);


            memset(line,0,MED_MEM);
            ptr++;
        }

    };

    bool load_lang(char* lang)
    {
        FILE* f = 0;
        char fullpath[SMALL_MEM] = {0};

        sprintf(fullpath,"data/lang/%s.txt",lang);

        f = fopen(make_path(fullpath),"rb");
        if (!f) return false;

        fseek(f,0,SEEK_END);
        unsigned int size = ftell(f);
        fseek(f,0,SEEK_SET);


        char* data = new char[size+1];
        if (!data)
        {
            fclose(f);
            return false;
        }

        memset(data,0,size+1);

        fread(data,1,size,f);

        parser_lang_file(data);

        delete [] data;

        fclose(f);


        set_current_lang(lang);

        return true;
    };



    void find_langs()
    {

        DIR *dir;
        struct dirent *ent;

        char name[SMALL_MEM]={0};

        total_lang = 0; // clear

        dir = opendir (make_path((char*)"data/lang/"));

        if (dir != NULL) {

        while ((ent = readdir (dir)) != NULL)
        {

            if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            {
              continue;
            }

            // -- save the lang. file

            char* p = new char[SMALL_MEM];
            if (!p) break;

            memset(p,0,SMALL_MEM);

            int i = 0;
            char* c = ent->d_name;

            while(*c && *c != '.' && i < SMALL_MEM-1)
                name[i++] = *c++;

            string_lower(name); // store as lower
            name[0] = toupper(name[0]); // first char upper

            strcpy(p,name);

            lang_list[total_lang] = p;

            total_lang++;

            memset(name,0,SMALL_MEM);

        }
        closedir (dir);

        } else { return; }


    };


    void goto_next_lang()
    {
        if (current_lang < total_lang-1) current_lang++;
        else current_lang = 0;

        strcpy(current_name,lang_list[current_lang]);
    };

    void set_current_lang(char* lang)
    {
        loopi(total_lang)
        {
            if(strcmp(lang_list[i],lang) == 0) {
                current_lang = i;
                break;
            }
        }

        strcpy(current_name,lang_list[current_lang]);
    };

    char* string_lower(char* x)
    {
        char* p = x;

        while(*p) {
          *p = tolower(*p);
          p++;
        }

        return x;
    };


};


#endif // LANG_H_INCLUDED
