#ifndef _FAVORITES_H_
#define _FAVORITES_H_

#include <dirent.h>
#include <vector>
#include "station.h"

class favorite : public station {
  public:
    string      filename;

    favorite(string name, string url, string filename);
};

favorite::favorite(string _name, string _url, string _filename) : station(_name, _url)
{
    filename = _filename;
}

class favorites {
    public:

    #define F_PATH ("pls/")

    vector<favorite> list;

    favorites() {};
    ~favorites() { clear_list(); };

    void delete_favorite(unsigned int item)
    {
        if (item >= list.size())
            return;
        vector<favorite>::iterator it = list.begin();
        unsigned int i=0;
        while (i++ < item) ++it;

        remove(list[item].filename.c_str());
        list.erase(it);
        total_num_playlists--;
    }

    string friendly_name(string s)
    {
        char remove[10] = { '/','\\',':','*','"','\'','?','<','>','|' };

        loopi(s.length())
        {
            loopj(10)
            {
                if ((s[i] < 0x20) || (s[i] == remove[j])) {
                    s[i] = 0x20;
                    break;
                }
            }
        }

        return s;
    };

    // make .pls compat. file
    void save_current(station s)
    {
        string full_name = "pls/"+friendly_name(s.name)+".pls";

        FILE* f = fopen(make_path(full_name.c_str()),"w");
        if (!f) return;

        fwrite("[playlist]",10,1,f);
        fwrite("\n",1,1,f);
        fwrite("numberofentries=1",17,1,f);
        fwrite("\n",1,1,f);

        fwrite("File1=http://",13,1,f);
        fwrite(s.url().c_str(),s.url().length(),1,f);
        fwrite(":",1,1,f);

        char tmp[8] = {0}; // save as ascii
        sprintf(tmp,"%d",s.port);
        fwrite(tmp,strlen(tmp),1,f);

        fwrite(s.path.c_str(),s.path.length(),1,f);
        fwrite("\n",1,1,f);

        fwrite("Title1=(#1 - 1/1) ",18,1,f);
        fwrite(s.name.c_str(),s.name.length(),1,f);
        fwrite("\n",1,1,f);

        fwrite("Length1=-1",10,1,f);
        fwrite("\n",1,1,f);

        fwrite("Version=2",9,1,f);
        fwrite("\n",1,1,f);

        fclose(f);

    };

    void clear_list()
    {
        list.clear();
    };

    void parse_items_m3u(FILE* f, char* fname)
    {
        fseek(f,0,SEEK_END);
        unsigned int size = ftell(f);
        fseek(f,0,SEEK_SET);

        char* data = new char[size+1];
        fread(data,size,1,f);
        data[size] = '\0';

        char* start = data;
        char* end = 0;

        while(start)
        {
            char station_name[512] = {0};
            char station_url[512] = {0};
            char* save_start = 0;

            start = strstr(start,"#EXTINF:");
            save_start = start;
            if (!start) break;
            // name, seems to be wrapped in ( ) from m3u's created by winamp on streams
            start = strstr(start,"(");

            if (start) {
                start += 1;
                end = strstr(start,")");
                if (end) {
                    memcpy(station_name,start,end-start);
                }
            }else{

                if (save_start)
                {
                    // just in case take the next char after the comma
                    start = strstr(save_start,",");
                    if (start) {
                        start += 1;
                        end = strstr(start,"\n");
                        if (end) {
                            memcpy(station_name,start,end-start);
                        }
                    }

                }else break; // ??? not valid then .. eh?

            }



            //url
            start = strstr(start,"\n");
            if (start) {
                start += strlen("\n");
                end = strstr(start,"\n");
                if (end) {
                    memcpy(station_url,start,end-start);
                }
            }else break;


            //add to list
            if (strlen(station_url)>0
                && strlen(station_name)>0
                ) {
                    string name = station_name;
                    string url  = station_url;
                    string fn   = fname;
                    favorite fav(name, url, fn);
                    list.push_back(fav);

                    total_num_playlists++;
            }

        }
        delete[] data;
    };

