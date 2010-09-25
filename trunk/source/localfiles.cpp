#include "globals.h"
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include "station.h"
#include "localfiles.h"

static int sort_files(const void * ia, const void * ib)
{
    const localfile* a = (localfile*)ia;
    const localfile* b = (localfile*)ib;

    if (a->isfolder && !b->isfolder)
        return -1;

    if (b->isfolder && !a->isfolder)
        return 1;

    return stricmp((char *)(localfile*)a->filename.c_str(), (char *)(localfile*)b->filename.c_str());
}

localfile::localfile(string _name, string _path, string _filename, bool _isfolder)
: station(_name, _path, true)
{
    filename = _filename;
    isfolder = _isfolder;
}

localfiles::localfiles() :
    total_num_files(0),
    current_position(0)
{
}

string localfiles::friendly_name(string s)
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
}

void inline localfiles::clear_list()
{
    list.clear();
}

bool inline localfiles::add_file(const char* filename,const char* path,const bool folder)
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

void localfiles::dirup()
{
    // This string should contain a / at the end so remove it so our find last will find the prior /
    string new_path = current_path;
    new_path.erase (new_path.end()-1, new_path.end());

    int found = new_path.find_last_of("/\\");
    if(found == -1) // Must be at root
        return;

    new_path = new_path.substr(0,found);
    new_path += "/"; // add the path slash back
    directory_list(new_path.c_str());
}

void localfiles::directory_list(const char* new_path)
{

    if (!new_path) sprintf(current_path,"%s",make_path(F_PATH_LOCAL));
    else sprintf(current_path,"%s",new_path); // make_path already done if !new path

    // save as system variable
    vars.add_var("$LOCAL_FILE_PATH", current_path);

    clear_list(); //remove anything in list

    //rest the total playlist var
    current_position = total_num_files = 0;

    DIR *dir;
    struct dirent *ent;
    struct stat fstat; //mingwin does not have a d_type (folder/file) have to use stat to see if folder or file on windoze
    bool fld = false;
    #define FILE_LEN_GUARD (200)
    char fullname[FILENAME_MAX+FILE_LEN_GUARD]; // guarded
    memset(fullname,0,FILENAME_MAX+FILE_LEN_GUARD);

    dir = opendir (current_path);

    if (dir != 0)
    {

    while ((ent = readdir (dir)) != 0)
    {
        if(!ent) continue;

       if((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0))
            continue; // don't add cd .. cd / we will handle this ourselves


        sprintf(fullname,"%s%s",current_path,ent->d_name);

        stat(fullname,&fstat);

        if(add_file(ent->d_name,current_path,(fstat.st_mode & S_IFDIR))) // adds a file to the list
            total_num_files++;

        memset(fullname,0,FILENAME_MAX+FILE_LEN_GUARD);
      }
      closedir (dir);

    } else { return; }

    //re-sort
    if (total_num_files > 1)
    {
        //sort(local_file_list.begin(), local_file_list.end(), sort_files);
        qsort ((void*)&list[0], list.size(), sizeof(localfile), sort_files);
    }
}

