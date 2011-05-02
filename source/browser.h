#ifndef _BROWSER__H__
#define _BROWSER__H__

#include "station_cache.h"

struct station_list {
    station_list() :   codec_type(0),
                        service_type(0) {}
    char station_name[MED_MEM]; // name
    char station_playing[MED_MEM]; // playing
    char station_id[MED_MEM]; // url
    int   codec_type;
    int   service_type; // shoutcast, icecast, etc...
};

class browser
{
private:

    /*
        .:: type ::.

        0 SHOUTCAST
        1 ICECAST

    */

   /*
    #define SHOUTCAST_DB_URL    ("www.shoutcast.com")
    *** shoutcast is no longer free .. need an DEV APIKEY
    */
    #define NAVIX_DB_URL        ("www.navi-x.org") // NICE !!!
    #define ICECAST_DB_URL      ("dir.xiph.org")
    #define MAX_PAGE_SIZE       (500000)

    vector<station_list*>   sl;
    network*                net;
    int                     type;

    int  total;
    int  position;
    int  view_max;

public:

    void SetMax(const int m);
    const int GetSize();
    void MoveBack();
    void MoveNext();
    const int GetPosition();

    app_wiiradio*   theapp;

    browser(app_wiiradio* _theapp);
    ~browser();

    const char* get_station(int num);
    station_list* GetItem(const u32 n);

    const int connect(const char* path,char* genre,int itype);

private:

    station_list* new_sl_list();
    void clear_list();
    inline void strip_quotes(char* in);
    char* get_attrib_navix(char* data,const char* attrib,char* attib_buffer,int max);
    char* get_element(char* data,const char* element,char* element_buffer,int max);
    void get_stationid(const char* url,char* stationid);
    void parse_page(char* data,char* genre_filter,int page_size);

    void string_lower(char* string);
    // --- end
};

#endif


