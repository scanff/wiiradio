#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "gui_label.h"
#include "../gui.h"

gui_label::gui_label(app_wiiradio* _theapp)
{
    theapp = _theapp;
    fnts = theapp->GetFonts();
    guibuffer = theapp->ui->guibuffer;
    obj_type = GUI_LABEL;
}

gui_label::~gui_label()
{
}

int gui_label::draw()
{
    if (!(parent ? parent->visible : 1))
        return 0;

    if (!gui_object::draw())
        return 0;

    const char* real_text = gui_object::get_text();
    if(!real_text)
        return 0;

    if (*real_text)
    {
        const int xoffset = parent ? parent->s_x + s_x: s_x;
        const int yoffset = parent ? parent->s_y + s_y: s_y;

        fnts->set_size(font_sz);
        fnts->change_color((text_color >> 16)& 0xff,(text_color >> 8)& 0xff,text_color & 0xff);

        if (center_text) {
            int text_len = fnts->get_length_px((char*)real_text,font_sz);
            int cx = 0;
            text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
            fnts->text(guibuffer,real_text,cx+xoffset,yoffset+pad_y,limit_text);
        }else{
            fnts->text(guibuffer,real_text,xoffset+pad_x,yoffset+pad_y,limit_text);
        }
    }
    return 1;
}
