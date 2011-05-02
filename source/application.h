#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "variables.h"
#include "fonts.h"
#include "options.h"
#include "station.h"
#include "genre_list.h"
#include "audio/sound_sdl.h"

#ifdef _WII_
    enum {
        DEV_SD = 0,
        DEV_USB,
       // DEV_DVD,
        DEV_MAX
    };
#endif

// remap SDL keys so we don't have to loop through 300+ keys that we don't use
enum our_keys
{
    KEY_1 = 0,
    KEY_2,
    KEY_a,
    KEY_b,
    KEY_MINUS,
    KEY_PLUS,
    KEY_DOWN,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ESCAPE,
    KEY_RETURN,
    KEY_F10,
    KEY_MAX
};

// key/button struct
struct _keybutstate
{
    u8 current_keys[KEY_MAX];
    u8 last_keys[KEY_MAX];
    u32 downtime;
};

class icy;
class browser;
class playlists;
class network;
class gui;
class fft;
class favorites;
class station;
class visualizer;
class local_player;
class localfiles;
class langs;
class skins;
class texture_cache;
class projectm;
class script;


// search options
struct _so
{
    char    buf[SMALL_MEM];
    int     search_type;
};

enum connect_info
{
  I_STATION = 0,
  I_PLAYLIST,
  I_DIRECT, // User has typed IP or address
  I_HASBEENSET,
  I_LOCAL
};

static const int remap_keys[KEY_MAX] = {
    SDLK_1,SDLK_2,SDLK_a,SDLK_b,
    SDLK_MINUS,SDLK_PLUS,SDLK_DOWN,SDLK_UP,
    SDLK_LEFT,SDLK_RIGHT,SDLK_ESCAPE,KEY_RETURN,SDLK_F10
};

class app_wiiradio
{
    public:

    app_wiiradio();
    ~app_wiiradio();

    // main loop
    int wrMain(int argc, char **argv);


    icy*                icy_info;

    network*            net;
    gui*                ui;
    fft*                fourier;
    station             playing; // make now playing as station class so we can access the ip/name quickly
    visualizer*         visuals;
    langs*              lang;

    playlists *GetPlaylists() { return playlst; }
    script *GetScript()  { return lua_script; }
    wiiradio_variables *GetVariables()  { return vars; }
    wr_options *GetSettings()  { return settings; }
    audio_device *GetAudio() { return audio_dev; }
    fonts *GetFonts()  { return &fnts; }
    texture_cache *GetTextures() { return tx; }
    skins *GetSkins()  { return sk; }
    projectm *GetPrjM()  { return prjm; }
    local_player *GetLocFile()  { return localpb; }
    genre_list *GetGenreList() { return &gl; }
    localfiles *GetFileDir()  { return localfs; }
    browser *GetBrowser() { return scb; }
    favorites *GetFavorites() { return favs; }

    private:

    favorites*          favs;
    playlists*          playlst;
    localfiles*         localfs;
    local_player*       localpb;
    texture_cache*      tx;
    fonts               fnts;
    skins*              sk;
    projectm*           prjm;
    wr_options*         settings; // saved options/settings
    wiiradio_variables* vars; // variables
    audio_device*       audio_dev;
    script*             lua_script;
    genre_list          gl; // - genre list
    browser*            scb;

    public:

    struct newconnection
    {
        connect_info    info;
        int             value;

    }current_info;


#ifdef _WII_

    const DISC_INTERFACE* sd;
    const DISC_INTERFACE* usb;
    const DISC_INTERFACE* dvd;

    struct _devs
    {
        bool dev_mounted;
        char name[SMALL_MEM];
    }s_devices[DEV_MAX];

    void unmount();
	void prob_media(char* p);
#endif

    SDL_Surface*        screen;
    u64                 last_button_time;
    u64                 sleep_time_start;
    bool                unsaved_volume_change;
    bool                screen_sleeping;

    u8                  audio_data[8192];


    u32                 playing_uuid;
    u32                 playing_item; // item that's currentlt playing
    string              playing_path;

    int                 replay_item; // replay song

    // video flags
    u32                 video_flags;
    _keybutstate        keys_buttons;
    int                 exit_mode; // 0: normal exit, 1: shutdown Wii
    bool                critical_running;


    u64                 app_timer;
    u64                 last_time;
    u64                 buffering_start;
    bool                reloading_skin;
    int                 visualize_number;
    bool                mute;
    u64                 vol_lasttime;


    bool                net_ok; // -- is the network up ... only useful on Wii

#ifndef _WII_ // really not needed for Wii
    void toggle_fullscreen();
#endif
    void switch_sdl_video_mode(const _VideoMode mode);
    void load_splash(const char* text);
    void delete_playlist(const int value);
    void connect_to_stream(const int value, connect_info info);
    void connect_direct(const char* typed);
    inline void check_keys();
    void check_sleep_timer();
    void search_function(const char* value, const int search_type);
    void get_favorites();
    void request_save_fav();
    int new_connection();

    void pause_playback(const bool);
    void seek_playback(const int);

    void screen_timeout();
    void next_lang();
    void next_skin();
    void set_widescreen(const int ws);

    int GetSongPos();

    /* Screen status access */
    void SetScreenStatus(const u32);
    int GetScreenStatus();
    void SetLastScreenStatus();

    // Which list is showing
    int GetListView();
    void SetListView(const u32 s);

    // status .. playing, stopped, buffering etc...
    int GetSystemStatus();
    void SetSystemStatus(const u32 s);
    // general window popups that can be used, these don't effect the states
    const int GetWNDStatus();
    void SetWNDStatus(const u32 s);

    const bool check_media(const char* m);

    private:

    u32     current_wnd;
    u32     last_wnd;
    u32     current_ss;
    u32     last_ss;
    u32     current_lv;
    u32     last_lv;
    u32     current_screen_status;
    u32     last_screen_status;


    public:

    int network_playback(char* net_buffer);
    void local_playback();
    int net_setup();

    private:

    void non_critical();


    FORCEINLINE void run_scrips();
    FORCEINLINE void do_input();
    FORCEINLINE void check_status();
    FORCEINLINE void translate_keys();

    public:
    #ifdef _WII_
    int     cursor_x;
    int     cursor_y;
    float   cursor_angle;
    int     cursor_visible;
    #endif
};

app_wiiradio* GetApp();

#endif // APPLICATION_H_INCLUDED
