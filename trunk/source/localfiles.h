#ifndef _LOCALFILES_H_
#define _LOCALFILES_H_

#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include "station.h"

class localfile : public station {
  public:
    string      filename;
    bool        isfolder;

    localfile(){};
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

    static int sort_files(const localfile & a, const localfile & b)
    {
        if (a.isfolder && !b.isfolder)
            return true;
        else if (b.isfolder && !a.isfolder)
            return false;

        return stricmp((char *) a.filename.c_str(), (char *) b.filename.c_str());
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

    void inline clear_list()
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

        if((unsigned int)(total_num_files) >= list.max_size())
            list.resize(total_num_files + 20);

        list.push_back(finfo);

        return true;
    }

    void dirup()
    {
        // This string should contain a / at the end so remove it so our find last will find the prior /
        string new_path = (current_path.c_str());
        new_path.erase (new_path.end()-1, new_path.end());

        int found = new_path.find_last_of("/\\");
        if(found == -1) // Must be at root
            return;

        new_path = new_path.substr(0,found);
        new_path += "/"; // add the path slash back
        directory_list(new_path.c_str());
    }

    void directory_list(const char* new_path)
    {

        if (!new_path) current_path = make_path(F_PATH_LOCAL);
        else current_path = new_path; // make_path already done if !new path

        // save as system variable
        vars.add_var("$LOCAL_FILE_PATH", (char*)current_path.c_str());

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

        //re-sort
        if (total_num_files > 1)
            sort (list.begin(), list.end(),sort_files);
    };

};

#endif //_FAVORITES_H_
