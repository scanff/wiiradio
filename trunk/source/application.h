#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "station.h"

class icy;
class shoutcast_browser;
class playlists;
class network;
class gui;
class fft;
class favorites;
class station;
class visualizer;
class local_player;
class localfiles;
enum connect_info;

class app_wiiradio
{
    public:

    app_wiiradio();
    ~app_wiiradio();

    icy*                icy_info;
    shoutcast_browser*  scb;
    playlists*          playlst;
    network*            net;
    gui*                ui;
    fft*                fourier;
    favorites*          favs;
    station             playing; // make now playing as station class so we can access the ip/name quickly
    visualizer*         visuals;
    langs*              lang;
    skins*              sk;
    local_player*       localpb;
    localfiles*         localfs;


    int wii_radio_main(int argc, char **argv);
    void draw_ui(char* info);
    void station_lister(const char* path,char* gen);
    void delete_playlist(int value);
    void connect_to_stream(int value, connect_info info);
    void connect_direct(char* typed);
    void check_keys();
    void search_function(char* value,int search_type);
    void get_favorites();
    void request_save_fav();

    void inline network_playback(char* net_buffer);
    void inline local_playback();

    void stop_playback();
};

#endif // APPLICATION_H_INCLUDED
