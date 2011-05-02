#ifndef _GUI_VISUAL_H_
#define _GUI_VISUAL_H_

class app_wiiradio;
class gui;
#include "gui_object.h"

class gui_visual : public gui_object
{
    public:

    int vis_type;

    unsigned long vis_color_A;
    unsigned long vis_color_B;
    unsigned long vis_bgcolor;

    int changable;

    int peakResults[MAX_FFT_RES];

    gui_visual(app_wiiradio*);
    ~gui_visual();

    virtual int draw();
    virtual int hit_test(const SDL_Event *event);

};

#endif
