#ifndef _GUI_H_
#define _GUI_H_
#include "../application.h"
#include "../genre_list.h"

class gui {

    typedef void(*func_gui_void)(void*);

    // -- everyone knows who gui is :)
    #include "gui_dlg.h"
    #include "gui_button.h"
    #include "gui_textbox.h"
    #include "gui_options.h"
    #include "gui_search.h"
    #include "gui_log.h"
    #include "gui_info.h"
   // #include "gui_connect.h"

    public:

    // -- Buttons
    enum {
        BTN_NEXT = 0,
        BTN_PRIOR,
        BTN_PLAYLISTS,
        BTN_GENRES_SELECT,
        BTN_PLAYING,
        BTN_LOGO,
//        BTN_CANCEL, // cancel buffering or connecting !
        // -- OPTIONAL BUTTONS
        BTN_ADD, // -- optional button
        BTN_STOP, // -- optional stop button
        BTN_EXIT, // -- optional exit button on skin button
        BTN_BROWSER, // -- optional browser ... displays the browser items
        BTN_VISUALS, // -- optional Show Visuals ... Shows the visuals screen
        BTN_SEARCH, // -- optional Show search screen ... Shows the search screen
        BTN_NX_SKIN, // -- optional next skin ... Load the next skin
        BTN_LOCAL_BROWSER, // -- optional local browser button
        BTN_DIRUP, // --- optiona; dir up
        BTN_MAX
    };

    // -- format txt1..10
    #define TXT_MAX (10)


    enum // gui's
    {
        GUI_OPTIONS = 0,
        GUI_LOG,
        GUI_SEARCH,
        GUI_INFO,
        //GUI_CONNECT,
        GUI_MAX
    };

    enum // animated images ... only rip music image right now
    {
        GUI_IMG_FRAME_1 = 0,
        GUI_IMG_FRAME_2,
        GUI_IMG_FRAME_MAX
    };

    fonts*          fnts;
    visualizer*     vis;

    vector<gui_textbox*>    text_areas;

    gui_button*     buttons[BTN_MAX];
    gui_button**    buttons_listing;
    gui_button**    buttons_genre;
    gui_button**    buttons_playlists;
    gui_button**    buttons_delete;

    SDL_Surface*    cursor;
    SDL_Surface*    guibuffer;
    SDL_Surface*    visbuffer;
    SDL_Surface*    guibackground;
    SDL_Surface*    info_dlg;
    SDL_Surface*    vol_bg;
    SDL_Surface*    vol_ol;
    SDL_Surface*    dialog;
    SDL_Surface*    mute_img;
    SDL_Surface*    ripping_img[GUI_IMG_FRAME_MAX];
    SDL_Surface*    folder_img;
    SDL_Surface*    file_img;
    SDL_Surface*    dirup_img;

   // gui_search*     search_gui;

    int             genre_selected;

    int             vis_on_screen;
    SDL_Rect        vis_rect;
    unsigned long   vis_bgcolor;
    unsigned long   vis_color_bar;
    unsigned long   vis_color_peak;
    int             peakResults[MAX_FFT_RES];

    // gui's and pop-ups
    gui_dlg*    guis[GUI_MAX];


    unsigned long   dialog_text_color;

    genre_list      gl; // - genre list

    int             rip_image_idx; // current frame for rip image
    unsigned long   gui_last_time_rip;

    // keep track of time ... for animations ect...

    unsigned long   gui_current_time;

    app_wiiradio*   theapp;

    string          skin_path;

    unsigned long   visual_show_title;
    char            visual_last_track_title[SMALL_MEM];

    gui(app_wiiradio* _theapp, char* dir) :
        buttons_listing(0), buttons_genre(0),
        buttons_playlists(0), buttons_delete(0), genre_selected(0),
        vis_on_screen(0), dialog_text_color(0), rip_image_idx(0),
        gui_last_time_rip(0), gui_current_time(0),theapp(_theapp)
    {

        theapp->ui = this; // As we've not constructed this yet
        texture_cache*  tx = theapp->tx;
        skins*          sk = theapp->sk;

        fnts        = theapp->fnts;
        vis         = theapp->visuals;
        skin_path   = dir;

        ripping_img[GUI_IMG_FRAME_1] = ripping_img[GUI_IMG_FRAME_2] = 0;

        gl.load_file();

        text_areas.clear();

        unsigned long color, color2;

        visbuffer = 0;
        vis_rect.x = vis_rect.y = vis_rect.w = vis_rect.h = 0;
        vis_bgcolor = 0;

        guibuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH,
                                          rmask, gmask, bmask,amask);


        // search genre screen
      //  search_gui = new gui_search(this);


        char s_value1[SMALL_MEM];
        char s_value2[SMALL_MEM];

        if (!theapp->sk->load_skin(dir)) //skin
            exit(0);

        // -- how many listing used by this skin
        max_listings = theapp->sk->get_value_int("listing_number_of");

        // -- show visuals on screen
        vis_on_screen = sk->get_value_int("show_vis");
        if(vis_on_screen)
        {
            vis_rect.x = sk->get_value_int("vis_x");
            vis_rect.y = sk->get_value_int("vis_y");
            vis_rect.w = sk->get_value_int("vis_w");
            vis_rect.h = sk->get_value_int("vis_h");

            visbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,vis_rect.w,vis_rect.h,BITDEPTH,
                                          rmask, gmask, bmask,amask);

