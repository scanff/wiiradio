#include "../globals.h"
#include "../application.h"
#include "../textures.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_slider.h"
#include "../gui.h"
#include "../script.h"


gui_slider::gui_slider(app_wiiradio* _theapp)
    : slider_max(100), slider_pos(0)
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
    obj_type = GUI_SLIDER;

}

gui_slider::gui_slider(app_wiiradio* _theapp, int x,int y,char* t,long tc,bool scroll)
    : slider_max(100), slider_pos(0)
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
    obj_type = GUI_SLIDER;

    if (t) set_text(t);


}

gui_slider::~gui_slider()
{

}

void gui_slider::set_posimage_over(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_SLIDERPOS_OVER] = tx->texture_lookup(img);
}
void gui_slider::set_posimage_out(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_SLIDERPOS_OUT] = tx->texture_lookup(img);
}

//--Max
void gui_slider::set_max(const int m)
{
    slider_max = m;
}

void gui_slider::set_pos(const int p)
{
    slider_pos = p;
}
 //-- hit test
int gui_slider::hit_test(const SDL_Event *event)
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
            // calc new pos
            const int nx = parent ? parent->s_x + s_x: s_x;
           // const int ny = parent ? parent->s_y + s_y: s_y;


            const float fw = (float)((float)slider_max / ((float)object_images[GUI_IMG_SLIDERPOS_OUT]->w));
            slider_pos = (x - nx) * fw;
            // --
            obj_state = B_CLICK;
            if (click_func)
            {
                click_func(this);
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


int gui_slider::draw()
{

    if (!(parent ? parent->visible : 1))
        return 0;


    gui_object::draw();

    set_pos(theapp->GetSongPos());

    const int xoffset = parent ? parent->s_x + s_x: s_x;
    const int yoffset = parent ? parent->s_y + s_y: s_y;

    if (*gui_object::get_text())
    {

        fnts->set_size(font_sz);


        const char* real_text = gui_object::get_text();

        if (center_text) {

            const int text_len = fnts->get_length_px((char*)real_text,font_sz);
            int cx = 0;

            text_len > 0 ? cx = (int)((s_w-(text_len))/2): cx=1;
            fnts->text(guibuffer,real_text,cx+xoffset,yoffset+pad_y,limit_text);

        }else{
            fnts->text(guibuffer,real_text,xoffset+pad_x,yoffset+pad_y,limit_text);
        }

    }

    // draw the pos
//    : slider_max(100), slider_pos(50)
        // actual volume bar

    if (obj_state != B_OUT)
    {
        const float w = (float)(((float)object_images[GUI_IMG_SLIDERPOS_OUT]->w / (float)slider_max) * (float)slider_pos);
        SDL_Rect sr = { 0,0, (int)w,object_images[GUI_IMG_SLIDERPOS_OUT]->h };
        SDL_Rect ds = {xoffset,yoffset,(int)w/*object_images[GUI_IMG_SLIDERPOS_OUT]->w*/,object_images[GUI_IMG_SLIDERPOS_OUT]->h};
        SDL_BlitSurface( object_images[GUI_IMG_SLIDERPOS_OUT],&sr, guibuffer,&ds );
    }else{
        const float w = (float)(((float)object_images[GUI_IMG_SLIDERPOS_OVER]->w / (float)slider_max) * (float)slider_pos);
        SDL_Rect sr = { 0,0, (int)w,object_images[GUI_IMG_SLIDERPOS_OVER]->h };
        SDL_Rect ds = {xoffset,yoffset,(int)w/*object_images[GUI_IMG_SLIDERPOS_OUT]->w*/,object_images[GUI_IMG_SLIDERPOS_OVER]->h};
        SDL_BlitSurface( object_images[GUI_IMG_SLIDERPOS_OVER],&sr, guibuffer,&ds );

    }

    return 1;

}

