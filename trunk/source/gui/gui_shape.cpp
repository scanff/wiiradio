#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_shape.h"
#include "../gui.h"

gui_shape::gui_shape(app_wiiradio* _theapp)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_SHAPE;
}

gui_shape::gui_shape(app_wiiradio* _theapp,int x,int y)
{
    theapp = _theapp;
    s_x = x;
    s_y = y;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_SHAPE;

}

gui_shape::~gui_shape()
{
}


int gui_shape::draw()
{
    if (!(parent ? parent->visible : 1))
        return 0;

    const int xoffset = parent ? parent->s_x : 0;
    const int yoffset = parent ? parent->s_y : 0;

    draw_rect_rgb(guibuffer,xoffset+s_x,yoffset+s_y,s_w,s_h,bgcolor.cbyte.r,bgcolor.cbyte.g,bgcolor.cbyte.b);

    return 1;
}
