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
class fonts;
class langs;
class skins;
class texture_cache;
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
    local_player*       localpb;
    localfiles*         localfs;
    texture_cache*      tx;
    fonts*              fnts; //extern this to use everywhere
    skins*              sk;

    SDL_Surface*        screen;
    Uint64              last_button_time;
    Uint64              sleep_time_start;
    bool                unsaved_volume_change;
    bool                screen_sleeping;

    unsigned char       audio_data[8192];

    int                 playing_item; // item that's currentlt playing

    // -- threads
    SDL_Thread*         connectthread;
    SDL_mutex*          connect_mutex;

    int wii_radio_main(int argc, char **argv);
    void draw_ui(char* info);
    void station_lister(const char* path,char* gen);
    void delete_playlist(int value);
    void connect_to_stream(int value, connect_info info);
    void connect_direct(char* typed);
    void check_keys();
    void check_sleep_timer();
    void search_function(char* value,int search_type);
    void get_favorites();
    void request_save_fav();

    void inline network_playback(char* net_buffer);
    void inline local_playback();

    void stop_playback();
    void screen_timeout();
    void next_lang();
    void next_skin();
};

#endif // APPLICATION_H_INCLUDED
