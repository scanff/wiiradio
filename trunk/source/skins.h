#ifndef SKINS_H_INCLUDED
#define SKINS_H_INCLUDED

#include <dirent.h>
#include <string.h>

class skins {
    public:

    #define MAX_SKINS  (100)

    char*   skin_list[MAX_SKINS]; // cap @ 100 right now
    int     total_skins;
    int     current_skin;
    char    friendly[SMALL_MEM];

    skins() :  total_skins(0), current_skin(0)
    {
        loopi(MAX_SKINS) skin_list[i] = 0;
        find_skins();
    };
    ~skins()
    {

        loopi(MAX_SKINS)
        {
            if(skin_list[i])
            {
                delete [] skin_list[i];
                skin_list[i] = 0;
            }

        }
    }

    unsigned long get_value_color(const char* search)
    {
        char buffer[SMALL_MEM] = {0};

        char* str = get_value_string(search,buffer);

        if(!str) return 0;

        unsigned int color = 0;

        if (*str == '#') str++;

        sscanf(str, "%x", &color);

        return color;
    };


    int get_value_int(const char* search)
    {
        int value = 0;

        char* result = vars.search_var((char*)search);
        if (result) value = atoi(result);

        return value;
    };

    char* get_value_string(const char* search,char* buffer)
    {

        memset(buffer,0,SMALL_MEM); // force clear

        char* result = vars.search_var((char*)search);
        if (result) strcpy(buffer,result);

        return buffer;

    };

    char* get_value_file(const char* search,char* buffer,char* dir)
    {

        memset(buffer,0,SMALL_MEM); // force clear

        char* result = vars.search_var((char*)search);
        if (result) sprintf(buffer,"%s%s",dir,result);


        return buffer;



    };

    void parser_skin_file(char* skin_data)
    {
        char name[SMALL_MEM] = {0};
        char value[SMALL_MEM] = {0};
        char line[MED_MEM] = {0};

        char* start = skin_data;
        char* end = 0;
        char* non_ws = 0;
        char* ptr = skin_data;

        int i = 0;
        while(*ptr)
        {
            i = 0;
            // -- get a line
            while((*ptr != '\n' && *ptr != '\r') && (*ptr) && (i<MED_MEM-1))
                line[i++] = *ptr++;

            // -- parse the line
            end = non_ws = start = line;
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
                    strncpy(name,start,non_ws-start);
                    start=end+1;
                }

                if (p==i)
                {
                    strncpy(value,start,end-start);
                }

                if (line[p] != ' ')
                    non_ws++;
                end++;
            }
            if (*name && *value)
                vars.add_var(name,value);


            memset(line,0,MED_MEM);
            if (*ptr)
                ptr++;
        }

    };

    bool load_skin(char* dir)
    {
        FILE* f = 0;
        char fullpath[SMALL_MEM] = {0};

        sprintf(fullpath,"%sskin.txt",dir);

        f = fopen(make_path(fullpath),"r");
        if (!f) return false;

        fseek(f,0,SEEK_END);
        unsigned int size = ftell(f);
        fseek(f,0,SEEK_SET);


        char* skin_data = new char[size+1];
        if (!skin_data)
        {
            fclose(f);
            return false;
        }

        memset(skin_data,0,size+1);

        fread(skin_data,size,1,f);

        parser_skin_file(skin_data);

        delete [] skin_data;

        fclose(f);


        return true;
    };



    void find_skins()
    {

        DIR *dir,*check;
        struct dirent *ent;

        char path[SMALL_MEM]={0};

        total_skins = 0; // clear

        dir = opendir (make_path((char*)"skins/"));

        if (dir != NULL) {

        while ((ent = readdir (dir)) != NULL)
        {

          if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
          {
              continue;
          }

            // -- check this is a skin dir....

            sprintf(path,"skins/%s",ent->d_name);
            check = 0;
            check = opendir (make_path(path));

            if (check)
            {

              while ((ent = readdir (check)) != NULL)
              {
                  string_lower(ent->d_name);

                   if(strcmp(ent->d_name,"skin.txt") == 0) //ok this is a skin dir
                   {

                        char* p = new char[SMALL_MEM];
                        if (!p) break;

                        memset(p,0,SMALL_MEM);

                        strcpy(p,path);

                        skin_list[total_skins] = p;

                        total_skins++;
                        break;
                   }

              }
              closedir (check);

            }

        }
          closedir (dir);

        } else { return; }


    };

    // -- get_current_skin()
    // -- return the skin list
    char* get_current_skin()
    {
        // return a friendly name
        memset(friendly,0,SMALL_MEM);
        char* c = skin_list[current_skin];
        while(*c++ != '/' && *c != 0);

        strcpy(friendly,c);

        string_lower(friendly);

        return friendly;
    };

    char* get_current_skin_path()
    {
        if (current_skin < 0 || current_skin >= total_skins) return 0; //safety

        char *value = new char[SMALL_MEM];
        snprintf(value,SMALL_MEM-1,"%s/",skin_list[current_skin]);
        value[SMALL_MEM-1] = '\0';

        return value;
    };

    void goto_next_skin()
    {
        if (current_skin < total_skins-1) current_skin++;
        else current_skin = 0;
    };

    void set_current_skin(char* path)
    {
        char search_value[SMALL_MEM] = {0};

        loopi(total_skins)
        {
            sprintf(search_value,"%s/",skin_list[i]);
            if(strcmp(search_value,path) == 0) {
                current_skin = i;
                break;
            }
        }
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

#endif // SKINS_H_INCLUDED