    void parse_items_pls(FILE* f,char* fname)
    {
        fseek(f,0,SEEK_END);
        unsigned int size = ftell(f);
        fseek(f,0,SEEK_SET);

        char* data = new char[size+1];
        fread(data,size,1,f);
        data[size] = '\0';


        char* start = data;
        char* end = 0;

        while(start)
        {
            char station_name[SMALL_MEM] = {0};
            char station_url[SMALL_MEM] = {0};

           //url
            start = strstr(start,"File1=");
            if (start) {
                start += strlen("File1=");
                end = strstr(start,"\n");
                if (end) {
                    memcpy(station_url,start,end-start);
                }
            }else break;

            //STATION NAME
            start = strstr(start,"Title1=");
            if (start) {
                char* save_start = 0;
                start += strlen("Title1=");
                save_start = start;
                //bypass the () which tells the stream number of
                start = strstr(start,")");
                if (start) {
                    start += 1;
                    end = strstr(start,"\n");
                    if (end) {
                        memcpy(station_name,start,end-start);
                    }
                }else{
                    if (save_start)
                    {
                        end = strstr(save_start,"\n");
                        if (end) {
                            memcpy(station_name,save_start,end-save_start);
                        }
                    }else break;
                }
            }else break;


            //add to list
            if (strlen(station_url)>0
                && strlen(station_name)>0
                ) {
                    string name = station_name;
                    string url  = station_url;
                    string fn   = fname;
                    favorite fav(name, url, fn);
                    list.push_back(fav);

                    total_num_playlists++;
            }

        }
        delete[] data;
    };

    int get_fav_type(FILE* f)
    {
        int type = -1;
        char peek_buf[20] = {0};

        fread(peek_buf,19,1,f);

        if (strstr(peek_buf,"[playlist]")) {
            type = 0; // .pls
        }else if (strstr(peek_buf,"#EXTM3U")) {
            type = 1; // . m3u
        }

        // set stream back to start
        fseek(f,0,SEEK_SET);

        return type;
    };

    void load_file(const char* fn)
    {

        char full_name[SMALL_MEM]={0};
        sprintf(full_name,"pls/%s",fn);

        FILE * file = fopen(make_path(full_name),"r");
        if (!file) {
            return;
        }

        // pls parser
        int fav_type = get_fav_type(file);

        switch (fav_type)
        {
            case 0:
            parse_items_pls(file,make_path(full_name)); // .pls
            break;

            case 1:
            parse_items_m3u(file,make_path(full_name));// m3u
            break;
        }

        fclose(file);

    };
#ifdef _WII_
    void load_favorites()
    {
        clear_list(); //remove anything in list
        //rest the total playlist var
        total_num_playlists = 0;

        DIR *pdir;
        struct dirent *pent;
        struct stat statbuf;

        pdir=opendir(make_path((char*)"pls/"));

        if (!pdir){
            return;
        }

        while ((pent=readdir(pdir))!=NULL) {

            stat(pent->d_name,&statbuf);
            if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
            {
                memset(pent->d_name,0,NAME_MAX+1);
                continue;
            }
          //  if(S_ISDIR(statbuf.st_mode)) // no dirs sorry
          //      continue; //printf("%s <dir>\n", pent->d_name);

            //if(!(S_ISDIR(statbuf.st_mode))){

                load_file(pent->d_name);

                memset(pent->d_name,0,NAME_MAX+1);
            //}
        }

        closedir(pdir);
    }
#else
    void load_favorites()
    {
        clear_list(); //remove anything in list

        //rest the total playlist var
        total_num_playlists = 0;

        DIR *dir;
        struct dirent *ent;
        dir = opendir (make_path(F_PATH));

        if (dir != NULL) {

          while ((ent = readdir (dir)) != NULL) {
            if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
                continue;

            load_file(ent->d_name);
          }
          closedir (dir);

        } else { return; }

    };
#endif
};

#endif //_FAVORITES_H_
