#include "../globals.h"
#include "../textures.h"
#include "../fonts.h"
#include "../application.h"
#include "gui_object.h"
#include "../script.h"

gui_object::gui_object() :
    obj_type(0),
    s_x(0),
    s_y(0),
    s_h(0),
    s_w(0),
    text_color(0),
    text_color_over(0),
    text_color_playing(0),
    font_sz(0),
    z_order(0),
    center_text(true),
    limit_text(0),
    obj_state(B_OUT),
    obj_sub_state(B_OFF),
    pad_x(0),
    pad_y(0),
    isvariable(false),
    click_func(0),
    over_func(0),
    right_click_func(0),
    object_id(0),
    visible(false),
    center_img(1),
    parent( NULL ),
    show_on_status(0) // default all
{
    bgcolor.color = 0;
    script_onclick = NULL;

    text = new char[SMALL_MEM + 1];
    memset(text,0,SMALL_MEM + 1);

    // NULL all image ptrs
    for(int i=0; i < GUI_IMG_MAX; i++)
        object_images[i] = 0;


    bind_screen = -1;

    memset(show_on_var,0,SMALL_MEM);
    memset(fun_arg,0,SMALL_MEM);
    memset(click_script,0,MED_MEM);
}

gui_object::~gui_object()
{
    delete [] text;

    //object_images free'd in texture cache
}

void gui_object::set_binds(char* instr)
{
    bind_screen = theapp->GetScript()->GetVariableInt((const char*)instr);
}


void gui_object::set_name(char* name, unsigned long name_hash)
{
    if (!name) return;

    this->name = name_hash;
    strcpy(this->text_name,name);

}

void gui_object::set_image_img(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_BG] = tx->texture_lookup(img);

}

void gui_object::set_image_out(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_OUT] = tx->texture_lookup(img);

    /*
        re-calculate the hit area based from the images
        ... if none specified
    */
    if(object_images[GUI_IMG_OUT] && !s_w)
        s_w = object_images[GUI_IMG_OUT]->w;

    if(object_images[GUI_IMG_OUT] && !s_h)
        s_h = object_images[GUI_IMG_OUT]->h;

}

void gui_object::set_image_onout(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_OFF] = tx->texture_lookup(img);
}

void gui_object::set_image_onover(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_ON] = tx->texture_lookup(img);
}

void gui_object::set_image_ondown(char* img)
{
   // texture_cache* tx = theapp->tx;
  //  object_images[GUI_IMG_OFF] = tx->texture_lookup(img);
}

void gui_object::set_show_status(const char* status)
{
    this->show_on_status = theapp->GetScript()->GetVariableInt(status);
}


const char *gui_object::get_var()
{
    return &text[1];
}

const char *gui_object::get_text()
{
    const char* mytext = text;
    const char* varname = &text[1];
    if (*mytext=='$') // is an internal variable
    {
        const char* r = theapp->GetVariables()->search_var(text);
        if (!r) return 0;

        return r;
    }else if (*mytext=='%') // is a LUA variable
    {
        const char* r = theapp->GetScript()->GetVariable(varname);

        if (!r) return 0;

        return r;
    }


    return mytext;
}

void gui_object::set_text(const char* t)
{
    if(!t)
        return;

    const int len = strlen(t);

    if (len >= SMALL_MEM)
        return;

    strcpy(text,t);
}

void gui_object::set_image_down(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_DOWN] = tx->texture_lookup(img);
}

void gui_object::set_image_over(char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_OVER] = tx->texture_lookup(img);

    // also if no DOWN image set yet default to the over
    if (!object_images[GUI_IMG_DOWN]) object_images[GUI_IMG_DOWN] = object_images[GUI_IMG_OVER];
}

void gui_object::set_image_playing(const char* img)
{
    texture_cache* tx = theapp->GetTextures();
    object_images[GUI_IMG_PLAYING] = tx->texture_lookup(img);
}
// -- set the images for this gui object
void gui_object::set_images(const char* out, const char* over, const char* off, const char* on)
{
    texture_cache* tx = theapp->GetTextures();

    switch(obj_type)
    {
        case GUI_TEXTBOX:
        case GUI_BUTTON:

        object_images[GUI_IMG_OUT] = tx->texture_lookup(out);
        object_images[GUI_IMG_OVER] = tx->texture_lookup(over);
        object_images[GUI_IMG_DOWN] = object_images[GUI_IMG_OVER]; // TODO --- click
        if (on) object_images[GUI_IMG_ON]  = tx->texture_lookup(on);
        if (off)
            object_images[GUI_IMG_OFF]  = tx->texture_lookup(off);

        // @ minimum we need these two images for a button
        if(!object_images[GUI_IMG_OUT] || !object_images[GUI_IMG_OVER])
            exit(0);


        //re-calculate the hit area based from the images
        if(object_images[GUI_IMG_OUT])
        {
            s_w = object_images[GUI_IMG_OUT]->w;
            s_h = object_images[GUI_IMG_OUT]->h;
        }

        break;

        case GUI_TOGGLE:

        object_images[GUI_IMG_OFF] = tx->texture_lookup(off);
        object_images[GUI_IMG_ON] = tx->texture_lookup(on);

        // @ minimum we need these two images for a toggle
        if(!object_images[GUI_IMG_OFF] || !object_images[GUI_IMG_ON])
            exit(0);


        //re-calculate the hit area based from the images
        if(object_images[GUI_IMG_OFF]) {
            s_w = object_images[GUI_IMG_OFF]->w;
            s_h = object_images[GUI_IMG_OFF]->h;
        }

        break;

        default: return;

    }
}

