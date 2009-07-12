#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

class gui_options
{
    public:

    enum _option_buttons {
       // O_WIDESCREEN = 0,
        //O_SCROLL_TEXT,
        O_SCROLL_TEXT = 0,
        O_MAX
    };


    SDL_Surface* options_bg;

    gui_button*     b_quit;
    gui_button*     b_return;
    gui_button*     b_restart_net;

    fonts*          fnts;

    SDL_Surface*    dest;
    gui_toggle*     b_option_item[O_MAX];

    gui_group*      saver_group;

    gui_options(fonts* _f, SDL_Surface* _d) : options_bg(0), fnts(_f), dest(_d)
    {
        options_bg = tx->texture_lookup("imgs/options-bg.png");
        if (!options_bg) exit(0);

        loopi(O_MAX) b_option_item[i] = 0;
        //quit
        b_quit = new gui_button(_d,_f,300,355,212,68,0,0,false);
        b_quit->set_images("imgs/exit_over.png","imgs/exit_out.png","imgs/exit_over.png");
        b_quit->bind_screen = S_OPTIONS;

        //return
        b_return = new gui_button(_d,_f,40,355,212,68,0,0,false);
        b_return->set_images("imgs/return_over.png","imgs/return_out.png","imgs/return_over.png");
        b_return->bind_screen = S_OPTIONS;

        //restart network
        b_restart_net = new gui_button(_d,_f,220,240,41,26,0,0,false);
        b_restart_net->set_images("imgs/toggle_out.png","imgs/toggle_out.png","imgs/toggle_out.png");
        b_restart_net->bind_screen = S_OPTIONS;

        int offset_y = 0;
        loopi(O_MAX)
        {
           // b_option_item[i] = new gui_toggle(_d,_f,220,120 + offset_y,41,26,0,0,false);
            b_option_item[i] = new gui_toggle(_d,_f,220,150 + offset_y,41,26,0,0,false);
            b_option_item[i]->set_images("imgs/toggle_out.png","imgs/toggle_on.png");
            b_option_item[i]->bind_screen = S_OPTIONS;

            offset_y += 30;
        }

        saver_group = new gui_group(4,220,210,41,26,20,_d,_f);
        saver_group->set_on(g_screensavetime);


        // set options
        g_oscrolltext ? b_option_item[O_SCROLL_TEXT]->btn_state = B_ON : b_option_item[O_SCROLL_TEXT]->btn_state = B_OFF;
//        g_owidescreen ? b_option_item[O_WIDESCREEN]->btn_state = B_ON : b_option_item[O_WIDESCREEN]->btn_state = B_OFF;



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

        delete b_restart_net;
        delete b_quit;
        delete b_return;
    };

    int handle_events(SDL_Event* events)
    {
        if (events->type != SDL_MOUSEBUTTONDOWN)
        {
            bloopj(MAX_Z_ORDERS) // top ordered first
            {

                if (b_quit->mouse_over(events->motion.x,events->motion.y,j) == B_OVER)
                    return 0; // found it, don't continue

                if (b_return->mouse_over(events->motion.x,events->motion.y,j) == B_OVER)
                    return 0; // found it, don't continue


            }
        }

        if(events->type == SDL_MOUSEBUTTONDOWN)
        {
            bloopj(MAX_Z_ORDERS)
            {
                 if(b_quit->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    g_running = false;
                    return 1;
                 }

                  if(b_return->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    g_screen_status = S_BROWSER;
                    return 1;
                 }

                loopi(O_MAX)
                {
                    b_option_item[i]->hit_test(events->motion.x,events->motion.y,j);
                }

                g_screensavetime = saver_group->hit_test(events->motion.x,events->motion.y,j);

                b_option_item[O_SCROLL_TEXT]->btn_state == B_OFF ? g_oscrolltext = 0 : g_oscrolltext = 1;
//                b_option_item[O_WIDESCREEN]->btn_state == B_OFF ? g_owidescreen = 0 : g_owidescreen = 1;


            }
        }

        return 0;
    };

    void output_options()
    {

        fnts->change_color(60,60,60);

        fnts->set_size(FS_SMALL);
      //  fnts->text(dest,"Widescreen(SOON!):",200,120,0,1);
        fnts->text(dest,"Scroll Station Text:",200,150,0,1);

        fnts->text(dest,"1min       5min     10min     Off",220,180,0,0);
        fnts->text(dest,"Screen Save After:",200,210,0,1);
       // fnts->text(dest,"Restart Network:",200,240,0,1);


        loopi(O_MAX) b_option_item[i]->draw();

        b_quit->draw();
        b_return->draw();
       // b_restart_net->draw();
        saver_group->draw();

        // -- about
        fnts->text(dest,"About:",200,250,0,1);
        fnts->text(dest,"Version 0.4.",220,250,0);
        fnts->text(dest,"By Scanff & TiMeBoMb",220,270,0);

    };

    void draw()
    {
        SDL_BlitSurface(options_bg,0, dest,0);
        output_options();
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
