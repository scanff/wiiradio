#include "globals.h"
#include "application.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include "station.h"
#include "script.h"
#include "localfiles.h"

// use DIR_ITER in Wii as standard DIR is really slow.
#define MAX_FILE_ITEMS (2500) // Maximum number of files per dir
SDL_mutex*   listviews_mutex;
static int bTermThread = 0;

static FORCEINLINE void swap(lfile *x,lfile *y)
{
   lfile temp;
   memcpy(&temp,x,sizeof(lfile));
   memcpy(x,y,sizeof(lfile));
   memcpy(y,&temp,sizeof(lfile));
}

static FORCEINLINE int compare(const lfile *ia, const lfile *ib)
{
    const lfile* a = (lfile*)ia;
    const lfile* b = (lfile*)ib;

    if (a->isfolder && !b->isfolder)
        return -1;

    if (b->isfolder && !a->isfolder)
        return 1;

    return strcasecmp(a->name, b->name);
}

// background process to find and add local music .. Mainly added as USB is too slow to add on the fly
// THIS IS STILL VERY SLOW AND EFFECTS THE OTHER THREADS  !!!! I Was testing on a dir with 2200 files in tho.
static int finder_thread(void* arg)
{
    localfiles* lf = (localfiles*)arg;

    while(!bTermThread)
    {
        usleep(2000);

        if(!lf->parsing_dir)
            continue;


        #ifdef _WII_
            if (lf->dir) dirclose (lf->dir);
            lf->dir = diropen (lf->current_path);
        #else
            if (lf->dir) closedir (lf->dir);
            lf->dir = opendir (lf->current_path);
        #endif


        // save as system variable
        SDL_mutexP(script_mutex);
        lf->theapp->GetVariables()->add_var("$LOCAL_FILE_PATH", lf->current_path);
        SDL_mutexV(script_mutex);

        SDL_mutexP(listviews_mutex);
        lf->current_position = lf->find_dir_pos();
        lf->clear_list(); //remove anything in list
        //rest the total playlist var
        lf->total_num_files = 0;
        SDL_mutexV(listviews_mutex);


        SDL_mutexP(script_mutex);
        lf->theapp->GetScript()->SetVariableInt("sortingfolders",1);
        SDL_mutexV(script_mutex);

        lf->halt = false;
        bool ndone = false;

        while(!lf->halt || !ndone)
        {
            SDL_mutexP(listviews_mutex);
            ndone = lf->directory_list();
            SDL_mutexV(listviews_mutex);

            if( lf->halt)
                break;

            usleep(20);

        }

        // Now the sort ... qsort is fast but locks the cpu on (Wii)
        int n = lf->total_num_files;
        lfile* list = lf->list_mem;
        lf->halt = false;
        if (n > 1)
        {
            bool done;
            do
            {
                done = false;
                for( int j = 1; j < n; ++j )
                {
                    SDL_mutexP(listviews_mutex);

                    if(compare(&list[j-1],&list[j]) >= 0)
                    {
                        swap(&list[j-1],&list[j]);
                        done = true;
                    }

                    SDL_mutexV(listviews_mutex);

                    usleep( 1 );
                }

                if( lf->halt)
                    break;

                --n;
            }while( done );
        }


        SDL_mutexP(script_mutex);
        lf->theapp->GetScript()->SetVariableInt("sortingfolders",0);
        SDL_mutexV(script_mutex);

        lf->parsing_dir = false;

    }
    return 0;
}

localfiles::localfiles(app_wiiradio* _theapp) :
    list_mem(0),
    total_num_files(0),
    current_position(0),
    hfinder(0),
    dir(0),
    current_playing_dir_count(0),
    parsing_dir(false),
    theapp(_theapp)
{
#ifdef _WII_
    files_start_path =  "sd:/";
#else
#ifdef _WIN32
    files_start_path = "C://";
#endif
#ifdef _LINUX_
    files_start_path = "/";
#endif
#endif
    strcpy(current_path,files_start_path.c_str());
    list_mem = (lfile*)malloc(sizeof(lfile)*MAX_FILE_ITEMS);
    if (!list_mem)
        exit(0);

    listviews_mutex = SDL_CreateMutex();

    bTermThread = 0;
    hfinder = SDL_CreateThread(finder_thread,this);

}

localfiles::~localfiles()
{

    loopi(dirpos.size())
    {
        if(dirpos[i])
        {
            delete dirpos[i];
            dirpos[i] = NULL;
        }
    }

    dirpos.clear();

    bTermThread = 1;
    halt = true;
    if (hfinder)
    {
        SDL_WaitThread(hfinder, NULL); // wait for it to stop
        hfinder = 0;
    }

    SDL_DestroyMutex(listviews_mutex);

    free(list_mem);
    list_mem = NULL;

}

