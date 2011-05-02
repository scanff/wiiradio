#include "../globals.h"
#include "../textures.h"
#include "../fonts.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_screen.h"
#include "../gui.h"

gui_screen::gui_screen(app_wiiradio* _theapp)
{
    theapp = _theapp;
    obj_type = GUI_SCREEN;
    guibuffer = theapp->ui->guibuffer;
}

gui_screen::~gui_screen()
{
}

// -- set the images for this gui object

void gui_screen::set_image_img(char* theimg)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_BG] = tx->texture_lookup(theimg);

    //re-calculate the hit area based from the images
    if(object_images[GUI_IMG_BG]) {
        s_w = object_images[GUI_IMG_BG]->w;
        s_h = object_images[GUI_IMG_BG]->h;
    }

}

// -- draw to screen
int gui_screen::draw()
{
    if(!visible) return 0;

    if (!gui_object::draw())
    {
        visible = false;
        return 0;
    }

    if (bgcolor.color && !object_images[GUI_IMG_BG]) // has a bgcolor
    {
        draw_rect_rgb(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,bgcolor.cbyte.r,bgcolor.cbyte.g,bgcolor.cbyte.b);

    }else if (object_images[GUI_IMG_BG]){

         SDL_Rect ds = {s_x,s_y,object_images[GUI_IMG_BG]->w, object_images[GUI_IMG_BG]->h};
         SDL_BlitSurface( object_images[GUI_IMG_BG],0, guibuffer,&ds );
    }

    return 1;

}

bool inline gui_screen::point_in_rect(const int x, const int y)
{
    return (bool)(x > s_x) && (x < (s_x + s_w)) && (y > s_y) && (y < (s_y + s_h));
}

//-- hit test
int gui_screen::hit_test(SDL_Event *event, int current_z)
{
    return 0;
}

// -- test the z-order
bool gui_screen::test_zorder(const int c)
{
    return (c > z_order ? true : false);
}
