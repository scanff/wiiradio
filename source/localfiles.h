#ifndef _LOCALFILES_H_
#define _LOCALFILES_H_

#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include "station.h"

class localfile : public station {
  public:
    string      filename;
    bool        isfolder;

    localfile(string name, string path, string filename, bool folder);
};

localfile::localfile(string _name, string _path, string _filename, bool _isfolder) : station(_name, _path, true)
{
    filename = _filename;
    isfolder = _isfolder;
}

class localfiles {
    public:
    // starting path for local dirs
    #define F_PATH_LOCAL ("music/")

    vector<localfile>   list;
    int                 total_num_files;
    string              current_path;
    int                 current_position;

    localfiles() : total_num_files(0), current_position(0)
    {
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

    bool add_file(const char* filename,const char* path,const bool folder)
    {

        if (!folder)
        {
            int len = strlen(filename) - 1;
            if (filename[len]   != '3' ||
                filename[len-1] != 'p' ||
                filename[len-2] != 'm' ||
                filename[len-3] != '.')
                return false;

        }

        string displayname = friendly_name(filename);
        string nameandpath;

        nameandpath = path;
        nameandpath += filename;
        if(folder) nameandpath += "/";

        localfile finfo(displayname, nameandpath, filename,folder);
        list.push_back(finfo);

        return true;
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
    void directory_list(const char* new_path)
    {
        if (!new_path) current_path = make_path(F_PATH_LOCAL);
        else current_path = make_path(new_path);

        clear_list(); //remove anything in list

        //rest the total playlist var
        current_position = total_num_files = 0;

        DIR *dir;
        struct dirent *ent;
        struct stat fstat; //mingwin does not have a d_type (folder/file) have to use stat to see if folder or file on windoze
        bool isfolder;
        string fullname;

        dir = opendir (current_path.c_str());

        if (dir != NULL) {

          while ((ent = readdir (dir)) != NULL) {
            if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
                continue;

            isfolder = false;
            fullname = current_path.c_str();
            fullname += ent->d_name;

            stat(fullname.c_str(),&fstat);

            if (fstat.st_mode & S_IFDIR)
                isfolder = true;

            if(add_file(ent->d_name,current_path.c_str(),isfolder)) // adds a file to the list
                total_num_files++;
          }
          closedir (dir);

        } else { return; }

    };
#endif
};

#endif //_FAVORITES_H_