            SDL_SetColorKey(visbuffer,SDL_SRCCOLORKEY, SDL_MapRGB(visbuffer->format,0,0,0));

            vis_bgcolor = sk->get_value_color("vis_bgcolor");
            vis_color_bar = sk->get_value_color("vis_color_bar");
            vis_color_peak = sk->get_value_color("vis_color_peak");

            loopi(MAX_FFT_RES) {
                peakResults[i] = 0;
                vis->fft_results[i] = 0;
            }

        }


        // gui's
        loopi(GUI_MAX) guis[i] = 0;

        guis[GUI_OPTIONS]   = new gui_options(theapp);
        guis[GUI_LOG]       = new gui_log(theapp);
        guis[GUI_SEARCH]    = new gui_search(theapp);
        guis[GUI_INFO]      = new gui_info(theapp);
//        guis[GUI_SC_ERROR]  = new gui_sc_error(this); -- TODO
//        guis[GUI_CONNECT]   = new gui_connect(this);

        loopi(BTN_MAX) buttons[i] = 0; // NULL

        //fontset
        if(sk->get_value_file("fontset",s_value1,dir))
        {
            int fsizes[3];

            fsizes[2] = sk->get_value_int("fontsize_small");
            fsizes[1] = sk->get_value_int("fontsize_medium");
            fsizes[0] = sk->get_value_int("fontsize_large");

            // - deafult sizes
            if (!fsizes[0]) fsizes[0] = 14;
            if (!fsizes[1]) fsizes[1] = 22;
            if (!fsizes[2]) fsizes[2] = 26;

            fnts->reload_fonts(s_value1,fsizes);

        }

        // -- background
        if (!sk->get_value_file("bgimg",s_value1,dir)) exit(0);
        if(!(guibackground = tx->texture_lookup(s_value1))) exit(0);

        // -- cursor
        if (!sk->get_value_file("cursor",s_value1,dir)) exit(0);
        cursor = tx->texture_lookup(s_value1);

        // --- info
        if (!sk->get_value_file("info",s_value1,dir)) exit(0);
        info_dlg = tx->texture_lookup(s_value1);

        // -- volume overlay
        if (!sk->get_value_file("volume_bg",s_value1,dir)) exit(0);
        vol_bg = tx->texture_lookup(s_value1);
        if (!sk->get_value_file("volume_over",s_value1,dir)) exit(0);
        vol_ol = tx->texture_lookup(s_value1);

        // --- Diaglog bg
        if (!sk->get_value_file("dialog",s_value1,dir)) exit(0);
        dialog = tx->texture_lookup(s_value1);
        dialog_text_color = sk->get_value_color("dialog_text_color");


        // --- Mute image
        if (!sk->get_value_file("mute",s_value1,dir)) exit(0);
        mute_img = tx->texture_lookup(s_value1);

        // --- Ripping indicator ... (TODO add skin option?)
        if (!ripping_img[GUI_IMG_FRAME_1])
            ripping_img[GUI_IMG_FRAME_1] = tx->texture_lookup("imgs/record.png");

        if (!ripping_img[GUI_IMG_FRAME_2])
            ripping_img[GUI_IMG_FRAME_2] = tx->texture_lookup("imgs/record-blink.png");


        // --- folder image
        folder_img = 0;
        folder_img = tx->texture_lookup("imgs/folder.png");

        file_img = 0;
        file_img = tx->texture_lookup("imgs/file.png");

        int height, y_offset, x_offset;

        // list buttons

        height = 0;

        buttons_listing = new gui_button*[max_listings];
        if (!buttons_listing) exit(0);

        buttons_playlists = new gui_button*[max_listings];
        if (!buttons_playlists) exit(0);

        buttons_genre = new gui_button*[max_listings];
        if (!buttons_genre) exit(0);

        buttons_delete = new gui_button*[max_listings];
        if (!buttons_delete) exit(0);

        loopi(max_listings)
        {
            if (!sk->get_value_file("list_out",s_value1,dir)) exit(0);
            if (!sk->get_value_file("list_over",s_value2,dir)) exit(0);

            x_offset = sk->get_value_int("listing_start_x");//20;
            y_offset = sk->get_value_int("listing_start_y");//115;

            color = sk->get_value_color("listing_font_color");
            color2 = sk->get_value_color("listing_font_color_over");
            // stations

            buttons_listing[i] = new gui_button(theapp,x_offset,y_offset+i*height,NULL,color,true);
            buttons_listing[i]->set_images(s_value1,s_value2,0,0);
            buttons_listing[i]->center_text = false;
            buttons_listing[i]->limit_text = sk->get_value_int("listing_limit_text");//530;
            buttons_listing[i]->scroll_speed = 5;
            buttons_listing[i]->bind_screen = S_BROWSER;
            buttons_listing[i]->font_sz = sk->get_value_int("listing_font_size");
            buttons_listing[i]->pad_y = sk->get_value_int("listing_pad_text_y");
            buttons_listing[i]->pad_x = sk->get_value_int("listing_pad_text_x");
            buttons_listing[i]->text_color = sk->get_value_color("listing_font_color");
            buttons_listing[i]->text_color_over = sk->get_value_color("listing_font_color_over");
            buttons_listing[i]->click_func = click_item;
            buttons_listing[i]->object_id = i;

            //favs
            buttons_playlists[i] = new gui_button(theapp,x_offset,y_offset+i*height,NULL,color,true);
            buttons_playlists[i]->set_images(s_value1,s_value2,0,0);
            buttons_playlists[i]->center_text = false;
            buttons_playlists[i]->limit_text = sk->get_value_int("listing_limit_text");//530;
            buttons_playlists[i]->bind_screen = S_PLAYLISTS;
            buttons_playlists[i]->scroll_speed = 5;
            buttons_playlists[i]->font_sz = sk->get_value_int("listing_font_size");
            buttons_playlists[i]->pad_y = sk->get_value_int("listing_pad_text_y");
            buttons_playlists[i]->pad_x = sk->get_value_int("listing_pad_text_x");
            buttons_playlists[i]->text_color = sk->get_value_color("listing_font_color");
            buttons_playlists[i]->text_color_over = sk->get_value_color("listing_font_color_over");
            buttons_playlists[i]->click_func = click_item;
            buttons_playlists[i]->object_id = i;

            //genres
            buttons_genre[i] = new gui_button(theapp,x_offset,y_offset+i*height,NULL,color,false);
            buttons_genre[i]->set_images(s_value1,s_value2,0,0);
            buttons_genre[i]->limit_text = sk->get_value_int("listing_limit_text");//530;
            buttons_genre[i]->bind_screen = S_GENRES;
            buttons_genre[i]->font_sz = sk->get_value_int("listing_font_size");
            buttons_genre[i]->pad_y = sk->get_value_int("listing_pad_text_y");
            buttons_genre[i]->pad_x = sk->get_value_int("listing_pad_text_x");
            buttons_genre[i]->text_color = sk->get_value_color("listing_font_color");
            buttons_genre[i]->text_color_over = sk->get_value_color("listing_font_color_over");
            buttons_genre[i]->click_func = click_item;
            buttons_genre[i]->object_id = i;

            //delete buttons
            x_offset = sk->get_value_int("delete_start_x");


            if (!sk->get_value_file("delete_out",s_value1,dir)) exit(0);
            if (!sk->get_value_file("delete_over",s_value2,dir)) exit(0);

            buttons_delete[i] = new gui_button(theapp,x_offset,y_offset+i*height+sk->get_value_int("delete_pad_y"),NULL,0,false);
            buttons_delete[i]->set_images(s_value1,s_value2,0,0);
            buttons_delete[i]->z_order = 1;
            buttons_delete[i]->bind_screen = S_PLAYLISTS;
            buttons_delete[i]->click_func = click_delete_item;
            buttons_delete[i]->object_id = i;


            height = buttons_listing[i]->s_h + sk->get_value_int("listing_pad_y");

        }



        /*
            All the skinable buttons
        */

        const char* btn_descripts[] =
        {
            "next","back","pls","genres","playing","logo","add","stop",
            "exit","browser","showvisuals","search","nextskin","localbrowser","dirup"
        };

        func_void btn_onclick[] =
        {
            click_next,click_back,click_playlists,
            click_genres,click_playing,click_logo,click_add,
            click_stop,click_exit, click_browser, click_showvisuals,
            click_search, click_nextskin, click_localbrowser,
            click_dirup
        };

        loopi(BTN_MAX)
        {
             buttons[i] = new_button(btn_descripts[i],btn_onclick[i]);
        }

        // -- general text items
        loopi(TXT_MAX)
        {
            sprintf(s_value1,"txt%d",i+1);
            gui_textbox* x = new_textbox(s_value1);
            if(x) text_areas.push_back(x);
            else break; // no point in continuing .. text areas should use the next free value ..i.e. txt1,txt2 ...
        }
    };

    ~gui()
    {
        loopi(text_areas.size())
        {
            if(text_areas[i])
            {
                delete text_areas[i];
                text_areas[i] = 0;
            }
        }

        loopi(BTN_MAX)
        {
            if (buttons[i])
            {
                delete buttons[i];
                buttons[i] = 0;
            }
        }

        loopi(max_listings)
        {

            delete buttons_delete[i];
            buttons_delete[i] = 0;

            delete buttons_listing[i];
            buttons_listing[i] = 0;

            delete buttons_genre[i];
            buttons_genre[i] = 0;

            delete buttons_playlists[i];
            buttons_playlists[i] = 0;
        }

        delete [] buttons_delete;
        delete [] buttons_listing;
        delete [] buttons_playlists;
        delete [] buttons_genre;

        SDL_FreeSurface(guibuffer);

        if(visbuffer) SDL_FreeSurface(visbuffer);

        loopi(GUI_MAX)
        {
            if (guis[i])
            {
                delete guis[i];
                guis[i] = 0;
            }
        }

    };

    gui_textbox* new_textbox(const char* name)
    {
        char txt_tmp[SMALL_MEM];
        char s_value1[SMALL_MEM];
        int x, y;
        gui_textbox* textarea = 0;
        skins* sk = theapp->sk;

        sprintf(txt_tmp,"%s_x",name);
        x = sk->get_value_int(txt_tmp);
        sprintf(txt_tmp,"%s_y",name);
        y = sk->get_value_int(txt_tmp);

        sprintf(txt_tmp,"%s_text",name);
        sk->get_value_string(txt_tmp,s_value1);

        if (*s_value1)
        {
            textarea = new gui_textbox(theapp,x,y,NULL,0,false);
            textarea->set_text(gui_gettext(s_value1));

            if(s_value1[0] == '$')
                textarea->isvariable = true; // the value should be filled with a variable

            sprintf(txt_tmp,"%s_text_color",name);
            textarea->text_color = sk->get_value_color(txt_tmp);

            sprintf(txt_tmp,"%s_text_size",name);
            textarea->font_sz = sk->get_value_int(txt_tmp);

            sprintf(txt_tmp,"%s_bind",name); // the screen to bind to e.g. 0,1,2
            textarea->bind_screen = sk->get_value_int(txt_tmp);

            sprintf(txt_tmp,"%s_pad_x",name);
            textarea->pad_x = sk->get_value_int(txt_tmp);

            sprintf(txt_tmp,"%s_pad_y",name);
            textarea->pad_y = sk->get_value_int(txt_tmp);

            sprintf(txt_tmp,"%s_nocentertext",name);
            textarea->center_text = !sk->get_value_int(txt_tmp);


        }

        return textarea;
    }

    gui_button* new_button(const char* name,func_void onclick)
    {
        char item[SMALL_MEM];
        char s_value1[SMALL_MEM];
        char s_value2[SMALL_MEM];
        int x, y;
        char* dir = (char*)skin_path.c_str();

        gui_button* thebutton = 0;

        skins* sk = theapp->sk;

        sprintf(item,"%s_out",name);
        sk->get_value_file(item,s_value1,dir);

        sprintf(item,"%s_over",name);
        sk->get_value_file(item,s_value2,dir);

        sprintf(item,"%s_x",name);
        x = sk->get_value_int(item);

        sprintf(item,"%s_y",name);
        y = sk->get_value_int(item);

        if (*s_value1 && *s_value2)
        {
            int scroll_text = 0;
            sprintf(item,"%s_scroll",name);
            scroll_text = sk->get_value_int(item);

            thebutton = new gui_button(theapp,x,y,NULL,0,(bool)scroll_text);
            thebutton->set_images(s_value1,s_value2,0,0);

            sprintf(item,"%s_text",name);
            if (sk->get_value_string(item,s_value1)) thebutton->set_text(gui_gettext(s_value1));

            sprintf(item,"%s_limit_text",name);
            thebutton->limit_text = sk->get_value_int(item);

            sprintf(item,"%s_text_color",name);
            thebutton->text_color = sk->get_value_color(item);

            sprintf(item,"%s_text_color_over",name);
            thebutton->text_color_over = sk->get_value_color(item);

            sprintf(item,"%s_text_size",name);
            thebutton->font_sz = sk->get_value_int(item);

            sprintf(item,"%s_pad_y",name);
            thebutton->pad_y = sk->get_value_int(item);

            sprintf(item,"%s_pad_x",name);
            thebutton->pad_x = sk->get_value_int(item);

            sprintf(item,"%s_autoscroll",name);
            thebutton->auto_scroll_text = sk->get_value_int(item);

            sprintf(item,"%s_nocentertext",name);
            thebutton->center_text = !sk->get_value_int(item);

            // callbacks
            thebutton->click_func = onclick;
        }

        return thebutton;
    };

    /* ----------------------
       Button Click Functions
       ---------------------- */

    static void click_next(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        char* gc;

        switch(GetScreenStatus())
        {
            case S_BROWSER:
                gc = g->gl.get_genre(g->genre_selected);
                genre_nex_prev(true,gc);
            break;

            case S_GENRES:
                if (genre_display + max_listings  >= g->gl.total)
                    return;

                genre_display += max_listings;

            break;

            case S_PLAYLISTS:
                if (pls_display + max_listings  >= total_num_playlists)
                    return;

                pls_display += max_listings;


            break;

            case S_LOCALFILES:
                if (g->theapp->localfs->current_position + max_listings  >= g->theapp->localfs->total_num_files)
                    return;

                g->theapp->localfs->current_position += max_listings;

            break;

            default:
            break;
        }

        g->reset_scrollings();
    }

    static void click_back(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        char*   gc;

        switch(GetScreenStatus())
        {
        case S_BROWSER:
            gc = g->gl.get_genre(g->genre_selected);
            if (display_idx>1) genre_nex_prev(false,gc);
            //if (display_idx>1) genre_nex_prev(false,(char*)genres[genre_selected]);
        break;

        case S_PLAYLISTS:
            pls_display -= max_listings;
            if (pls_display < 0) {
                pls_display = 0;
            }

        break;

         case S_GENRES:
            genre_display -= max_listings;
            if (genre_display < 0)
                genre_display = 0;

         break;

         case S_LOCALFILES:
            g->theapp->localfs->current_position -= max_listings;
            if (g->theapp->localfs->current_position < 0)
                g->theapp->localfs->current_position = 0;

         break;


         default:
         break;


        }
        g->reset_scrollings();
    }

    static void click_add(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        if (status == PLAYING)
            g->theapp->request_save_fav(); // save the pls
    }

    static void click_stop(void* arg)
    {
        status = STOPPED;
    }
    static void click_exit(void* arg)
    {
        g_running = false;
    }

    static void click_browser(void* arg)
    {
        SetScreenStatus(S_BROWSER);
    }

    static void click_search(void* arg)
    {
        SetScreenStatus(S_SEARCHGENRE);
    }

    static void click_nextskin(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        g->theapp->next_skin();
    }

    static void click_localbrowser(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        g->theapp->localfs->directory_list(0); // Run search
        SetScreenStatus(S_LOCALFILES);
    }

    static void click_dirup(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        g->theapp->localfs->dirup();
    }

    static void click_showvisuals(void* arg)
    {
        SetScreenStatus(S_VISUALS);
    }

    static void click_logo(void* arg)
    {
#ifdef LOG_ENABLED
        SetScreenStatus(S_LOG);
#else
        SetScreenStatus(S_OPTIONS);
#endif
    }

    static void click_playing(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        g->buttons[BTN_PLAYING]->scroll_reset();
    }

    static void click_genres(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        SetScreenStatus(S_GENRES);
        g->reset_scrollings();
    }

    static void click_playlists(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        SetScreenStatus(S_PLAYLISTS);
        g->reset_scrollings();
    }

    static void click_item(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        localfiles* lf = g->theapp->localfs;
        int i = obj->object_id;
        int connect;
        char* gc = 0;

        switch(GetScreenStatus())
        {

            case S_BROWSER:
                g->theapp->connect_to_stream(i,I_STATION);
            break;

            case S_LOCALFILES:

                connect = i + lf->current_position;
                if (connect >= lf->total_num_files || connect < 0)
                    return;

                if (lf->list[connect].isfolder)
                {
                    lf->directory_list(lf->list[connect].path.c_str());
                }
                else
                {
                    g->theapp->connect_to_stream(connect,I_LOCAL);
                }

            break;

            case S_PLAYLISTS:

                connect = i + pls_display;
                if (connect>= total_num_playlists || connect < 0)
                    return;

                g->theapp->connect_to_stream(connect,I_PLAYLIST);

            break;

            case S_GENRES:

                g->genre_selected = i;
                g->genre_selected += genre_display;

                if (g->genre_selected >= g->gl.total || g->genre_selected < 0)
                    return;

                gc = g->gl.get_genre(g->genre_selected);
                if (!gc) return;

                g->theapp->search_function(gc,SEARCH_GENRE); // do the search .. switch to browser
            break;

        }//switch

        g->reset_scrollings();

    }


    static void click_delete_item(void* arg)
    {
        const gui_object* obj = (gui_object*)arg;
        gui* g = (gui*)(obj->theapp->ui);

        int i = obj->object_id;
        int delete_item = -1;

        switch(GetScreenStatus())
        {
            case S_PLAYLISTS:

                delete_item = i + pls_display;
                if (delete_item >= total_num_playlists)
                    return;

                g->theapp->delete_playlist(delete_item);

            break;

        }

        g->reset_scrollings();
    }

    void reset_scrollings()
    {
        loopj(max_listings) {
            buttons_playlists[j]->scroll_reset();
            buttons_listing[j]->scroll_reset();
        }
    };

    void handle_options()
    {
        if (!g_oscrolltext)
        {
            loopi(max_listings) buttons_listing[i]->can_scroll = false;
            loopi(max_listings) buttons_playlists[i]->can_scroll = false;
        }else{
            loopi(max_listings) buttons_listing[i]->can_scroll = true;
            loopi(max_listings) buttons_playlists[i]->can_scroll = true;
        }
    };

    int handle_events(SDL_Event* events)
    {
        if ((events->type != SDL_MOUSEMOTION &&
            events->type != SDL_MOUSEBUTTONDOWN &&
            events->type != SDL_MOUSEBUTTONUP) ||
            (GetScreenStatus()==S_VISUALS))
            return -1;



        handle_options();

        // -- cancel buffering
        if (status == BUFFERING || status == CONNECTING)
        {
            if(guis[GUI_INFO]->cancel->hit_test(events,1)==B_CLICK)
            {
                status = STOPPED;
            }

            return 0;
        }


        // reset the "Canceled" state
        guis[GUI_INFO]->cancel->obj_state = B_OUT;

        switch(GetScreenStatus())
        {
            case S_OPTIONS:
            guis[GUI_OPTIONS]->handle_events(events);
            return 0;

            case S_LOG:
            return guis[GUI_LOG]->handle_events(events);

            case S_SEARCHGENRE:
            return guis[GUI_SEARCH]->handle_events(events);

            case S_SEARCHING:
            return 0; // no hit test .. TODO

            default:
            break;

        }

        bloopj(MAX_Z_ORDERS)
        {
            loopi(BTN_MAX)
            {
                if (buttons[i]) // could be optional button
                {
                    if (buttons[i]->hit_test(events,j))
                        return 0;
                }
            }

            loopi(max_listings)
            {
                switch(GetScreenStatus())
                {
                    case S_BROWSER:
                    {
                        if(buttons_listing[i]->hit_test(events,j))
                            return 0;
                    }
                    break;

                    case S_GENRES:
                    {
                        if(buttons_genre[i]->hit_test(events,j))
                            return 0;
                    }
                    break;

                    case S_LOCALFILES:
                    case S_PLAYLISTS:
                    {
                        if(buttons_playlists[i]->hit_test(events,j))
                            return 0;

                        if(buttons_delete[i]->hit_test(events,j))
                            return 0;
                    }
                    break;
                }
            }

        } //z-order loop


        return -1;

    };


    void inline draw_folder(const int x, const int y)
    {
        SDL_Rect dr = {x,y,folder_img->w,folder_img->h};
        SDL_BlitSurface(folder_img,0,guibuffer,&dr);
    };

    void inline draw_file(const int x, const int y)
    {
        SDL_Rect dr = {x,y,file_img->w,file_img->h};
        SDL_BlitSurface(file_img,0,guibuffer,&dr);
    };

    void inline dir_up()
    {
        if (buttons[BTN_DIRUP]) buttons[BTN_DIRUP]->draw();
    };

    void inline draw_local_files()
    {
        unsigned int i = 0;
        unsigned int p = 0;

        localfiles* lf = theapp->localfs;

        dir_up();

        while(i<(unsigned int)max_listings && p<lf->list.size())
        {
            if (p >= (unsigned int)lf->current_position) {
                buttons_playlists[i]->set_text(lf->list[p].name.c_str());
                buttons_playlists[i]->draw();

                if (lf->list[p].isfolder)
                    draw_folder(buttons_playlists[i]->s_w, buttons_playlists[i]->s_y + buttons_playlists[i]->pad_y);
                else
                    draw_file(buttons_playlists[i]->s_w, buttons_playlists[i]->s_y + buttons_playlists[i]->pad_y);

                i++;
            }
            p++;
        }

    }

    void inline draw_favs(const favorites *favs)
    {
        unsigned int i = 0;
        unsigned int p = 0;

        while(i<(unsigned int)max_listings && p<favs->list.size())
        {
            if (p >= (unsigned int)pls_display) {
                buttons_playlists[i]->set_text(favs->list[p].name.c_str());
                buttons_playlists[i]->draw();
                // delete option
                buttons_delete[i]->draw();

                i++;
            }
            p++;
        }
    }

    void draw()
    {
        station_list* current_list  = theapp->scb->sl_first;
        icy* ic                     = theapp->icy_info;
        favorites* favs             = theapp->favs;
        local_player* lpb           = theapp->localpb;

        gui_current_time = SDL_GetTicks();

        draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear the buffer

        if (GetScreenStatus() == S_VISUALS)
        {
            black_screen_saver();
            if (visualize_number < MAX_VISUALS) {
                if(playback_type == AS_LOCAL)
                {
                  //  localpb->user_data = ic->track_title;
                }else{
                    vis->user_data = ic->track_title;
                }
                vis->draw_visuals(guibuffer,visualize_number);

                if(get_tick_count() - visual_show_title < 60000) // -- hide after 1 min
                {
                    fnts->change_color(200,200,200);
                    fnts->set_size(FS_SYSTEM);
                    fnts->text(guibuffer,trim_string(ic->track_title,100),10,32,0);
                }else{
                    if (strcmp(visual_last_track_title,ic->track_title)!=0)
                    {
                        strcpy(visual_last_track_title,ic->track_title);
                        visual_show_title = get_tick_count();
                    }
                }

            }

        }else{


            SDL_BlitSurface(guibackground,0, guibuffer,0); //background
            buttons[BTN_LOGO]->draw(); // title

            // Text that can be defined in the skin
            loopi(text_areas.size())
                if(text_areas[i] && (GetScreenStatus()==text_areas[i]->bind_screen))
                    text_areas[i]->draw();


            if (GetScreenStatus() != S_OPTIONS && GetScreenStatus() != S_SEARCHGENRE &&
                GetScreenStatus() != S_LOG)
            {
                if(vis_on_screen)
                {
                    render_gui_bars();
                }
                if (status != STOPPED && status != FAILED)
                {
                    buttons[BTN_PLAYING]->auto_scroll_text = true;
                    if (playback_type == AS_LOCAL) buttons[BTN_PLAYING]->set_text(lpb->display_name.c_str());
                    else buttons[BTN_PLAYING]->set_text(ic->track_title);
                }else{
                    buttons[BTN_PLAYING]->auto_scroll_text = true;
                    char s_status[TINY_MEM] = {0};
                    char* l = 0;
                    switch(status){
                        case STOPPED:
                            l = vars.search_var("$LANG_TXT_STOPPED");
                        break;
                        case PLAYING:
                            l = vars.search_var("$LANG_TXT_PLAYING");
                        break;
                        case BUFFERING:
                            l = vars.search_var("$LANG_TXT_BUFFERING");
                        break;
                        case FAILED:
                            l = vars.search_var("$LANG_TXT_ERROR");
                        break;
                        case CONNECTING:
                            l = vars.search_var("$LANG_TXT_CONNECTING");
                        break;
                    }
                    if (l) strcpy(s_status,l);

                    buttons[BTN_PLAYING]->set_text(s_status);
                }

                buttons[BTN_PLAYING]->draw();

                switch(GetScreenStatus())
                {

                    case S_BROWSER:

                    if (!sc_error)
                    {
                        // loop through stations
                        station_list* csl = current_list;
                        int i = 0;
                        int p = 0;
                        while(csl)
                        {
                            if (i==max_listings) break;

                            if (p >= display_idx) {
                                buttons_listing[i]->set_text(csl->station_name);
                                buttons_listing[i]->draw();
                                i++;
                            }
                            p++;
                            csl = csl->nextnode;
                        }
                    }else{
                        draw_sc_error();
                    }

                    break;

                    case S_PLAYLISTS:
                        draw_favs(favs);
                    break;

                    case S_LOCALFILES:
                        draw_local_files();
                    break;

                    case S_GENRES:

                    loopi(max_listings)
                    {

                        //if (i+genre_display < MAX_GENRE) {
                        if (i+genre_display < gl.total)
                        {
                            char* g = gl.get_genre(i+genre_display);
                            if (g) buttons_genre[i]->set_text(g);
                            buttons_genre[i]->draw();
                        }

                    }

                    break;

                    case S_STREAM_INFO:
                        switch(playback_type)
                        {
                            case AS_LOCAL:
                                draw_mp3_details();
                            break;
                            case AS_SHOUTCAST:
                            case AS_ICECAST:
                                draw_stream_details();
                            break;
                            default:
                                draw_stream_details();
                            break;
                        }


                    break;


                } // switch

                if (GetScreenStatus() != S_STREAM_INFO)
                {
                    //more / prior ... used for browser, playlist and genre selection
                    buttons[BTN_NEXT]->draw();
                    buttons[BTN_PRIOR]->draw();

                    if (buttons[BTN_ADD]) buttons[BTN_ADD]->draw(); // -- optional button
                    if (buttons[BTN_STOP]) buttons[BTN_STOP]->draw(); // -- optional button
                    if (buttons[BTN_EXIT]) buttons[BTN_EXIT]->draw(); // -- optional button
                    if (buttons[BTN_BROWSER]) buttons[BTN_BROWSER]->draw(); // -- optional button
                    if (buttons[BTN_VISUALS]) buttons[BTN_VISUALS]->draw(); // -- optional button
                    if (buttons[BTN_SEARCH]) buttons[BTN_SEARCH]->draw(); // -- optional button
                    if (buttons[BTN_NX_SKIN]) buttons[BTN_NX_SKIN]->draw(); // -- optional button
                    if (buttons[BTN_LOCAL_BROWSER]) buttons[BTN_LOCAL_BROWSER]->draw(); // -- optional button
                    //genre select
                    buttons[BTN_GENRES_SELECT]->draw();
                    // playlists
                    buttons[BTN_PLAYLISTS]->draw();
                }


            }
        }

        if (GetScreenStatus() == S_SEARCHGENRE) guis[GUI_SEARCH]->draw();
        if (GetScreenStatus() == S_OPTIONS) draw_about();
        if (GetScreenStatus() == S_LOG) draw_log();



        if (GetScreenStatus() != S_VISUALS)
        {
            // always inform user if buffering
            if (status == BUFFERING) draw_info(vars.search_var("$LANG_TXT_BUFFERING"));
            if (status == CONNECTING) draw_info(vars.search_var("$LANG_TXT_CONNECTING"));
            if (GetScreenStatus() == S_SEARCHING) draw_info(vars.search_var("$LANG_TXT_SEARCHING"));

            visual_show_title = get_tick_count();
            strcpy(visual_last_track_title, ic->track_title);

            draw_recording();
        }

        // volume display ... like an OSD
        draw_volume();

    };

    void inline draw_info(char* txt)
    {
        guis[GUI_INFO]->draw(txt);
    };

    void inline draw_sc_error()
    {
        fnts->change_color((dialog_text_color >> 16), ((dialog_text_color >> 8) & 0xff),(dialog_text_color & 0xff));
        SDL_Rect t = {48,(400 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(dialog,0, guibuffer,&t);

        fnts->text(guibuffer,vars.search_var("$LANG_TXT_SC_ERROR_l1"),t.x + 94,t.y + 45,0);
        fnts->text(guibuffer,vars.search_var("$LANG_TXT_SC_ERROR_l2"),t.x + 94,t.y + 72,0);
    }

    void inline draw_about()
    {
        guis[GUI_OPTIONS]->draw();
    };

    void inline draw_log()
    {
        guis[GUI_LOG]->draw();
    };

    char *logtext()
    {
        return guis[GUI_LOG]->text;
    };

    void inline draw_stream_details()
    {
        icy*                    ic = theapp->icy_info;
        const unsigned long     text_color = 0;
        char*                   lang = 0;

        fnts->change_color((text_color >> 16), ((text_color >> 8) & 0xff),(text_color & 0xff));
        fnts->set_size(FS_SMALL);

        SDL_Rect t = {220,SCREEN_HEIGHT_D2 - (dialog->h / 2),0,0};

        SDL_FillRect(guibuffer, 0, 0xE6E6E6FF); // bgcolor

        lang = vars.search_var("$LANG_TXT_STATION");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s", lang,trim_string(ic->icy_name,50)),t.x + 35,t.y + 38,0);
        lang = vars.search_var("$LANG_TXT_URL");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s",lang,trim_string(ic->icy_url,50)),t.x + 35,t.y + 58,0);
        lang = vars.search_var("$LANG_TXT_BR");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %dKbps",lang,ic->icy_br),t.x + 35,t.y + 78,0);
        lang = vars.search_var("$LANG_TXT_TITLE");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s",lang,trim_string(ic->track_title,50)),t.x + 35,t.y + 98,0);

    };

    void inline draw_mp3_details()
    {
        local_player*           lp = theapp->localpb;
        const unsigned long     text_color = 0;
        char*                   lang = 0;
        SDL_Surface*            APIC_Surface = 0;
        SDL_Rect                APIC_dst = {20,(SCREEN_HEIGHT_D2 - APIC_SIZE_D2),APIC_SIZE,APIC_SIZE};
        const int               text_x = APIC_dst.w + APIC_dst.x + 20;
        const int               text_y = APIC_dst.y + 15;

        SDL_FillRect(guibuffer, 0, 0xE6E6E6FF); // bgcolor

        fnts->change_color((text_color >> 16), ((text_color >> 8) & 0xff),(text_color & 0xff));

        rectangleColor(guibuffer,APIC_dst.x-2,APIC_dst.y-2, APIC_dst.x+APIC_dst.w+2,APIC_dst.y+APIC_dst.h+2,0x0f0f0fff); // outline for picture

        // TODO -- Add langs
        fnts->set_size(FS_SYSTEM);
        lang = vars.search_var("$LANG_TXT_TITLE");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s",lang,lp->display_name.c_str()),20,100,0);

        fnts->text(guibuffer,lp->artist.c_str(),text_x,text_y,0);
        fnts->text(guibuffer,lp->title.c_str(),text_x,text_y+20,0);
        fnts->text(guibuffer,lp->album.c_str(),text_x,text_y+40,0);
        fnts->text(guibuffer,lp->year.c_str(),text_x,text_y+60,0);
        fnts->text(guibuffer,lp->genre.c_str(),text_x,text_y+80,0);
        fnts->text(guibuffer,lp->comments.c_str(),text_x,text_y+100,0);

        APIC_Surface = theapp->localpb->APIC_Surface;

        if (APIC_Surface)
        {
            SDL_BlitSurface(APIC_Surface,0, guibuffer,&APIC_dst);
        }

    };
    void inline draw_recording()
    {
        if (g_oripmusic && !theapp->screen_sleeping && ( status==PLAYING || status== BUFFERING ))
        {
            if ((gui_current_time - gui_last_time_rip) > 500)
            {
                rip_image_idx = !rip_image_idx;
                gui_last_time_rip = gui_current_time;
            }

            SDL_Rect d = {SCREEN_WIDTH -100,50,ripping_img[rip_image_idx]->w,ripping_img[rip_image_idx]->h};
            SDL_BlitSurface(ripping_img[rip_image_idx],0, guibuffer,&d);

        }

    }

    void inline draw_volume()
    {
        if (mute && !theapp->screen_sleeping) // always show if muted
        {
            SDL_Rect d = {20,50,mute_img->w,mute_img->h};
            SDL_BlitSurface(mute_img,0, guibuffer,&d);
        }

        if((get_tick_count() - g_vol_lasttime) < 2000) // show for 2 secs after change
        {
            SDL_Rect r = { (SCREEN_WIDTH / 2) - (vol_bg->w / 2),(SCREEN_HEIGHT / 2) - (vol_bg->h / 2),vol_bg->w,vol_bg->h };

            // bg
            SDL_BlitSurface( vol_bg,0, guibuffer,&r);

            // actual volume bar
            int pc =  (int)(((float)volume / 255.0) * (float)vol_ol->w);

            pc > vol_ol->w ? pc = vol_ol->w : pc < 0 ? pc = 0 : 0; // clip

            // overlay
            r.x += 18;
            SDL_Rect s = { 0,0, pc,vol_ol->h };
            SDL_BlitSurface( vol_ol,&s, guibuffer,&r);
        }
    };

    void inline draw_cursor(int x,int y, float angle)
    {
        /* Cursor rotation */
        SDL_Surface *rot = rotozoomSurface (cursor, angle, 1, 0);
        if(!rot) return; // don't count on rotozoomSurface to always allocate surface
        SDL_Rect r = { x-rot->w/2, y-rot->h/2, rot->w, rot->h };
        SDL_BlitSurface(rot, 0, guibuffer, &r);
        SDL_FreeSurface(rot); // free
    };

    char* gui_gettext(char* t)
    {
        if (*t=='$') // is a variable
        {
            char* r = vars.search_var(t);
            if (!r) return t;

            return r;
        }

        return t;
    };

    void inline black_screen_saver()
    {
        draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    };

    // -- for on screen bars
    void inline render_gui_bars()
    {

        int x = vis_rect.x;
        int y = vis_rect.y + vis_rect.h;
        int bar_height = vis_rect.h;
        int bar_width = vis_rect.w / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);

        loopi(MAX_FFT_RES)
        {
            // peak
            peakResults[i] -= (32767 / bar_height) * 4;

            if(peakResults[i] < 0) peakResults[i] = 0;
            if(peakResults[i] < vis->fft_results[i]) peakResults[i] = vis->fft_results[i];

            int peaks_newy = (y - (int)(percent * (double)peakResults[i]));

            // fft
            int newY = (y - (int)(percent * (double)vis->fft_results[i]));
            int h = abs(y - newY);

            // draw
            draw_rect(guibuffer,x+i*bar_width,newY,bar_width / 2,h,vis_color_bar );
            draw_rect(guibuffer,x+i*bar_width,peaks_newy,bar_width/2,3,vis_color_peak);
        }

    };


};

#endif //_GUI_H_
