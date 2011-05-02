#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_img_list.h"
#include "gui_img.h"
#include "../gui.h"

gui_imglist::gui_imglist(app_wiiradio* _theapp) :
    frame_time(0),
    time_l(0)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    fnts = theapp->GetFonts();
    obj_type = GUI_LIST;
    callback = 0;
};


gui_imglist::~gui_imglist()
{
    loopi(items.size())
    {
        delete items[i];
    }
}

inline int gui_imglist::new_img(char* path, char* file)
{
    char fullname[MED_MEM];
    gui_img* x = new gui_img(theapp,this->s_x,this->s_y);

    if (!x)
        return 0;

    sprintf(fullname,"%s%s",path,file);

    x->load_img(fullname);
    items.push_back(x);

    return (int)(x);
}

void gui_imglist::parse_imgstr(char* path,char* text)
{
    // parse the input string

 //   int i = 0;
    char new_text[MED_MEM];
    char* tmp_char = new_text;
    char* ptr = text;
    char* vptr_start = text;
    bool is_string = false;
    bool var_done = false;

    memset(new_text,0,MED_MEM);

    while(*ptr)
    {
        var_done = false;

        if (isspace(*ptr) && !is_string)
            continue; // ignore whitespaces, unless there a value like "hello there"

            switch(*ptr)
            {
                case ',':
                    var_done = true; // nothing else on this line matters
                break;

                case '"':
                case '\'':
                continue;
            }

            if(var_done)
            {
                if (!new_img(path,new_text))
                    return;

                memset(new_text,0,MED_MEM);
                vptr_start = ptr++; // skip the ','
                tmp_char = new_text;

            }

            *tmp_char++ = *ptr++;

        }

        // we should capture the last item now
        new_img(path,new_text);

}

void gui_imglist::set_images(char* path, char* image_str)
{
    parse_imgstr(path,image_str);
}

void gui_imglist::create()
{
    loopi(items.size())
    {
        items[i]->parent        = this;
        items[i]->object_id     = i;
        items[i]->s_x           = this->s_x;
        items[i]->s_y           = this->s_y;
        items[i]->s_h           = this->s_h;
        items[i]->s_w           = this->s_w;
        items[i]->isvariable    = 0;


            items[i]->bind_screen   = this->bind_screen;


    }
    this->s_x = this->s_y = 0; // Else will be offset

}

int gui_imglist::draw()
{
    visible = false;

    app_wiiradio* lapp = theapp;

    if (lapp->GetSystemStatus() == show_on_status)
    {
         visible = true;
    }else{
        if((show_on_status == SYS_SEARCHING) && lapp->GetWNDStatus()) //&& theapp->searching)
        {
            visible = true;
        }
    }

    if (!visible) return 0;

    const u32 time_c = lapp->app_timer;

    if (index >= items.size()) index = 0;

    items[index]->draw();

    if ((time_c - time_l) > frame_time)
    {
        index++;
        time_l = time_c;
    }

    return 0;
}

int gui_imglist::hit_test(const SDL_Event *event)
{  // n/a
    return 0;
}

