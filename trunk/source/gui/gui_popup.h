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

};

#endif
