#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED
#include "../globals.h"
#include "../application.h"
#include "gui_button.h"
#include "gui_group.h"



class gui_options : public gui_object
{
    public:

    enum _option_buttons {
        //O_SCROLL_TEXT = 0,
        O_WIDESCREEN = 0,
        O_RIPMUSIC,
        O_STARTFROMLAST,
        O_MAX,
        // These are not sticky buttons or toggles
        OB_QUIT = 0,
        OB_RETURN,
    //    OB_MORE,
        OB_NEXT_SKIN,
        OB_NEXT_LANG,
        OB_NET_REINIT,
        OB_MAX,

    };

    gui_button*     option_buts[OB_MAX];

    SDL_Surface*    logo;
    gui_button*     b_option_item[O_MAX];

    gui_group*      saver_group;
    gui_group*      service_group;
    gui_group*      sleep_timer_group;
    gui_group*      visuals_group;

    // Used for disk space
    char            freespace_str[50];
    string          media_free_space_desc;
    unsigned long   last_time_ds;


    gui_options(app_wiiradio* _theapp);
    ~gui_options();


    int Events(const SDL_Event* events);
    virtual int draw();

    private:

    void output_options();

};

#endif // GUI_OPTIONS_H_INCLUDED
