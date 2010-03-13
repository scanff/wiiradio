#ifndef GUI_INFO_H_INCLUDED
#define GUI_INFO_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

class gui_info : public gui_dlg
{
    public:


    gui_info(gui* _g)
    {
        thegui = _g;
    };

    ~gui_info()
    {
    };

    int handle_events(SDL_Event* events)
    {

        return 0;
    };


    void draw(void* user_data)
    {
        if (!user_data) return;

        char* txt = (char*)user_data;

        thegui->fnts->set_size(thegui->info_text_size);
        thegui->fnts->change_color((thegui->dialog_text_color >> 16), ((thegui->dialog_text_color >> 8) & 0xff),(thegui->dialog_text_color & 0xff));
        fade(thegui->guibuffer,SDL_MapRGB(thegui->guibuffer->format,0,0,0),100);
        SDL_Rect t = {(SCREEN_WIDTH/2)-(thegui->info_dlg->w / 2),(440 / 2) - (thegui->info_dlg->h / 2),thegui->info_dlg->w,thegui->info_dlg->h};
        SDL_BlitSurface(thegui->info_dlg,0, thegui->guibuffer,&t);
        thegui->fnts->text(thegui->guibuffer,txt,t.x + thegui->info_text_x,t.y + thegui->info_text_y,0);

        if (status == BUFFERING || status == CONNECTING)
        {
            thegui->fnts->set_size(FS_MED);
            cancel->s_x = t.x + thegui->info_cancel_x;
            cancel->s_y = t.y + thegui->info_cancel_y;
            cancel->draw();
        }
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
