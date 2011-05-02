#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_list.h"
#include "gui_img.h"
#include "gui_img_list.h"
#include "gui_button.h"
#include "../gui.h"

gui_list::gui_list(app_wiiradio* _theapp) :
    number(0),
    selected(0),
    icons(0),
    item_icon_id(0)
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    fnts = theapp->GetFonts();
    obj_type = GUI_LIST;
    callback = 0;
};


gui_list::~gui_list()
{
    if(item_icon_id)
    {
        delete item_icon_id;
        item_icon_id = 0;
    }

    loopi(number)
    {

        delete items[i];
        items[i] = 0;

        delete item_text[i];
        item_text[i] = 0;
    }

    delete[] items;
    items = 0;

    delete[] item_text;
    item_text = 0;
}



// override the set text as it can be an array
//void gui_list::set_text(char* text)
//{
//}
void gui_list::parse_text(const char* text)
{
    // parse the input string

    int i = 0;
    char new_text[MED_MEM];
    char* tmp_char = new_text;
    char* ptr = (char*)text;
    char* vptr_start = (char*)text;
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
                item_text[i++] = strdup(new_text);

                memset(new_text,0,MED_MEM);
                vptr_start = ptr++; // skip the ','
                tmp_char = new_text;

                if (i >= number)
                    return;
            }

            *tmp_char++ = *ptr++;

        }

        // we should capture the last item now
        item_text[i] = strdup(new_text);

}
// create the list if not done so in construct
void gui_list::create()
{
    bool isv = false;

    items = new gui_button*[number];
    item_text = new char*[number];

    if(icons)
    {
        item_icon_id = new int[number];
        memset(item_icon_id,0,number*sizeof(int));
    }

    loopi(number)
    {
        item_text[i] = 0;
    }

    parse_text(gui_object::get_text_raw());

    loopi(number)
    {
        isv = false;
        items[i] = new gui_button(theapp);

        if(item_text[i])
        {
            items[i]->set_text(this->item_text[i]);
            if (*item_text[i] == '$') isv = true;
        }

        items[i]->parent = this->parent;
        items[i]->object_id = i;
        items[i]->ishighlighted = false;
        items[i]->s_x = this->s_x;
        items[i]->s_y = s_y + (s_h * i) + (i*vspacing);
        items[i]->s_h = this->s_h;
        items[i]->s_w = this->s_w;
        items[i]->text_color = this->text_color;
        items[i]->text_color_over = this->text_color_over;
        items[i]->text_color_playing = this->text_color_playing;
        items[i]->limit_text = this->limit_text;
        items[i]->object_images[GUI_IMG_DOWN] = this->object_images[GUI_IMG_DOWN];
        items[i]->object_images[GUI_IMG_OUT] = this->object_images[GUI_IMG_OUT];
        items[i]->object_images[GUI_IMG_OVER] = this->object_images[GUI_IMG_OVER];
        items[i]->object_images[GUI_IMG_PLAYING] = this->object_images[GUI_IMG_PLAYING];
        items[i]->font_sz = this->font_sz;
        items[i]->center_text = this->center_text;
        items[i]->pad_x = this->pad_x;
        items[i]->pad_y = this->pad_y;
        items[i]->isvariable = isv;
        items[i]->bind_screen = this->bind_screen;
        items[i]->click_func = this->click_func;

    }

}

void gui_list::SetItemVisible(const int i, const bool v)
{
    items[i]->visible = v;
}

void gui_list::SetIconID(const int i, const int id)
{
    item_icon_id[i] = id;
}

void gui_list::SetHighlight(const int i, const bool h)
{
    items[i]->ishighlighted = h;
}

int gui_list::draw()
{
    if (((!this->visible) && (listid != theapp->GetListView())) || (theapp->GetScreenStatus() != this->parent->bind_screen))
        return 0;


    visible = true;

    loopi(number)
    {
        if (items[i]->visible)
        {
            items[i]->draw();

            if (icons)
            {
                gui_img* img_list = static_cast<gui_imglist*>(icons)->GetImage(item_icon_id[i]);

                if (items[i]->visible)
                    img_list->draw(items[i]->s_x,items[i]->s_y);
            }
        }
    }

    return 0;
}

int gui_list::hit_test(const SDL_Event *event)
{
    if ((!this->parent || !this->parent->visible || !this->visible) && (listid != theapp->GetListView()))
        return 0;

    loopi(number) items[i]-> obj_state = B_OUT;

    // test for a change
    int res = 1;
    loopi(number)
    {
        if (!items[i]->visible)
            continue;

        if ((res = items[i]->hit_test(event)))
        {
            return res;
        }
    }

    return 0;
}

