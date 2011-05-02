#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_

#include "gui_object.h"

class gui_button : public gui_object
{
    public:

    SDL_Surface*    scroll_area;

    int             scroll_x;
    bool            auto_scroll_text;
    int             scroll_speed;
    bool            can_scroll;
    bool            ishighlighted;

    gui_button(app_wiiradio* _theapp, int x,int y,char* t,long tc,bool scroll);
    gui_button(app_wiiradio* _theapp);
    ~gui_button();


    void set_scroll(bool set);
    void scroll_reset();
    void scroll_text();
    void highlight_text();
    void inline button_is_current_song();
    virtual int draw();
    virtual int hit_test(const SDL_Event *event);


};

#endif
