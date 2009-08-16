#ifndef GUI_GROUP_H_INCLUDED
#define GUI_GROUP_H_INCLUDED

#include "gui_object.h"
#include "gui_toggle.h"

class gui_group : public gui_object
{
    public:

    int number;

    gui_toggle**  items;

    gui_group(int _num,int _x, int _y, int _w, int _h, int _spacing, SDL_Surface* _g,fonts* _f) :
    number(_num)
    {
        s_x = _x;
        s_y = _y;
        guibuffer = _g;
        fnts = _f;
        obj_type = GUI_GROUP;

        items = new gui_toggle*[_num];

        loopi(_num)
        {
            items[i] = new gui_toggle(_g,_f, s_x + (_w * i) + (i*_spacing) ,s_y ,_w,_h,0,0) ;
            items[i]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
            items[i]->bind_screen = S_OPTIONS;
        }
    };

    ~gui_group()
    {
        loopi(number)
        {
            delete items[i];
            items[i] = 0;
        }

        delete items;
        items = 0;
    };

    void draw()
    {
        loopi(number) items[i]->draw();
    };

    int hit_test(int x, int y, int current_z)
    {
        int h = 0;
        int on = 0;

        // save current on
        loopj(number) if (items[j]->obj_state == B_ON) on = j;

        // turn all off
        loopj(number) items[j]->obj_state = B_OFF;

        // test for a change
        for(h=0;h<number;h++)
        {
            if (items[h]->hit_test(x,y,current_z) == B_ON)
            {
               return h;
            }
        }

        // failed hit test, don't deactivate the one that's on!
        items[on]->obj_state = B_ON;

        return on;
    };


    void set_on(int i)
    {
        if (i >= 0 && i < number) items[i]->obj_state = B_ON;
    };



};

#endif // GUI_GROUP_H_INCLUDED
