#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_button.h"
#include "../gui.h"
#include "../script.h"


gui_button::gui_button(app_wiiradio* _theapp) :
        scroll_area(0),
        scroll_x(0),
        auto_scroll_text(false),
        scroll_speed(3),
        can_scroll(true),
        ishighlighted(false)

{
    theapp = _theapp;

    s_x = 0;
    s_y = 0;
    pad_x = 30;
    pad_y = 12;
    text_color = 0;
    fnts = theapp->GetFonts();
    guibuffer = theapp->ui->guibuffer;
    obj_state = B_OUT;
    obj_type = GUI_BUTTON;
    visible = 1;

}

gui_button::gui_button(app_wiiradio* _theapp, int x,int y,char* t,long tc,bool scroll) :
    scroll_area(0),
    scroll_x(0),
    auto_scroll_text(false),
    scroll_speed(3),
    can_scroll(true),
    ishighlighted(false)

{
    theapp = _theapp;
    visible = 1;
    s_x = x;
    s_y = y;
    pad_x = 30;
    pad_y = 12;
    text_color = tc;
    fnts = theapp->GetFonts();
    guibuffer = theapp->ui->guibuffer;
    obj_state = B_OUT;
    obj_type = GUI_BUTTON;

    if (t) set_text(t);

    set_scroll(scroll);

}

gui_button::~gui_button()
{
    if (scroll_area) SDL_FreeSurface(scroll_area);
}

void gui_button::set_scroll(bool set)
{
    if(set && !scroll_area)
    {

        scroll_area = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,FONT_SIZE,BITDEPTH, rmask, gmask, bmask,amask);
        SDL_SetColorKey(scroll_area,SDL_SRCCOLORKEY, SDL_MapRGB(scroll_area->format,200,200,200));

    }

}

 //-- hit test
int gui_button::hit_test(const SDL_Event *event)
{
    if(!IsVisible())
        return 0;

    int x;
    int y;

    switch (event->type)
    {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event->button.x;
            y = event->button.y;
            break;
        default:
            return obj_state;
    }

    obj_state = B_OUT;
    if (point_in_rect(x,y))
    {
        if (event->type == SDL_MOUSEBUTTONDOWN)
        {
            obj_state = B_CLICK;
            if (click_func)
            {
                click_func(this);

            }
            if(*click_script)
            {
                theapp->GetScript()->CallVoidVoidFunction(this->click_script);
            }
        }else {
            obj_state = B_OVER;
        }

        // if button has on off images
        if (object_images[GUI_IMG_ON] && obj_state == B_CLICK)
            obj_sub_state = (obj_sub_state == B_OFF ? obj_sub_state = B_ON : obj_sub_state = B_OFF);

    }

    return obj_state;
}


void gui_button::scroll_reset() { scroll_x = 0; };

void gui_button::scroll_text()
{
    SDL_Rect ds = {s_x+pad_x,s_y+pad_y,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
    SDL_Rect sr = {0,0,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};

    draw_rect(scroll_area,0,0,sr.w,FONT_SIZE,0xC8C8C8); // the clear color

    const int fontwidth = fnts->text(scroll_area,gui_object::get_text(),scroll_x,0,0);

    if (obj_state == B_OVER || auto_scroll_text) scroll_x-=scroll_speed; //scroll speed

    SDL_BlitSurface( scroll_area,&sr , guibuffer,&ds );

    if (scroll_x < -(fontwidth)) scroll_x = limit_text;

}

void gui_button::highlight_text()
{
    ishighlighted = true;
}

void inline gui_button::button_is_current_song()
{
    fnts->change_color((text_color_playing >> 16), ((text_color_playing >> 8) & 0xff),(text_color_playing & 0xff));
}

int gui_button::draw()
{

    if (!(parent ? parent->visible : 1))
        return 0;

    gui_object* inherit = parent;
    int xoffset = 0;
    int yoffset = 0;

    while(inherit && (inherit != inherit->parent))
    {
        xoffset += inherit->s_x;
        yoffset += inherit->s_y;
        inherit = inherit->parent;
    }
    xoffset += s_x;
    yoffset += s_y;

    if(ishighlighted && object_images[GUI_IMG_PLAYING])
    {
        SDL_Rect ds = {xoffset,yoffset, object_images[GUI_IMG_PLAYING]->w,object_images[GUI_IMG_PLAYING]->h};
        SDL_BlitSurface( object_images[GUI_IMG_PLAYING],0, guibuffer,&ds );
    }else{
        gui_object::draw();
    }


    const char* real_text = gui_object::get_text();
    if(!real_text)
        return 0;

    if (*real_text)
    {
        fnts->set_size(font_sz);
        if (ishighlighted) button_is_current_song();

        if (scroll_area && can_scroll)
        {
            scroll_text();
        }else{

            if (center_text) {

                const int text_len = fnts->get_length_px((char*)real_text,font_sz);
                int cx = 0;

                text_len > 0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                fnts->text(guibuffer,real_text,cx+xoffset,yoffset+pad_y,limit_text);

            }else{
                fnts->text(guibuffer,real_text,xoffset+pad_x,yoffset+pad_y,limit_text);
            }
        }
    }

    ishighlighted = false;

    return 1;

}

