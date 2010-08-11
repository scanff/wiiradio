#ifndef _LOCALFILES_H_
#define _LOCALFILES_H_

#include <dirent.h>
#include <vector>
#include "station.h"

class localfile : public station {
  public:
    string      filename;

    localfile(string name, string path, string filename);
};

localfile::localfile(string _name, string _path, string _filename) : station(_name, _path, true)
{
    filename = _filename;
}

class localfiles {
    public:
    // starting path for local dirs
    #define F_PATH_LOCAL ("music/")

    vector<localfile>   list;
    int                 total_num_files;
    string              current_path;

    localfiles() : total_num_files(0)
    {
        current_path = make_path(F_PATH_LOCAL);
    };
    ~localfiles() { clear_list(); };

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

    void clear_list()
    {
        list.clear();
    };

    void add_file(const char* filename,const char* path)
    {
        string displayname = friendly_name(filename);
        string nameandpath;

        nameandpath = path;
        nameandpath += filename;

        localfile finfo(displayname, nameandpath, filename);
        list.push_back(finfo);

    }



#ifdef _WII_
    void directory_list()
    {
      /* TODO

		clear_list(); //remove anything in list
        //rest the total playlist var
        total_num_playlists = 0;

        DIR *pdir;
        struct dirent *pent;
        struct stat statbuf;

        pdir=opendir(make_path((char*)"music/"));

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

        closedir(pdir);*/


    }
#else
    void directory_list()
    {
        clear_list(); //remove anything in list

        //rest the total playlist var
        total_num_files = 0;

        DIR *dir;
        struct dirent *ent;
        dir = opendir (current_path.c_str());

        if (dir != NULL) {

          while ((ent = readdir (dir)) != NULL) {
            if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
                continue;

            add_file(ent->d_name,current_path.c_str()); // adds a file to the list
            total_num_files++;
          }
          closedir (dir);

        } else { return; }

    };
#endif
};

#endif //_FAVORITES_H_
