#ifndef _GUI_SHAPE_H_
#define _GUI_SHAPE_H_

class app_wiiradio;
class gui;
#include "gui_object.h"

class gui_shape : public gui_object
{
    public:

    gui_shape(app_wiiradio*);
    gui_shape(app_wiiradio* _theapp, int x,int y);

    ~gui_shape();

    virtual int draw();

};

#endif
