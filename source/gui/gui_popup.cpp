#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_popup.h"
#include "../gui.h"

gui_popup::gui_popup(app_wiiradio* _theapp)
    : modal( 1 ), fadebg( 1 ), moveable( 0 ), m_down( false )
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_POPUP;
    m_start_y = m_start_x = 0;
}

gui_popup::gui_popup(app_wiiradio* _theapp,int x,int y)
    : modal( 1 ), fadebg( 1 ), moveable( 0 ), m_down( false )
{
    theapp = _theapp;
    s_x = x;
    s_y = y;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_POPUP;
    m_start_y = m_start_x = 0;

}

gui_popup::~gui_popup()
{
}

void gui_popup::load_img(char* name)
{
    gui_object::set_image_img(name);
}


 //-- hit test
int gui_popup::hit_test(const SDL_Event *event)
{
    if(!IsVisible())
        return 0;

    int x;
    int y;

    switch (event->type)
    {
        case SDL_MOUSEMOTION:
        break;
        case SDL_MOUSEBUTTONDOWN:
            m_down = true;
            m_start_x = event->button.x - s_x;
            m_start_y = event->button.y - s_y;
        break;
        case SDL_MOUSEBUTTONUP:
            m_down = false;
        break;
        default:
            return obj_state;
    }

    x = event->button.x;
    y = event->button.y;

    if (point_in_rect(x,y))
    {
        // make this topmost window
        if(m_down) SetTopMost();

        if(m_down && moveable)
        {
            s_x = x - m_start_x;
            s_y = y - m_start_y;
        }
        return 1;
    }

    m_down = false;
    return 0;
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

    if(!IsVisible())
    {
        visible = false;
        return 0;
    }



    if (!visible) return 0;

    if(modal) SetTopMost();
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
