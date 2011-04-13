#ifndef GUI_INFO_H_INCLUDED
#define GUI_INFO_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

class gui_info : public gui_dlg
{
    public:

    int             info_text_x;
    int             info_text_y;
    int             info_text_size;
    int             info_cancel_x;
    int             info_cancel_y;

    gui_info(app_wiiradio* _theapp)
    {
        theapp = _theapp;

        skins*  sk = theapp->sk;
        char*   dir = (char*)theapp->ui->skin_path.c_str();

        char s_value1[SMALL_MEM];
        char s_value2[SMALL_MEM];

         // cancel ...
        if (!sk->get_value_file("info_cancel_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("info_cancel_over",s_value2,dir)) exit(0);

        this->cancel = new gui_button(theapp,0,25,NULL,0,false);

        this->cancel->set_images(s_value1,s_value2,0,0);
        this->cancel->center_text = true;
        this->cancel->z_order =  1;
        if (sk->get_value_string("info_cancel_text",s_value1)) this->cancel->set_text(theapp->ui->gui_gettext(s_value1));
        this->cancel->text_color = sk->get_value_color("info_cancel_text_color");
        this->cancel->text_color_over = sk->get_value_color("info_cancel_text_color_over");
        this->cancel->font_sz = sk->get_value_int("info_cancel_text_size");
        this->cancel->pad_y = sk->get_value_int("info_cancel_text_pad_y");

        // --- other stuff
        info_text_x = sk->get_value_int("info_txt_x");
        info_text_y = sk->get_value_int("info_txt_y");
        info_text_size = sk->get_value_int("info_txt_size");
        info_cancel_x = sk->get_value_int("info_cancel_x");
        info_cancel_y = sk->get_value_int("info_cancel_y");
    };

    ~gui_info()
    {
        if (this->cancel)
        {
            delete this->cancel;
            this->cancel = 0;
        }
    };

    int handle_events(SDL_Event* events)
    {

        return 0;
    };


    void draw(void* user_data)
    {
        if (!user_data) return;

        char* txt = (char*)user_data;

        theapp->fnts->set_size(info_text_size);
        theapp->fnts->change_color((theapp->ui->dialog_text_color >> 16), ((theapp->ui->dialog_text_color >> 8) & 0xff),(theapp->ui->dialog_text_color & 0xff));
        fade(theapp->ui->guibuffer,SDL_MapRGB(theapp->ui->guibuffer->format,0,0,0),100);
        SDL_Rect t = {(SCREEN_WIDTH/2)-(theapp->ui->info_dlg->w / 2),(440 / 2) - (theapp->ui->info_dlg->h / 2),theapp->ui->info_dlg->w,theapp->ui->info_dlg->h};
        SDL_BlitSurface(theapp->ui->info_dlg,0, theapp->ui->guibuffer,&t);
        theapp->fnts->text(theapp->ui->guibuffer,txt,t.x + info_text_x,t.y + info_text_y,0);

        if (status == BUFFERING || status == CONNECTING)
        {
            theapp->fnts->set_size(FS_MED);
            cancel->s_x = t.x + info_cancel_x;
            cancel->s_y = t.y + info_cancel_y;
            cancel->draw();
        }
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
