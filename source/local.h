#ifndef _LOCAL_PLAYER_H_
#define _LOCAL_PLAYER_H_

#include "id3.h"
class app_wiiradio;
class local_player
{
    public:
    int read(u8* buf,int size);
    int open(const char* ifilename);

    app_wiiradio*   theapp;

    id3             m_id3;

    char            track_title[SMALL_MEM]; // track info
    char            last_track_title[SMALL_MEM]; // track info - last


    unsigned long   filepos;
    unsigned long   fileendpos;

    string          display_name;

    FILE*           current_fp;

    unsigned long   song_len;
    char            time_elap_str[TINY_MEM];

    u32             bytes_done;
    bool            done;
  //  u8              buffer[40960];


public:

    local_player(app_wiiradio*);
    ~local_player();

    u32 get_size();
    void seek(const u32 pos);

    private:

    void inline check_filename(const char* ifilename);
int local_open(const char* ifilename);
    public:


    void close();
    void reset();


};

#endif

