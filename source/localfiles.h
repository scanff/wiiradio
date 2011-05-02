#ifndef _LOCALFILES_H_
#define _LOCALFILES_H_

#include <dirent.h>
#include <sys/stat.h>
#include <vector>

struct lfile
{
    char name[SMALL_MEM];
    char path[SMALL_MEM];
    bool isfolder;
};

struct directorypos
{
    string  dirpath;
    u32     pos;
};

class app_wiiradio;
class localfiles  {
    public:

    lfile*                  list_mem;
    vector<lfile*>          list;
    vector<directorypos*>   dirpos;

    int                 total_num_files;
    char                current_path[MAX_PATH];

    int                 current_position;
    inline const int    find_dir_pos();
    private:



    string              files_start_path;

    SDL_Thread*         hfinder;

    string              search_path;
    int                 search_start;

    public:

    bool                halt;
#ifdef _WII_
    DIR_ITER            *dir; // faster than standard DIR functions on wii
#else
    DIR                 *dir;
#endif

    u32                 current_playing_dir_count;
    bool                parsing_dir;

    app_wiiradio*       theapp;

    string              playing_dir;

    localfiles(app_wiiradio*);
    ~localfiles();

    void inline clear_list();
    private:

    string inline friendly_name(string s, const bool folder);

    bool inline add_file(const char* filename,const char* path,const bool folder);
    inline void save_dir_pos();


    public:

    void reset_location(const char* loc);
    void set_playing_dir();
    const int GetPosition() { return current_position; }
    void SetPosition(const int p)
    {
        if (p < 0)
        {
            current_position = 0;
        }
        else if (p > total_num_files)
        {
            current_position = total_num_files;
        }
        else
        {
            current_position = p;
        }
    }

    void directory_list_start(const char* new_path, const int start,const bool dirup);
    bool directory_list();
    void set_limit(const int limit);
    //void directory_listnextblock();
    void dirup();



};

#endif
