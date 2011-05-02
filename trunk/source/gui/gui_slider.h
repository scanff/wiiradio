#ifndef _GUI_SLIDER_H_
#define _GUI_SLIDER_H_

#include "gui_object.h"

class gui_slider : public gui_object
{
    public:



    gui_slider(app_wiiradio* _theapp, int x,int y,char* t,long tc,bool scroll);
    gui_slider(app_wiiradio* _theapp);
    ~gui_slider();

    void set_posimage_over(char* img);
    void set_posimage_out(char* img);

    int get_max() { return slider_max; }
    void set_max(const int m);
    void set_pos(const int p);
    int get_pos() { return slider_pos; }

    virtual int draw();
    virtual int hit_test(const SDL_Event *event);
    //virtual int hit_test(SDL_Event *event, int current_z);

private:

    int slider_max;
    int slider_pos;

};

#endif
