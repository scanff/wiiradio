#ifndef _FAVORITES_H_
#define _FAVORITES_H_

#include <dirent.h>


struct fav_item {
    fav_item() {
        nextnode = 0;
        memset(station_name,0,SMALL_MEM);
        memset(station_url,0,SMALL_MEM);
        memset(station_path,0,SMALL_MEM);
        memset(file_name,0,SMALL_MEM);
        port=0;
    };

    char station_name[SMALL_MEM]; // name
    char station_url[SMALL_MEM]; // playing
    char station_path[SMALL_MEM]; // playing
    char file_name[SMALL_MEM]; // file
    int port;

    struct fav_item *nextnode; // pointer to next
};

class favorites {
    public:

    #define F_PATH ("pls/")

    fav_item*   first;
    fav_item*   list;


    favorites() : first(0), list(0) {};
    ~favorites() { clear_list(); };

    char* friendly_name(char* current)
    {
        char remove[10] = { '/','\\',':','*','"','\'','?','<','>','|' };

        int len = strlen(current);

        loopi(len)
        {
            loopj(10)
            {
                if ((current[i] < 0x20) || (current[i] == remove[j])) {
                    current[i] = 0x20;
                    break;
                }
            }
        }

        return current;
    };

    // make .pls compat. file
    void save_current(fav_item* c)
    {
        if (!c) return;

        char full_name[SMALL_MEM]={0};
        sprintf(full_name,"pls/%s.pls",friendly_name(c->station_name));

        FILE* f = fopen(make_path(full_name),"w");
        if (!f) return;

        fwrite("[playlist]",10,1,f);
        fwrite("\n",1,1,f);
        fwrite("numberofentries=1",17,1,f);
        fwrite("\n",1,1,f);

        fwrite("File1=http://",13,1,f);
        fwrite(c->station_url,strlen(c->station_url),1,f);
        fwrite(":",1,1,f);

        char tmp[8] = {0}; // save as ascii
        sprintf(tmp,"%d",c->port);
        fwrite(tmp,strlen(tmp),1,f);

        fwrite(c->station_path,strlen(c->station_path),1,f);
        fwrite("\n",1,1,f);

        fwrite("Title1=(#1 - 1/1) ",18,1,f);
        fwrite(c->station_name,strlen(c->station_name),1,f);
        fwrite("\n",1,1,f);

        fwrite("Length1=-1",10,1,f);
        fwrite("\n",1,1,f);

        fwrite("Version=2",9,1,f);
        fwrite("\n",1,1,f);

        fclose(f);

    };

    void clear_list()
    {
        struct fav_item* f = first;
        struct fav_item* n = 0;
        while(f) {

            n = f->nextnode;

            if (f) {
                delete f;
                f = 0;
            }

            f = n;
        }

        first = 0;
        list = 0;
    };

    struct fav_item* new_fav_item()
    {
        if (!first)
        {
            first = new fav_item;
            return first;

        }else{

            fav_item* prior = list;

            if(!list) {
                list = new fav_item;
                first->nextnode = list;
            }else{
                list = new fav_item;
                prior->nextnode = list;
            }

            return list;
        }

        return 0;
    };

    void parse_items_m3u(FILE* f,char* fname)
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

                fav_item* n = new_fav_item();

                if (n) {
                    split_url( n, station_url);
                    strcpy(n->station_name,station_name);
                    strcpy(n->file_name,fname);

                    total_num_playlists++;
                }

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

                fav_item* n = new_fav_item();

                if (n) {
                    split_url( n, station_url);
                    strcpy(n->station_name,station_name);
                    strcpy(n->file_name,fname);

                    total_num_playlists++;
                }

            }

        }
        delete[] data;
    };

    void split_url(fav_item* n,char* url)
    {
        int part = 0;
        char* split = url;
         // ignore http://
        char* search = strstr(url,"http://");
        if(!search) return;

        search += strlen("http://");

        split = strtok(search,":/");
        while(split) {

            if(!part) {
                strcpy(n->station_url,split);
            }else if(part==1) {
                n->port = atoi(split);
                if (n->port == 0) {
                    strcat(n->station_path,"/");
                    strcat(n->station_path,split);
                }
            }else{
                strcat(n->station_path,"/");
                strcat(n->station_path,split);
            }

            split = strtok(0,":/");
            part++;
        }
    }

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
            total_num_playlists++;
          }
          closedir (dir);

        } else { return; }

    };
#endif
};

#endif //_FAVORITES_H_
