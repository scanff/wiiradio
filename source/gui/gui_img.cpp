#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_img.h"
#include "../gui.h"

gui_img::gui_img(app_wiiradio* _theapp)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_IMG;
}

gui_img::gui_img(app_wiiradio* _theapp,int x,int y)
{
    theapp = _theapp;
    s_x = x;
    s_y = y;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_IMG;

}

gui_img::~gui_img()
{
}

void gui_img::load_img(char* name)
{
    gui_object::set_image_img(name);
}

int gui_img::draw()
{
    if (!(parent ? parent->visible : 1))
        return 0;


    const int xoffset = parent ? parent->s_x : 0;
    const int yoffset = parent ? parent->s_y : 0;

    SDL_Rect ds = {xoffset+s_x,yoffset+s_y,object_images[GUI_IMG_BG]->w,object_images[GUI_IMG_BG]->h};
    SDL_BlitSurface( object_images[GUI_IMG_BG],0, guibuffer,&ds );

    return 0;
}

int gui_img::draw(int x, int y)
{
    SDL_Rect ds = {x,y,object_images[GUI_IMG_BG]->w,object_images[GUI_IMG_BG]->h};
    SDL_BlitSurface( object_images[GUI_IMG_BG],0, guibuffer,&ds );

    return 0;
}
