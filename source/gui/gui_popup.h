#ifndef _GUI_POPUP_H_
#define _GUI_POPUP_H_

class app_wiiradio;
class gui;
#include "gui_object.h"


class gui_popup : public gui_object
{
    public:

    gui_popup(app_wiiradio*);
    gui_popup(app_wiiradio* _theapp, int x,int y);

    ~gui_popup();


    void set_show_status(const char* status);
    void load_img(char* name);
    virtual int draw();
    virtual const int IsModal() { return modal; }
    virtual int hit_test(const SDL_Event *event);
    public:

    int     modal;
    int     fadebg;
    int     moveable;

    private:

    bool    m_down;
    int     m_start_x;
    int     m_start_y;

};

#endif
