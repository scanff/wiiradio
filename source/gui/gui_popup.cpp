#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_popup.h"
#include "../gui.h"

gui_popup::gui_popup(app_wiiradio* _theapp)
    : modal( 1 ), fadebg( 1)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_POPUP;
}

gui_popup::gui_popup(app_wiiradio* _theapp,int x,int y)
    : modal( 1 ), fadebg( 1)
{
    theapp = _theapp;
    s_x = x;
    s_y = y;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_IMG;

}

gui_popup::~gui_popup()
{
}

void gui_popup::load_img(char* name)
{
    gui_object::set_image_img(name);
}


int gui_popup::draw()
{
    visible = false;

    if (theapp->GetSystemStatus() == show_on_status)
    {
         visible = true;
    }else{
        if((show_on_status == theapp->GetWNDStatus()) )//&& theapp->searching)
        {
            visible = true;
        }
    }


    if (!visible) return 0;

    if(fadebg) fade(guibuffer,50);

    const int xoffset = parent ? parent->s_x : 0;
    const int yoffset = parent ? parent->s_y : 0;

    fonts* f = theapp->GetFonts();
    SDL_Surface* d = theapp->ui->guibuffer;


    SDL_Rect ds = {xoffset+s_x,yoffset+s_y,object_images[GUI_IMG_BG]->w,object_images[GUI_IMG_BG]->h};

    if (center_img)
    {
        ds = {(SCREEN_WIDTH/2)-(object_images[GUI_IMG_BG]->w / 2),(440 / 2) - (object_images[GUI_IMG_BG]->h / 2),
                object_images[GUI_IMG_BG]->w,object_images[GUI_IMG_BG]->h};

        // set the new x,y -- could be used by child!
        s_x = ds.x;
        s_y = ds.y;
    }
    f->set_size(gui_object::font_sz);
    f->change_color((text_color >> 16), ((text_color >> 8) & 0xff),(text_color & 0xff));
    SDL_BlitSurface( object_images[GUI_IMG_BG],0, guibuffer,&ds );
    f->text(d,gui_object::get_text(),s_x+pad_x,s_y+pad_y,0);

    return 1;
}