void localfiles::set_playing_dir()
{
    playing_dir = current_path;
    current_playing_dir_count = total_num_files;
}

void localfiles::reset_location(const char* loc)
{
    if(!loc) return;

    sprintf(current_path,"%s",loc);

    files_start_path = loc;
    // update
    theapp->GetVariables()->add_var("$LOCAL_FILE_PATH", current_path);

    directory_list_start(0,0,false);
}


void localfiles::directory_list_start(const char* new_path,const int start, const bool dirup)
{
    if(parsing_dir)
        return;  // already working

    halt = true;


    if (!new_path)
    {
        sprintf(current_path,"%s",current_path);
    }
    else
    {
        if (!dirup)
            save_dir_pos(); // handy for large dirs

        sprintf(current_path,"%s",new_path); // make_path already done if !new path
    }

    search_start = start;
    if (new_path) search_path = new_path;
    else search_path = "";

    parsing_dir = true;

}

string inline localfiles::friendly_name(string s,const bool folder)
{
    const char remove[10] = { '/','\\',':','*','"','\'','?','<','>','|' };
    int len = s.length();

    // remove extention
    if(!folder)
    {
        len -= 4;
        s[len] = '\0';
    }

    // remove un-friendly chars
    loopi(len)
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
        const int len = strlen(filename) - 1;
        if (filename[len]   != '3' ||
            filename[len-1] != 'p' ||
            filename[len-2] != 'm' ||
            filename[len-3] != '.')
            return false;
    }

    string displayname = friendly_name(filename,folder);
    string nameandpath;

    nameandpath = path;
    nameandpath += filename;
    if(folder) nameandpath += "/";

    lfile finfo;
    memset(&finfo,0,sizeof(finfo));

    strcpy(finfo.name,displayname.c_str());
    strcpy(finfo.path,nameandpath.c_str());
    finfo.isfolder = folder;

    SDL_mutexP(listviews_mutex);

    if(list.capacity()-list.size() == 0)
        list.reserve(list.size()+100);

    lfile* lmp = (lfile*)&list_mem[total_num_files];//+ (total_num_files*sizeof(lfile)));

    memcpy((void*)lmp, (void*)&finfo, sizeof(lfile));

    list.push_back(lmp);

    SDL_mutexV(listviews_mutex);

    return true;
}

/*
    Saves the position in local dirs... helpful if you've got a ton of folders
*/
inline void localfiles::save_dir_pos()
{
    loopi(dirpos.size())
    {
        if (current_path == dirpos[i]->dirpath)
        {
            dirpos[i]->pos = current_position;
            return;
        }
    }

    directorypos*   x = new directorypos();
    if(!x)
        return;

    x->dirpath = current_path;
    x->pos = current_position;

    dirpos.push_back(x);
}

/*
    Get the position if saved
*/
inline const int localfiles::find_dir_pos()
{
    loopi(dirpos.size())
    {
        if (current_path == dirpos[i]->dirpath)
            return dirpos[i]->pos;

    }

    return 0;
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
    directory_list_start(new_path.c_str(),0,true);
}

bool localfiles::directory_list()
{
#ifndef _WII_
    struct dirent *ent;
#endif

    struct stat fstat;
    char fullname[FILENAME_MAX*2];
    char filename[FILENAME_MAX];
    int max = 0;

    while (max < 30)
    {
        usleep(10);

        if (total_num_files >= MAX_FILE_ITEMS - 1)
        {
            halt = true;
            return false;
        }

        max++;

#ifdef _WII_

		if(dirnext(dir,filename,&fstat) != 0)
        {
             dirclose (dir);
             dir = 0;
             halt = true;
             return false;
        }

        if((filename[0] == '.') || (strcmp("..", filename) == 0) || (filename[0] == '$'))
            continue; // don't add cd .. cd / we will handle this ourselves, also $ is recycle bin on most devs


        sprintf(fullname,"%s%s",current_path,filename);
#else
        ent = readdir (dir);

        if(!ent)
        {
             closedir (dir);
             dir = 0;
             halt = true;

             return false;
        }

        if((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0) || (ent->d_name[0] == '$'))
            continue; // don't add cd .. cd / we will handle this ourselves


        if (strlen(ent->d_name) > SMALL_MEM)
            continue;

        sprintf(fullname,"%s%s",current_path,ent->d_name);

        stat(fullname,&fstat);

        strcpy(filename,ent->d_name);
#endif


        if(add_file(filename,current_path,(fstat.st_mode & S_IFDIR))) // adds a file to the list
        {
            total_num_files++;
        }

      }


    return true;

}

