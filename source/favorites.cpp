
#include "globals.h"
#include "textures.h"
#include "fonts.h"
#include "application.h"
#include "favorites.h"

#include <dirent.h>
#include <vector>
#include "station.h"

// a favorite
favorite::favorite(string _name, string _url, string _filename) : station(_name, _url)
{
    filename = _filename;
}
favorite::favorite(string _name, string _url, string _filename, bool local) : station(_name, _url, local)
{
    filename = _filename;
}


// all favorites
favorites::favorites(app_wiiradio* _theapp) : theapp(_theapp), position(0) {};
favorites::~favorites() { clear_list(); };

void* favorites::GetItem( const u32 i )
{
    const u32 p = i + position;

    if(p < list.size())
        return (void*)&(list[p]);

    return NULL;
}

const char* favorites::GetItemText(const u32 i)
{
     const u32 p = i + position;

    if(p < list.size())
        return (list[p].name.c_str());

    return NULL;
}

void favorites::SetMax(const int m) {view_max = m; }
const int favorites::GetPosition() { return position; }
void favorites::MoveNext()
{
    if (position + view_max > total)
        return;

    position += view_max;
}

void favorites::MoveBack()
{
    if (position - view_max <= 0)
    {
        position = 0;
        return;
    }

    position -= view_max;
}
const int favorites::GetSize() { return total; }

void favorites::delete_favorite(unsigned int item)
{
    if (item >= list.size())
        return;
    vector<favorite>::iterator it = list.begin();
    unsigned int i=0;
    while (i++ < item) ++it;

    remove(list[item].filename.c_str());
    list.erase(it);
    total--;
}

string favorites::friendly_name(string s)
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
void favorites::save_pls(string full_name, station s)
{
    FILE* f = fopen(make_path(full_name.c_str()),"wb");
    if (!f) return;

    fwrite("[playlist]",10,1,f);
    fwrite("\n",1,1,f);
    fwrite("numberofentries=1",17,1,f);
    fwrite("\n",1,1,f);

    if(s.local)
    {
        fwrite("File1=",6,1,f);
        fwrite(s.path.c_str(),s.path.length(),1,f);

    }else{
        fwrite("File1=http://",13,1,f);
        fwrite(s.url().c_str(),s.url().length(),1,f);
    }

    fwrite("\n",1,1,f);

    fwrite("Title1=(#1 - 1/1) ",18,1,f);
    fwrite(s.name.c_str(),s.name.length(),1,f);
    fwrite("\n",1,1,f);

    fwrite("Length1=-1",10,1,f);
    fwrite("\n",1,1,f);

    fwrite("Version=2",9,1,f);
    fwrite("\n",1,1,f);

    fclose(f);

}

void favorites::save_fav(station s)
{
    string full_name = "data/pls/"+friendly_name(s.name)+".pls";
    save_pls(full_name, s);
}

void favorites::save_current(station s)
{
    if(playback_type == AS_NETWORK)
        save_pls("data/current.pls", s);
}

void favorites::clear_list()
{
    list.clear();
}

void favorites::parse_items_m3u(FILE* f, const char* fname)
{
    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);

    char* data = new char[size+1];
    fread(data,1,size,f);
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
                // save our locals as .pls
                favorite fav(name, url, fn, false);
                list.push_back(fav);

                total++;
        }

    }
    delete[] data;
}

vector<favorite> favorites::parse_items_pls(FILE* f, const char* fname)
{
    vector<favorite> ret;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);

    char* data = new char[size+1];
    fread(data,1,size,f);
    data[size] = '\0';


    char* start = data;
    char* end = 0;

    while(start)
    {
        char station_name[MED_MEM] = {0};
        char station_url[MED_MEM] = {0};

       //url
        start = strstr(start,"File1=");
        if (start) {
            start += strlen("File1=");
            end = strstr(start,"\n");
            if (end)
            {
                const int size = end - start;
                strncpy(station_url,start,size);
                station_url[size]='\0';

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
                string name     = station_name;
                string url      = station_url;
                string fn       = fname;
                bool local    = true;

                if((station_url[0] == 'h' && station_url[1] == 't' &&
                   station_url[2] == 't' && station_url[3] == 'p'))
                   local = false;

                if(!local)
                {
                    favorite fav(name, url, fn);
                    ret.push_back(fav);
                }else{
                    favorite fav(name, url, fn, local);
                    ret.push_back(fav);
                }

        }

    }
    delete[] data;
    return ret;
}

int favorites::get_fav_type(FILE* f)
{
    int type = -1;
    char peek_buf[20] = {0};

    fread(peek_buf,1,19,f);

    if (strstr(peek_buf,"[playlist]")) {
        type = 0; // .pls
    }else if (strstr(peek_buf,"#EXTM3U")) {
        type = 1; // . m3u
    }

    // set stream back to start
    fseek(f,0,SEEK_SET);

    return type;
}

void favorites::load_file(const char* fn)
{

    char full_name[SMALL_MEM]={0};
    sprintf(full_name,"data/pls/%s",fn);

    FILE * file = fopen(make_path(full_name),"rb");
    if (!file) {
        return;
    }

    // pls parser
    int fav_type = get_fav_type(file);
    vector<favorite> parsed;

    switch (fav_type)
    {
        case 0:
        {
            parsed = parse_items_pls(file,make_path(full_name)); // .pls
            for (unsigned int i=0; i<parsed.size(); i++)
                list.push_back(parsed[i]);

            total+=parsed.size();
        }
        break;

        case 1:
        parse_items_m3u(file,make_path(full_name));// m3u
        break;
    }

    fclose(file);

}


#ifdef _WII_
void favorites::load_favorites()
{
    clear_list(); //remove anything in list
    //rest the total playlist var
    total = 0;

    DIR *pdir;
    struct dirent *pent;
    struct stat statbuf;

    pdir=opendir(make_path((char*)"data/pls/"));

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
void favorites::load_favorites()
{
    clear_list(); //remove anything in list

    //rest the total playlist var
    total = 0;

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

}
#endif
