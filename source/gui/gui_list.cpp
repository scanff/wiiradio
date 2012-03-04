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
    item_icon_id(0),
    item_w(0),
    item_h(0),
    item_ox(0),
    item_oy(0),
    moveable(0),
    m_down( false ),
    m_start_x( 0 ),
    m_start_y( 0 )
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

    if (parent) z_order += parent->z_order;

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

    // If image use its dims.
    if(this->object_images[GUI_IMG_OUT])
    {
        item_h = this->object_images[GUI_IMG_OUT]->h;
        item_w = this->object_images[GUI_IMG_OUT]->w;
    }

    loopi(number)
    {
        isv = false;
        items[i] = new gui_button(theapp);

        if(item_text[i])
        {
            items[i]->set_text(this->item_text[i]);
            if (*item_text[i] == '$') isv = true;
        }
        items[i]->bgcolor_over.color = item_bgcolor_over;
        items[i]->parent = this;
        items[i]->object_id = i;
        items[i]->ishighlighted = false;
        items[i]->s_x =  item_ox;
        items[i]->s_y = (item_oy) + (item_h * i) + (i*vspacing);
        items[i]->s_h = item_h;
        items[i]->s_w = item_w;
        items[i]->z_order = this->z_order + 1;
        items[i]->text_color = this->text_color;
        items[i]->text_color_over = this->text_color_over;
        items[i]->text_color_playing = this->text_color_playing;
        items[i]->limit_text = this->limit_text;
        items[i]->object_images[GUI_IMG_DOWN] = this->object_images[GUI_IMG_DOWN];
        items[i]->object_images[GUI_IMG_OUT] = this->object_images[GUI_IMG_OUT];
        items[i]->object_images[GUI_IMG_OVER] = this->object_images[GUI_IMG_OVER];
        items[i]->object_images[GUI_IMG_PLAYING] = this->object_images[GUI_IMG_PLAYING];
        items[i]->bgcolor.color = item_bgcolor;
        items[i]->font_sz = this->font_sz;
        items[i]->center_text = this->center_text;
        items[i]->pad_x = this->pad_x;
        items[i]->pad_y = this->pad_y;
        items[i]->isvariable = isv;
        items[i]->bind_screen = this->bind_screen;
        items[i]->click_func = this->click_func;

    }

    // this actually compensates for a bug of not defining a w/h in the skin file

    int m_h = items[number-1]->s_y + items[number-1]->s_h;
    int m_w = items[number-1]->s_w;
    if (s_h < m_h) s_h = m_h;
    if (s_w < m_w) s_w = m_w;

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
    if (!IsVisible() || (listid != theapp->GetListView()))
        return 0;


    visible = true;

    // list has a background
    const int xoffset = parent ? parent->s_x + s_x: s_x;
    const int yoffset = parent ? parent->s_y + s_y: s_y;

    if (bgcolor.color && !object_images[GUI_IMG_BG]) // has a bgcolor
    {
        draw_rect_rgb(guibuffer,xoffset,yoffset,s_w,s_h,bgcolor.cbyte.r,bgcolor.cbyte.g,bgcolor.cbyte.b);

    }else if (object_images[GUI_IMG_BG]){

         SDL_Rect ds = {xoffset,yoffset,object_images[GUI_IMG_BG]->w, object_images[GUI_IMG_BG]->h};
         SDL_BlitSurface( object_images[GUI_IMG_BG],0, guibuffer,&ds );
    }

    loopi(number)
    {
        if (items[i]->visible)
        {
            items[i]->draw();

            if (icons)
            {
                gui_img* img_list = static_cast<gui_imglist*>(icons)->GetImage(item_icon_id[i]);

                if (items[i]->visible)
                {
                    const int xoffset = parent ? parent->s_x + s_x + items[i]->s_x : s_x + items[i]->s_x;
                    const int yoffset = parent ? parent->s_y + s_y + items[i]->s_y : s_y + items[i]->s_y;
                    img_list->draw(xoffset,yoffset);
                }
            }
        }
    }

    return 0;
}

int gui_list::hit_test(const SDL_Event *event)
{
    if (!IsVisible() || (listid != theapp->GetListView()))
        return 0;

    loopi(number) items[i]-> obj_state = B_OUT;

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
        if(m_down && moveable)
        {
            s_x = x - m_start_x;
            s_y = y - m_start_y;
        }

        // test for a change
        int res = 1;
        loopi(number)
        {
            if (!items[i]->visible)
                continue;

            if ((res = items[i]->hit_test(event)))
            {
                m_down = false; // clear the container state
                return res;
            }
        }

        return 1;
    }

    m_down = false; // clear the container state
    return 0;
}

