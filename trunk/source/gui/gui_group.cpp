#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_button.h"
#include "gui_group.h"
#include "../gui.h"

gui_group::gui_group(app_wiiradio* _theapp, int _num,int _x, int _y, int _w, int _h, int _spacing, void(*cb)(app_wiiradio*)) :
    number(_num),selected(0)
{
    theapp = _theapp;
    s_x = _x;
    s_y = _y;
    guibuffer = theapp->ui->guibuffer;
    fnts = theapp->GetFonts();
    obj_type = GUI_GROUP;
    callback = cb;

    items = new gui_button*[_num];

    loopi(_num)
    {
        items[i] = new gui_button(theapp, (_w * i) + (i*_spacing) ,0,0,0,0);// ,_w,_h,0,0) ;
        items[i]->set_images((char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_on.png");
        items[i]->set_image_onout((char*)"data/imgs/toggle_on.png");
        items[i]->bind_screen = this->bind_screen;
        items[i]->parent = (gui_object*)this;
    }
};

gui_group::~gui_group()
{
    loopi(number)
    {
        delete items[i];
        items[i] = 0;
    }

    delete[] items;
    items = 0;
};

int gui_group::draw()
{
    visible = true;
    //gui_object::draw();

    loopi(number)
        items[i]->draw();

    return 1;
};

int gui_group::hit_test(const SDL_Event *event)
{
    // test for a change
    for(int h = 0; h < number; h++)
    {

        if (items[h]->hit_test(event)==B_CLICK)
        {
           if (callback)
               (*callback)(this->theapp);

            items[selected]->obj_sub_state = B_OFF;

            // turn off current
            if (items[h]->obj_sub_state == B_ON)
                selected = h;
        }
    }

    // failed hit test, don't deactivate the one that's on!
    items[selected]->obj_sub_state = B_ON;

    return selected;
}

void gui_group::set_on(int num)
{
    for(int i = 0; i < number; i++)
    {
        items[i]->obj_sub_state = B_OFF;
    }

    items[num]->obj_sub_state = B_ON;
    selected = num;
}
