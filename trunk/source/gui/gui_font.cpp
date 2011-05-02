#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_font.h"
#include "../gui.h"

gui_font::gui_font(app_wiiradio* _theapp) :
    set( NULL )
{
    theapp = _theapp;
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_FONT;
}

gui_font::~gui_font()
{
    if (set)
    {
        delete [] set;
        set = 0;
    }
}

void gui_font::SetSize(const int _s)
{
    size = _s;
}

void gui_font::SetFont(const char* _f)
{
    if (!_f) return;
    int len = strlen(_f);

    set = new char[len+1];
    strcpy(set,_f);
    set[len] = '\0';
}

int gui_font::draw()
{
    return 0;
}
