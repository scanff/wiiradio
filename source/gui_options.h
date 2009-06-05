#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED

#include "gui_button.h"
#include "gui_toggle.h"

class gui_options
{
    public:

    enum _option_buttons {
        O_WIDESCREEN = 0,
        O_SCROLL_TEXT,
        O_MAX
    };

    int x_pos;
    int o_state;

    SDL_Surface* options_bg;



    gui_button*     b_quit;
    gui_button*     b_return;

    fonts*          fnts;

    SDL_Surface*    dest;
    gui_toggle*     b_option_item[O_MAX];

    gui_options(fonts* _f, SDL_Surface* _d) : x_pos(0), o_state(0), options_bg(0), fnts(_f), dest(_d)
    {
        options_bg = tx->texture_lookup("imgs/options-bg.png");
        if (!options_bg) exit(0);

        x_pos = 0 ;//- options_bg->w;
        loopi(O_MAX) b_option_item[i] = 0;
        //quit
        b_quit = new gui_button(_d,_f,300,355,212,68,0,0,false);
        b_quit->set_images("imgs/exit_over.png","imgs/exit_out.png","imgs/exit_over.png");
        b_quit->bind_screen = S_OPTIONS;

        //return
        b_return = new gui_button(_d,_f,40,355,212,68,0,0,false);
        b_return->set_images("imgs/return_over.png","imgs/return_out.png","imgs/return_over.png");
        b_return->bind_screen = S_OPTIONS;

        int offset_y = 0;
        loopi(O_MAX)
        {
            b_option_item[i] = new gui_toggle(_d,_f,220,150 + offset_y,41,26,0,0,false);
            b_option_item[i]->set_images("imgs/toggle_out.png","imgs/toggle_on.png");
            b_option_item[i]->bind_screen = S_OPTIONS;

            offset_y += 30;
        }

        g_oscrolltext ? b_option_item[O_SCROLL_TEXT]->btn_state = B_ON : b_option_item[O_SCROLL_TEXT]->btn_state = B_OFF;
        g_owidescreen ? b_option_item[O_WIDESCREEN]->btn_state = B_ON : b_option_item[O_WIDESCREEN]->btn_state = B_OFF;

    };

    ~gui_options()
    {
        loopi(O_MAX)
        {
            delete b_option_item[i];
            b_option_item[i] = 0;
        }
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

                b_option_item[O_SCROLL_TEXT]->btn_state == B_OFF ? g_oscrolltext = 0 : g_oscrolltext = 1;
                b_option_item[O_WIDESCREEN]->btn_state == B_OFF ? g_owidescreen = 0 : g_owidescreen = 1;


            }
        }

        return 0;
    };

    void output_options()
    {

        fnts->change_color(60,60,60);

        fnts->set_size(FS_SMALL);
        fnts->text(dest,"Widescreen:",200,150,0,1);
        fnts->text(dest,"Scroll Station Text:",200,180,0,1);
        fnts->text(dest,"Restart Network:",200,210,0,1);
        fnts->text(dest,"Screen Save After:",200,240,0,1);
        fnts->text(dest,"About:",200,270,0,1);

        loopi(O_MAX)
        {
            b_option_item[i]->draw();
        }
        b_quit->draw();
        b_return->draw();

    };

    void draw()
    {

        SDL_Rect t = {x_pos,0,options_bg->w,options_bg->h};
        SDL_BlitSurface(options_bg,0, dest,&t);

    /*    switch(o_state)
        {
            case O_SLIDE: // slid in

                if (x_pos < -20)
                {
                    x_pos += 20;
                }else{
                    o_state ++;
                    x_pos = 0;
                }

            break;

            case O_STATIC:
                output_options();
            break;

*/
            output_options();
 //       }


    };



};

#endif // GUI_OPTIONS_H_INCLUDED
