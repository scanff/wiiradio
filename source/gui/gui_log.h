#ifndef GUI_LOG_H_INCLUDED
#define GUI_LOG_H_INCLUDED

#include "gui_button.h"
#include "gui_group.h"

class gui_log : public gui_dlg
{
    public:

    gui_button*     b_return;

    gui_log(gui* g_)
    {

        gui_dlg::fnts = g_->fnts;
        gui_dlg::dest = g_->guibuffer;

        strcpy(text, "");

        //return
        b_return = new gui_button(gui_dlg::dest,gui_dlg::fnts,400,410,0,0,false);
        b_return->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_return->set_text(vars.search_var("$LANG_TXT_OK"));
        b_return->pad_y = 5;
        b_return->text_color = 0x000044;
        b_return->text_color_over = 0xff0000;
        b_return->font_sz = FS_SYSTEM;
        b_return->center_text = true;
        b_return->bind_screen = S_LOG;

    };

    ~gui_log()
    {
        delete b_return;
    };

    int handle_events(SDL_Event* events)
    {
        bloopj(MAX_Z_ORDERS)
        {
            if(b_return->hit_test(events,j)==B_CLICK) SetLastScreenStatus();
        }
        return 0;
    };

    void output_log()
    {
        b_return->draw();

        fnts->change_color(100,100,100);
        fnts->set_size(FS_SYSTEM);

        fnts->text(dest, text, 50, 50, 0);

    };

    void draw()
    {
        boxRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,230,230,230,255); // bg
        rectangleRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,100,100,230,255); // outline
        output_log();
    };



};

#endif // GUI_LOG_H_INCLUDED
