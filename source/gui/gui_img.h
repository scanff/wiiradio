#ifndef _GUI_IMG_H_
#define _GUI_IMG_H_

class app_wiiradio;
class gui;
#include "gui_object.h"

//simple button class
class gui_img : public gui_object
{
    public:

    gui_img(app_wiiradio*);
    gui_img(app_wiiradio* _theapp, int x,int y);

    ~gui_img();

    void load_img(char* name);
    virtual int draw();
    int draw(int x, int y);

};

#endif
