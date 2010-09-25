#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED

#include "../globals.h"
#include "../application.h"
#include "gui_button.h"
#include "gui_toggle.h"
#include "gui_group.h"

static void sleep_timer_callback(app_wiiradio* theapp)
{
    theapp->sleep_time_start = get_tick_count();
}

class gui_options : public gui_dlg
{
    public:

    enum _option_buttons {
        O_SCROLL_TEXT = 0,
        O_WIDESCREEN,
        O_RIPMUSIC,
        O_STARTFROMLAST,
        O_MAX
    };

    gui_button*     b_quit;
    gui_button*     b_return;
    gui_button*     b_next_skin;
    gui_button*     b_next_lang;

    SDL_Surface*    logo;
    gui_toggle*     b_option_item[O_MAX];

    gui_group*      saver_group;
    gui_group*      service_group;
    gui_group*      sleep_timer_group;

    // Used for disk space
    char            freespace_str[50];
    string          media_free_space_desc;
    unsigned long   last_time_ds;


    gui_options(app_wiiradio* _theapp) :
        logo(0),
        media_free_space_desc("unknown"),
        last_time_ds(0)
    {
        theapp = _theapp;
        gui_dlg::fnts = theapp->fnts;
        gui_dlg::dest =  theapp->ui->guibuffer;

        logo = IMG_Load(make_path("imgs/def_logo.png"));

        loopi(O_MAX) b_option_item[i] = 0;
        //quit
        b_quit = new gui_button(theapp, 270,410,0,0,false);
        b_quit->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_quit->set_text(vars.search_var("$LANG_TXT_QUIT_LOADER"));
        b_quit->pad_y = 5;
        b_quit->text_color = 0x000000;
        b_quit->text_color_over = 0xff0044;
        b_quit->font_sz = FS_SYSTEM;
        b_quit->center_text = true;
        b_quit->bind_screen = S_OPTIONS;

        //return
        b_return = new gui_button(theapp,400,410,0,0,false);
        b_return->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_return->set_text(vars.search_var("$LANG_TXT_OK"));
        b_return->pad_y = 5;
        b_return->text_color = 0x000000;
        b_return->text_color_over = 0xff0044;
        b_return->font_sz = FS_SYSTEM;
        b_return->center_text = true;
        b_return->bind_screen = S_OPTIONS;

        int y = 90;

        service_group = new gui_group(theapp,2,220,y,41,26,143,NULL);
        service_group->set_on(g_servicetype);

        y += 35;

        b_option_item[O_SCROLL_TEXT] = new gui_toggle(theapp,220,y,41,26,0,0);
        b_option_item[O_SCROLL_TEXT]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_SCROLL_TEXT]->bind_screen = S_OPTIONS;

