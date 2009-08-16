#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_

#include "gui_object.h"
//simple button class
class gui_button : public gui_object
{
    public:


    SDL_Surface*    scroll_area;

    int             scroll_x;
    bool            auto_scroll_text;
    int             scroll_speed;
    bool            can_scroll;

    gui_button(SDL_Surface* g,fonts* f,int x,int y,char* t,long tc,bool scroll) :
        scroll_area(0),
        scroll_x(0),
        auto_scroll_text(false),
        scroll_speed(3),
        can_scroll(true)

    {
        s_x = x;
        s_y = y;
        pad_x = 30;
        pad_y = 12;
        text_color = tc;
        fnts = f;
        guibuffer = g;
        obj_state = B_OUT;
        obj_type = GUI_BUTTON;

        if (t) strcpy(text_l1,t);

        // ------

        Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0x00000000;
#endif


        if (scroll)
        {
            scroll_area = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,FONT_SIZE,BITDEPTH, rmask, gmask, bmask,amask);
            SDL_SetColorKey(scroll_area,SDL_SRCCOLORKEY, SDL_MapRGB(scroll_area->format,200,200,200));
        }

    }

    ~gui_button()
    {
        if (scroll_area) SDL_FreeSurface(scroll_area);
    };


    void scroll_reset() { scroll_x = 0; };

    void scroll_text()
    {
        draw_rect(scroll_area,0,0,SCREEN_WIDTH,FONT_SIZE,0xC8C8C8);
        int fontwidth = fnts->text(scroll_area,text_l1,scroll_x,0,0);

        if (obj_state == B_OVER || auto_scroll_text) scroll_x-=scroll_speed; //scroll speed

        SDL_Rect ds = {s_x+pad_x,s_y+pad_y,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_Rect sr = {0,0,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_BlitSurface( scroll_area,&sr , guibuffer,&ds );

        if (scroll_x < -(fontwidth)) scroll_x = limit_text;

    };



    void draw()
    {

        gui_object::draw();

        if (*text_l1)
        {
            fnts->set_size(font_sz);

            if (scroll_area && can_scroll)
            {
                scroll_text();
            }else{
                if (center_text) {
                    int text_len = fnts->get_length_px(text_l1,font_sz);
                    int cx = 0;
                    text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                    fnts->text(guibuffer,text_l1,cx+s_x,s_y+pad_y,limit_text);
                }else{
                    fnts->text(guibuffer,text_l1,s_x+pad_x,s_y+pad_y,limit_text);
                }
            }
        }

    };



};

#endif
