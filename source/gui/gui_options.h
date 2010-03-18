#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

class gui_options : public gui_dlg
{
    public:

    enum _option_buttons {
        O_SCROLL_TEXT = 0,
        O_WIDESCREEN,
        O_MAX
    };


    gui_button*     b_quit;
    gui_button*     b_return;
    gui_button*     b_next_skin;
    gui_button*     b_next_lang;

    SDL_Surface*    logo;
    gui_toggle*     b_option_item[O_MAX];

    gui_group*      saver_group;

    gui_options(gui* g_) : logo(0)
    {

        gui_dlg::fnts = g_->fnts;
        gui_dlg::dest = g_->guibuffer;

        logo = IMG_Load(make_path("imgs/def_logo.png"));

        loopi(O_MAX) b_option_item[i] = 0;
        //quit
        b_quit = new gui_button(gui_dlg::dest,gui_dlg::fnts,270,410,0,0,false);
        b_quit->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_quit->set_text(vars.search_var("$LANG_TXT_QUIT_LOADER"));
        b_quit->pad_y = 5;
        b_quit->text_color = 0x000000;
        b_quit->text_color_over = 0xff0044;
        b_quit->font_sz = FS_SYSTEM;
        b_quit->center_text = true;
        b_quit->bind_screen = S_OPTIONS;

        //return
        b_return = new gui_button(gui_dlg::dest,gui_dlg::fnts,400,410,0,0,false);
        b_return->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_return->set_text(vars.search_var("$LANG_TXT_OK"));
        b_return->pad_y = 5;
        b_return->text_color = 0x000000;
        b_return->text_color_over = 0xff0044;
        b_return->font_sz = FS_SYSTEM;
        b_return->center_text = true;
        b_return->bind_screen = S_OPTIONS;



        b_option_item[O_SCROLL_TEXT] = new gui_toggle(gui_dlg::dest,gui_dlg::fnts,220,150,41,26,0,0);
        b_option_item[O_SCROLL_TEXT]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_SCROLL_TEXT]->bind_screen = S_OPTIONS;

        b_option_item[O_WIDESCREEN] = new gui_toggle(gui_dlg::dest,gui_dlg::fnts,480,150,41,26,0,0);
        b_option_item[O_WIDESCREEN]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_WIDESCREEN]->bind_screen = S_OPTIONS;




        // next skin
        b_next_skin = new gui_button(gui_dlg::dest,gui_dlg::fnts,420,254,0,0,false);
        b_next_skin->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_next_skin->set_text(vars.search_var("$LANG_TXT_NEXT"));
        b_next_skin->pad_y = 5;
        b_next_skin->text_color = 0x000000;
        b_next_skin->text_color_over = 0xff0044;
        b_next_skin->font_sz = FS_SYSTEM;
        b_next_skin->center_text = true;
        b_next_skin->bind_screen = S_OPTIONS;


        b_next_lang = new gui_button(gui_dlg::dest,gui_dlg::fnts,420,314,0,0,false);
        b_next_lang->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_next_lang->set_text(vars.search_var("$LANG_TXT_NEXT"));
        b_next_lang->pad_y = 5;
        b_next_lang->text_color = 0x000000;
        b_next_lang->text_color_over = 0xff0044;
        b_next_lang->font_sz = FS_SYSTEM;
        b_next_lang->center_text = true;
        b_next_lang->bind_screen = S_OPTIONS;


        saver_group = new gui_group(4,220,210,41,26,20,gui_dlg::dest,gui_dlg::fnts);
        saver_group->set_on(g_screensavetime);

        // set options
        g_oscrolltext ? b_option_item[O_SCROLL_TEXT]->obj_state = B_ON : b_option_item[O_SCROLL_TEXT]->obj_state = B_OFF;


    };

    ~gui_options()
    {
        delete saver_group;
        saver_group = 0;

        loopi(O_MAX)
        {
            delete b_option_item[i];
            b_option_item[i] = 0;
        }

        delete b_next_skin;
        delete b_next_lang;
        delete b_quit;
        delete b_return;

        SDL_FreeSurface(logo);
    };

    int handle_events(SDL_Event* events)
    {

        bloopj(MAX_Z_ORDERS)
        {
            if(b_quit->hit_test(events,j)==B_CLICK) g_running = false;
            if(b_return->hit_test(events,j)==B_CLICK) g_screen_status = S_BROWSER;
            if(b_next_skin->hit_test(events,j)==B_CLICK) next_skin();
            if(b_next_lang->hit_test(events,j)==B_CLICK) next_lang();

            loopi(O_MAX) b_option_item[i]->hit_test(events,j);

            g_screensavetime = saver_group->hit_test(events,j);
            b_option_item[O_SCROLL_TEXT]->obj_state == B_OFF ? g_oscrolltext = 0 : g_oscrolltext = 1;


            if (b_option_item[O_WIDESCREEN]->obj_state == B_OFF && g_owidescreen)
            {
                g_owidescreen = 0;
                SetWidescreen();
            }
            else if (b_option_item[O_WIDESCREEN]->obj_state == B_ON && !g_owidescreen)
            {
                g_owidescreen = 1;
                SetWidescreen();
            }

        }

        return 0;
    };

    void output_options()
    {
        int x = 0;
        //logo
        SDL_Rect rc = {50,30,logo->w,logo->h};
        SDL_BlitSurface(logo,0,dest,&rc);

        fnts->change_color(100,100,100);
        fnts->set_size(FS_SYSTEM);

        fnts->text(dest,vars.search_var("$LANG_SCROLL_STATIONTEXT"),200,150,0,1);
        fnts->text(dest,"Widescreen :",470,150,0,1); // -- TO DO Variable this


        fnts->text(dest,"1min       5min     10min     Off",220,180,0,0);
        fnts->text(dest,vars.search_var("$LANG_SCREEN_SAVE"),200,210,0,1);

        // -- skin changer
        fnts->change_color(100,100,100);
        fnts->text(dest,vars.search_var("$LANG_CHANGE_SKIN"),200,260,0,1);
        fnts->change_color(40,40,100);

        fnts->text(dest,vars.search_var("skinname"),220,260,0);
        x = fnts->text(dest,vars.search_var("$LANG_AUTHOR"),220,280,0);
        fnts->text(dest,vars.search_var("skinauthor"),220+x,280,0);

        // -- language selection
        fnts->change_color(100,100,100);
        fnts->text(dest,vars.search_var("$LANG_CHANGE_LANG"),200,320,0,1);
        fnts->change_color(40,40,100);


        fnts->text(dest,vars.search_var("$LANG_NAME"),220,320,0);


        // -- about
        fnts->change_color(100,100,100);
        fnts->text(dest,vars.search_var("$LANG_ABOUT"),200,350,0,1);
        fnts->text(dest,"v0.5. by Scanff, TiMeBoMb and Knarrff",220,350,0);


        // -- draw the butons ect...
        loopi(O_MAX) b_option_item[i]->draw();

        b_quit->draw();
        b_return->draw();
        b_next_lang->draw();
        b_next_skin->draw();
        saver_group->draw();

    };

    void draw()
    {
        boxRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,230,230,230,255); // bg
        rectangleRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,100,100,230,255); // outline
        output_options();
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