        b_option_item[O_WIDESCREEN] = new gui_toggle(theapp,403,y,41,26,0,0);
        b_option_item[O_WIDESCREEN]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_WIDESCREEN]->bind_screen = S_OPTIONS;

        y += 35;

        b_option_item[O_STARTFROMLAST] = new gui_toggle(theapp,220,y,41,26,0,0);
        b_option_item[O_STARTFROMLAST]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_STARTFROMLAST]->bind_screen = S_OPTIONS;

        b_option_item[O_RIPMUSIC] = new gui_toggle(theapp,403,y,41,26,0,0);
        b_option_item[O_RIPMUSIC]->set_images(0,0,(char*)"imgs/toggle_out.png",(char*)"imgs/toggle_on.png");
        b_option_item[O_RIPMUSIC]->bind_screen = S_OPTIONS;

        y += 50;

        saver_group = new gui_group(theapp,4,220,y,41,26,20,NULL);
        saver_group->set_on(g_screensavetime);
        saver_group->bind_screen = S_OPTIONS;

        y += 45;

        sleep_timer_group = new gui_group(theapp,6,220,y,41,26,20,&sleep_timer_callback);
        sleep_timer_group->set_on(g_sleep_timer_time);
        sleep_timer_group->bind_screen = S_OPTIONS;

        y += 35;

        // next skin
        b_next_skin = new gui_button(theapp, 420,y,0,0,false);
        b_next_skin->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_next_skin->set_text(vars.search_var("$LANG_TXT_NEXT_SKIN"));
        b_next_skin->pad_y = 5;
        b_next_skin->text_color = 0x000000;
        b_next_skin->text_color_over = 0xff0044;
        b_next_skin->font_sz = FS_SYSTEM;
        b_next_skin->center_text = true;
        b_next_skin->bind_screen = S_OPTIONS;

        y += 50;

        // next lang
        b_next_lang = new gui_button(theapp,420,y,0,0,false);
        b_next_lang->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        b_next_lang->set_text(vars.search_var("$LANG_TXT_NEXT_LANG"));
        b_next_lang->pad_y = 5;
        b_next_lang->text_color = 0x000000;
        b_next_lang->text_color_over = 0xff0044;
        b_next_lang->font_sz = FS_SYSTEM;
        b_next_lang->center_text = true;
        b_next_lang->bind_screen = S_OPTIONS;


        // set options
        g_oscrolltext ? b_option_item[O_SCROLL_TEXT]->obj_state = B_ON : b_option_item[O_SCROLL_TEXT]->obj_state = B_OFF;
        g_owidescreen ? b_option_item[O_WIDESCREEN]->obj_state = B_ON : b_option_item[O_WIDESCREEN]->obj_state = B_OFF;
        g_oripmusic ? b_option_item[O_RIPMUSIC]->obj_state = B_ON : b_option_item[O_RIPMUSIC]->obj_state = B_OFF;
        g_startfromlast ? b_option_item[O_STARTFROMLAST]->obj_state = B_ON : b_option_item[O_STARTFROMLAST]->obj_state = B_OFF;

        // Try calc. ds on construction
        media_free_space_desc = get_media_free_space_desc();
    };

    ~gui_options()
    {
        delete service_group;
        service_group = 0;

        delete saver_group;
        saver_group = 0;

        delete sleep_timer_group;
        sleep_timer_group = 0;

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
            if(b_return->hit_test(events,j)==B_CLICK) SetLastScreenStatus();
            if(b_next_skin->hit_test(events,j)==B_CLICK) theapp->next_skin();
            if(b_next_lang->hit_test(events,j)==B_CLICK) theapp->next_lang();

            loopi(O_MAX) b_option_item[i]->hit_test(events,j);

            g_screensavetime = saver_group->hit_test(events,j);
            g_sleep_timer_time = sleep_timer_group->hit_test(events,j);
            g_servicetype = service_group->hit_test(events,j);

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

            // rip music
            b_option_item[O_RIPMUSIC]->obj_state == B_OFF ? g_oripmusic = 0 : g_oripmusic = 1;
            // start from last
            b_option_item[O_STARTFROMLAST]->obj_state == B_OFF ? g_startfromlast = 0 : g_startfromlast = 1;

        }

        return 0;
    };

    void output_options()
    {
        fonts* fnts = theapp->fnts;

        int x = 0;
        ostringstream str;

        //logo
        SDL_Rect rc = {50,30,logo->w,logo->h};
        SDL_BlitSurface(logo,0,dest,&rc);

        // Font setup
        fnts->change_color(100,100,100);
        fnts->set_size(FS_SYSTEM);

        // -- about
        str << vars.search_var("$LANG_ABOUT")
            << " v" << VERSION_NUMBER << " - " << vars.search_var("$LANG_AUTHOR")
            << " Scanff, TiMeBoMb " << vars.search_var("$LANG_AND") << " Knarrff";
        fnts->text(dest, str.str().c_str(), 200, 50, 0, 0);

        int y = 90;

        // for service type
        fnts->text(dest,"SHOUTcast :",200,y,0,1);
        fnts->text(dest,"Icecast :",393,y,0,1);

        y += 35;
        // Scrolltext, Widescreen, Rip Music
        fnts->text(dest,vars.search_var("$LANG_SCROLL_STATIONTEXT"),200,y,0,1);
        fnts->text(dest,"Widescreen :",393,y,0,1); // -- TO DO Variable this

        y += 35;
        fnts->text(dest,"Play last at start :", 200,y,0,1); // -- TO DO Variable this
        fnts->text(dest,"Rip Music :",393,y,0,1); // -- TO DO Variable this

        /*
            Show the user how much free space they have left.
            Only run this every few seconds as it my lag
        */
        if((SDL_GetTicks() - last_time_ds > 1000*10) && g_oripmusic)
        {
            last_time_ds = SDL_GetTicks();
            media_free_space_desc = get_media_free_space_desc();
        }

        fnts->text(dest,("(Free space: "+media_free_space_desc+")").c_str(),450,y,0,0);

        y += 50;
        // screen save
        fnts->text(dest,vars.search_var("$LANG_SCREEN_SAVE"),200,y,0,1);
        fnts->text(dest,"  Off         1min       5min      10min",220,y-20,0,0);

        y += 45;
        // sleep timer
        fnts->text(dest,vars.search_var("$LANG_SLEEP_TIMER"),200,y,0,1);
        fnts->text(dest,"  Off         5min      15min     30min        1h           3h",220,y-20,0,0);

        y += 35;
        // -- skin changer
        fnts->change_color(100,100,100);
        fnts->text(dest,vars.search_var("$LANG_CHANGE_SKIN"),200,y,0,1);
        fnts->change_color(40,40,100);

        fnts->text(dest,vars.search_var("skinname"),220,y,0);
        x = fnts->text(dest,vars.search_var("$LANG_AUTHOR"),220,y+20,0);
        x += fnts->text(dest,": ",220+x,y+20,0);
        fnts->text(dest,vars.search_var("skinauthor"),220+x,y+20,0);

        y += 50;
        // -- language selection
        fnts->change_color(100,100,100);
        fnts->text(dest,vars.search_var("$LANG_CHANGE_LANG"),200,y,0,1);
        fnts->change_color(40,40,100);
        fnts->text(dest,vars.search_var("$LANG_NAME"),220,y,0);

        // -- draw the butons ect...
        loopi(O_MAX) b_option_item[i]->draw();

        b_quit->draw();
        b_return->draw();
        b_next_lang->draw();
        b_next_skin->draw();
        saver_group->draw();
        sleep_timer_group->draw();
        service_group->draw();
    };

    void draw()
    {
        boxRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,230,230,230,255); // bg
        rectangleRGBA(dest,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,100,100,230,255); // outline
        output_options();
    };



};

#endif // GUI_OPTIONS_H_INCLUDED
