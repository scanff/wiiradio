#ifndef _GUI_H_
#define _GUI_H_

#include <vector>
#include "application.h"


class gui_options;
class gui_object;
class gui_textbox;
class gui_button;
class gui_dlg;
class gui {

    public:

    app_wiiradio*   theapp;

    fonts*          fnts;
    visualizer*     vis;

    vector<gui_object*>    gui_ents;

    SDL_Surface*    cursor;
    SDL_Surface*    guibuffer;
    SDL_Surface*    visbuffer;
    SDL_Surface*    info_dlg;
    SDL_Surface*    vol_bg;
    SDL_Surface*    vol_ol;
    SDL_Surface*    dialog;
    SDL_Surface*    mute_img;

    enum {
        QB_LOADER = 0,
        QB_SYS,
        QB_BACK,
        QB_TURNOFF,
        QB_MAX
    };

    gui_button*     bQuit[QB_MAX];

    int             vis_on_screen;
    string          skin_path;
    gui_options*    options_screen;

    public:

    gui(app_wiiradio* _theapp);
    ~gui();

    gui_textbox* new_textbox(const char* name);
    gui_button* new_button(const char* name,func_void onclick);

    void screen_changed(const int cur, const int last);


    void handle_events(const SDL_Event* events);
    inline void quit_events(const SDL_Event* events);

    void* new_object(const int type, void* o);
    void draw();
    char *logtext();

    void HideLists(const int); // hides all the list views except arg
    void HideScreens(const int);

    //void inline draw_info(char* txt);
    void draw_cursor(int x,int y, float angle);
    FORCEINLINE void draw_quit();

    int find_object_byname(const char* name);
    gui_object* FindObjByName_Obj(const char* name);

    private:

    void inline draw_visuals();
    void inline draw_browser();
    void inline draw_genres();
    void inline draw_status();
    void draw_local_files();
    void inline draw_favs();
    void inline draw_about();
    void inline draw_log();
    void inline draw_stream_details();
    void inline draw_mp3_details();
    void inline draw_recording();
    void inline draw_volume();

};

#endif //_GUI_H_
