#ifndef GUI_INFO_H_INCLUDED
#define GUI_INFO_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

class gui_info : public gui_dlg
{
    public:


    gui_info(app_wiiradio* _theapp)
    {
        theapp = _theapp;
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

        theapp->fnts->set_size(theapp->ui->info_text_size);
        theapp->fnts->change_color((theapp->ui->dialog_text_color >> 16), ((theapp->ui->dialog_text_color >> 8) & 0xff),(theapp->ui->dialog_text_color & 0xff));
        fade(theapp->ui->guibuffer,SDL_MapRGB(theapp->ui->guibuffer->format,0,0,0),100);
        SDL_Rect t = {(SCREEN_WIDTH/2)-(theapp->ui->info_dlg->w / 2),(440 / 2) - (theapp->ui->info_dlg->h / 2),theapp->ui->info_dlg->w,theapp->ui->info_dlg->h};
        SDL_BlitSurface(theapp->ui->info_dlg,0, theapp->ui->guibuffer,&t);
        theapp->fnts->text(theapp->ui->guibuffer,txt,t.x + theapp->ui->info_text_x,t.y + theapp->ui->info_text_y,0);

        if (status == BUFFERING || status == CONNECTING)
        {
            theapp->fnts->set_size(FS_MED);
            cancel->s_x = t.x + theapp->ui->info_cancel_x;
            cancel->s_y = t.y + theapp->ui->info_cancel_y;
            cancel->draw();
        }
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
