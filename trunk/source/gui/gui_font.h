#ifndef _GUI_FONT_H_
#define _GUI_FONT_H_

class app_wiiradio;
class gui;
#include "gui_object.h"

//simple button class
class gui_font : public gui_object
{
    public:

    gui_font(app_wiiradio*);
   ~gui_font();

    void SetSize(const int _s);
    void SetFont(const char* _f);

    private:

    int     size;
    char*   set;

    void load_font();

    virtual int draw();
    int draw(int x, int y);

};

#endif
