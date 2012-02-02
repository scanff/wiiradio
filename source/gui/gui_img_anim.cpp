#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_img_anim.h"
#include "gui_img.h"
#include "../gui.h"
#include "../script.h"

gui_imganim::gui_imganim(app_wiiradio* _theapp) :
    frame_time(0),
    time_l(0)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    fnts = theapp->GetFonts();
    obj_type = GUI_LIST;
    callback = 0;
};


gui_imganim::~gui_imganim()
{
    loopi(items.size())
    {
        delete items[i];
    }
}

inline int gui_imganim::new_img(char* path, char* file)
{
    char fullname[MED_MEM];
    gui_img* x = new gui_img(theapp,this->s_x,this->s_y);

    if (!x)
        return 0;

    sprintf(fullname,"%s%s",path,file);

    x->load_img(fullname);
    items.push_back(x);

    return !!x;
}

void gui_imganim::parse_imgstr(char* path,char* text)
{
    // parse the input string

    char new_text[MED_MEM];
    char* tmp_char = new_text;
    char* ptr = text;
    char* vptr_start = text;
    bool is_string = false;
    bool var_done;

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

void gui_imganim::set_images(char* path, char* image_str)
{
    parse_imgstr(path,image_str);
}

void gui_imganim::create()
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
        items[i]->bind_screen  = this->bind_screen;


    }
    this->s_x = this->s_y = 0; // Else will be offset

}
int gui_imganim::draw()
{

    visible = false;
// UGH ... need to improve this !!!
    if (theapp->GetSystemStatus() == show_on_status)
    {
         visible = true;
    }else{
        if((show_on_status == SYS_SEARCHING) && theapp->GetWNDStatus()) //&& theapp->searching)
        {
            visible = true;
        }
        else if(show_on_var[0])
        {
          //  visible = (bool)theapp->GetSettings()->GetOptionInt(show_on_var);
              SDL_mutexP(script_mutex);
              visible = (bool)theapp->GetScript()->GetVariableInt(show_on_var);
              SDL_mutexV(script_mutex);
        }
    }


    if (!visible) return 0;


    const int tx = s_x;
    const int ty = s_y;

    if (parent)
    {
        s_x += parent->s_x;
        s_y += parent->s_y;
    }

    if (!gui_object::draw())
    {

    s_x = tx;
    s_y = ty;
        return 0;
    }

    unsigned long time_c = SDL_GetTicks();

    if (index >= items.size()) index = 0;

    items[index]->draw();

    if ((time_c - time_l) > frame_time)
    {
        index++;
        time_l = time_c;
    }


    s_x = tx;
    s_y = ty;

    return 0;
}

int gui_imganim::hit_test(SDL_Event *event, int current_z)
{  // n/a
    return 0;
}

