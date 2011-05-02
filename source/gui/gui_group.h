#ifndef GUI_GROUP_H_INCLUDED
#define GUI_GROUP_H_INCLUDED

#include "gui_object.h"

class gui_button;

class gui_group : public gui_object
{
    public:

    int number;
    int selected;

    gui_button**  items;

    void (*callback)(app_wiiradio*);

    gui_group(app_wiiradio* _theapp, int _num,int _x, int _y, int _w, int _h, int _spacing, void(*cb)(app_wiiradio*));
    ~gui_group();

    virtual int draw();
    virtual int hit_test(const SDL_Event *event);
    void set_on(int num);

};

#endif // GUI_GROUP_H_INCLUDED
