#ifndef _FAVORITES_H_
#define _FAVORITES_H_

#include <dirent.h>
#include <vector>
#include "station.h"
class app_wiiradio;

class favorite : public station {
  public:
    string      filename;

    favorite(string name, string url, string filename);
    favorite(string name, string url, string filename, bool local);
};


class favorites {
    public:

    app_wiiradio* theapp;

    #define F_PATH ("data/pls/")

    favorites(app_wiiradio* _theapp);
    ~favorites();

private:
    int  total;
    int  position;
    int  view_max;

public:

    void SetMax(const int m);
    const int GetSize();
    void MoveBack();
    void MoveNext();
    const int GetPosition();


    void delete_favorite(unsigned int item);
    string friendly_name(string s);
    void save_pls(string full_name, station s);
    void save_fav(station s);
    void save_current(station s);
    void clear_list();
    void parse_items_m3u(FILE* f, const char* fname);
    vector<favorite> parse_items_pls(FILE* f, const char* fname);
    int get_fav_type(FILE* f);
    void load_file(const char* fn);
    void load_favorites();

    void* GetItem( const u32 i );
    const char* GetItemText(const u32 i);

    private:

    vector<favorite> list;

};

#endif //_FAVORITES_H_