bool gui_object::check_screen(const unsigned long cur)
{

    if (obj_type != GUI_SCREEN && !parent)
        return false; // object is not part of a screen

    if (!parent)
    {
        // is the screen
        return (bool)((name == cur));
    }
    bool res = (bool)((parent->name == cur));
    // is a child of a screen, check it's the screen we want to display
    return res;
}

bool gui_object::IsVisible()
{
    if (!parent)
    {
        return visible;

    }else{
        const int ss = theapp->GetSystemStatus();//theapp->GetScreenStatus();

        if((GetType() == GUI_POPUP) &&  (show_on_status != ss))
            return false;

        return parent->visible;
    }

    return false;

}

// -- draw to screen
int gui_object::draw()
{
    if(!IsVisible())
        return 0;


    const int xoffset = parent ? parent->s_x + s_x: s_x;
    const int yoffset = parent ? parent->s_y + s_y: s_y;


    if(object_images[GUI_IMG_OUT] && obj_state == B_OUT){
        if ((obj_sub_state == B_ON) && object_images[GUI_IMG_OFF])
        {
            SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_OFF]->w,object_images[GUI_IMG_OFF]->h};
            SDL_BlitSurface( object_images[GUI_IMG_OFF],0, guibuffer,&ds );
        }else{
            SDL_Rect ds = {xoffset,yoffset, object_images[GUI_IMG_OUT]->w,object_images[GUI_IMG_OUT]->h};
            SDL_BlitSurface( object_images[GUI_IMG_OUT],0, guibuffer,&ds );
        }
        fnts->change_color((text_color >> 16), ((text_color >> 8) & 0xff),(text_color & 0xff));
    }else if (object_images[GUI_IMG_OVER] && obj_state == B_OVER){

        if ((obj_sub_state == B_ON) && object_images[GUI_IMG_ON])
        {
            SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_ON]->w,object_images[GUI_IMG_ON]->h};
            SDL_BlitSurface( object_images[GUI_IMG_ON],0, guibuffer,&ds );
        }else{
            SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_OVER]->w,object_images[GUI_IMG_OVER]->h};
            SDL_BlitSurface( object_images[GUI_IMG_OVER],0, guibuffer,&ds );
        }
        fnts->change_color((text_color_over >> 16), ((text_color_over >> 8) & 0xff),(text_color_over & 0xff));

    }else if (object_images[GUI_IMG_DOWN] && obj_state == B_CLICK){

        SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_DOWN]->w,object_images[GUI_IMG_DOWN]->h};
        SDL_BlitSurface( object_images[GUI_IMG_DOWN],0, guibuffer,&ds );
        fnts->change_color((text_color_over >> 16), ((text_color_over >> 8) & 0xff),(text_color_over & 0xff));

    }else if(object_images[GUI_IMG_OFF] && obj_state == B_OFF){

        SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_OFF]->w,object_images[GUI_IMG_OFF]->h};
        SDL_BlitSurface( object_images[GUI_IMG_OFF],0, guibuffer,&ds );

    }else if (object_images[GUI_IMG_ON] && obj_state == B_ON){

        SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_ON]->w, object_images[GUI_IMG_ON]->h};
        SDL_BlitSurface( object_images[GUI_IMG_ON],0, guibuffer,&ds );

    }

    return 1;

}

bool  gui_object::point_in_rect(const int x, const int y)
{
    const int nx = parent ? parent->s_x + s_x: s_x;
    const int ny = parent ? parent->s_y + s_y: s_y;

    return (bool)(x > nx) && (x < (nx + s_w)) && (y > ny) && (y < (ny + s_h));
}

//-- hit test
int gui_object::hit_test(const SDL_Event *event)
{
    int x;
    int y;

    switch (event->type) {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            x = event->button.x;
            y = event->button.y;
            break;
        default:
            return obj_state;
    }

    if (obj_type == GUI_TOGGLE)
        obj_state = B_OFF;
    else obj_state = B_OUT;

    if (point_in_rect(x,y))
    {
        switch(obj_type)
        {
            case GUI_BUTTON:
            case GUI_TEXTBOX:

                if (event->type == SDL_MOUSEBUTTONDOWN)
                {
                    obj_state = B_CLICK;
                    if (click_func)
                    {
                        click_func(this);
                    }

                    if(*script_onclick)
                        theapp->GetScript()->CallVoidVoidFunction(this->script_onclick);
                }else {
                    obj_state = B_OVER;
                }

                // if button has on off images
                if (object_images[GUI_IMG_ON] && obj_state == B_CLICK)
                    obj_sub_state = (obj_sub_state == B_OFF ? obj_sub_state = B_ON : obj_sub_state = B_OFF);

              break;

            case GUI_TOGGLE:
              if (event->type == SDL_MOUSEBUTTONDOWN)
                  obj_state = (obj_state == B_ON) ? B_OFF : B_ON;
            break;
        }
    }


    return obj_state;
}

// -- set text
/*void gui_object::set_text(const char* t)
{
    if(!t) return;
    else { strcpy(text,t); }
}*/

// -- test the z-order
bool gui_object::test_zorder(const int c)
{
    return (c > z_order ? true : false);
}
