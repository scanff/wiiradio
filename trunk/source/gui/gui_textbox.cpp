#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_textbox.h"
#include "../gui.h"

gui_textbox::gui_textbox(app_wiiradio* _theapp)
{
    theapp = _theapp;
    fnts = theapp->GetFonts();
    guibuffer = theapp->ui->guibuffer;
    obj_state = B_OUT;
    obj_type = GUI_TEXTBOX;
}

gui_textbox::gui_textbox(app_wiiradio* _theapp,int x,int y,char* t,long tc,bool scroll) :
    scroll_area(0),
    scroll_x(0),
    auto_scroll_text(false),
    scroll_speed(3),
    can_scroll(true)

{
   theapp = _theapp;

    s_x = x;
    s_y = y;
    pad_x = 30;
    pad_y = 12;
    text_color = tc;
    fnts = theapp->GetFonts();
    guibuffer = theapp->ui->guibuffer;
    obj_state = B_OUT;
    obj_type = GUI_TEXTBOX;

    if (t) set_text(t);


    if (scroll)
    {
        scroll_area = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,FONT_SIZE,BITDEPTH, rmask, gmask, bmask,amask);
        SDL_SetColorKey(scroll_area,SDL_SRCCOLORKEY, SDL_MapRGB(scroll_area->format,200,200,200));
    }

}

gui_textbox::~gui_textbox()
{
    if (scroll_area) SDL_FreeSurface(scroll_area);
}

void gui_textbox::scroll_reset() { scroll_x = 0; };

void gui_textbox::scroll_text()
{
    draw_rect(scroll_area,0,0,SCREEN_WIDTH,FONT_SIZE,0xC8C8C8);
    int fontwidth = fnts->text(scroll_area,gui_object::get_text(),scroll_x,0,0);

    if (obj_state == B_OVER || auto_scroll_text) scroll_x-=scroll_speed; //scroll speed

    SDL_Rect ds = {s_x+pad_x,s_y+pad_y,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
    SDL_Rect sr = {0,0,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
    SDL_BlitSurface( scroll_area,&sr , guibuffer,&ds );

    if (scroll_x < -(fontwidth)) scroll_x = limit_text;

}

int gui_textbox::draw()
{
    if (!gui_object::draw())
        return 0;

    if (*gui_object::get_text())
    {
        fnts->set_size(font_sz);
        fnts->change_color((text_color >> 16)& 0xff,(text_color >> 8)& 0xff,text_color & 0xff);

        if (scroll_area && can_scroll)
        {
            scroll_text();
        }else{
            const char* real_text;

            if(isvariable) real_text = theapp->GetVariables()->search_var(gui_object::get_text());
            else real_text = gui_object::get_text();

            if (center_text) {
                int text_len = fnts->get_length_px(real_text,font_sz);
                int cx = 0;
                text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                fnts->text(guibuffer,real_text,cx+s_x,s_y+pad_y,limit_text);
            }else{
                fnts->text(guibuffer,real_text,s_x+pad_x,s_y+pad_y,limit_text);
            }
        }
    }

    return 1;

}
