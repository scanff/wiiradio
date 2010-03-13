#ifndef _GUI_H_
#define _GUI_H_

#include "../genre_list.h"

class gui {


    // -- everyone knows who gui is :)
    #include "gui_dlg.h"
    #include "gui_button.h"
    #include "gui_textbox.h"
    #include "gui_options.h"
    #include "gui_search.h"
    #include "gui_log.h"

    public:

    // -- Buttons
    enum {
        BTN_NEXT = 0,
        BTN_PRIOR,
        BTN_PLAYLISTS,
        BTN_GENRES_SELECT,
        BTN_PLAYING,
        BTN_LOGO,
        BTN_CANCEL, // cancel buffering or connecting !
        // -- OPTIONAL BUTTONS
        BTN_ADD, // -- optional button
        BTN_STOP, // -- optional stop button
        BTN_EXIT, // -- optional exit button on skin button
        BTN_BROWSER, // -- optional browser ... displays the browser items
        BTN_VISUALS, // -- optional Show Visuals ... Shows the visuals screen
        BTN_SEARCH, // -- optional Show search screen ... Shows the search screen
        BTN_NX_SKIN, // -- optional next skin ... Load the next skin
        BTN_MAX
    };

    // -- General text items - All Optional
    enum {
        TXT_ONE = 0,
        TXT_TWO,
        TXT_THREE,
        TXT_FOUR,
        TXT_FIVE,
        TXT_SIX,
        TXT_SEVEN,
        TXT_EIGHT,
        TXT_NINE,
        TXT_TEN,
        TXT_MAX
    };

    enum // gui's
    {
        GUI_OPTIONS = 0,
        GUI_LOG,
        GUI_SEARCH,
        GUI_MAX
    };


    fonts*          fnts;
    visualizer*     vis;

    gui_textbox*    text_areas[TXT_MAX];
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

    int             info_text_x;
    int             info_text_y;
    int             info_text_size;
    int             info_cancel_x;
    int             info_cancel_y;


    genre_list      gl; // - genre list

    gui(fonts* f,visualizer* v, char* dir) :
        fnts(f), vis(v), buttons_listing(0), buttons_genre(0),
        buttons_playlists(0), buttons_delete(0), genre_selected(0),
        vis_on_screen(0), dialog_text_color(0)
    {

        gl.load_file();

        loopi(TXT_MAX) text_areas[i] = 0; // clear


        unsigned long rmask, gmask, bmask, amask, color, color2;
        int x,y;

        visbuffer = 0;
        vis_rect.x = vis_rect.y = vis_rect.w = vis_rect.h = 0;
        vis_bgcolor = 0;
//for 24bit

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#else
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#endif


        guibuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH,
                                          rmask, gmask, bmask,amask);


        // search genre screen
      //  search_gui = new gui_search(this);


        char s_value1[SMALL_MEM];
        char s_value2[SMALL_MEM];

        if (!sk->load_skin(dir)) //skin
            exit(0);

        // -- how many listing used by this skin
        max_listings = sk->get_value_int("listing_number_of");

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
        loopi(GUI_MAX) guis[GUI_MAX] = 0;

        guis[GUI_OPTIONS]   = new gui_options(this);
        guis[GUI_LOG]       = new gui_log(this);
        guis[GUI_SEARCH]    = new gui_search(this);


        loopi(BTN_MAX) buttons[i] = 0; // NULL

        //fontset
        if(sk->get_value_file("fontset",s_value1,dir))
        {
            int s = sk->get_value_int("fontsize_small");
            int m = sk->get_value_int("fontsize_medium");
            int l = sk->get_value_int("fontsize_large");

            // - deafult sizes
            if (s==0) s = 14;
            if (m==0) m = 22;
            if (l==0) l = 26;

            fnts->reload_fonts(s_value1,s,m,l);

        }

        //cursor HIT offsets
        Y_OFFSET = sk->get_value_int("cursor_y_off");
        X_OFFSET = sk->get_value_int("cursor_x_off");


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


        //more
        if (!sk->get_value_file("next_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("next_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("next_x");
        y = sk->get_value_int("next_y");
        buttons[BTN_NEXT] = new gui_button(guibuffer,f,x,y,NULL,0,false);
        buttons[BTN_NEXT]->set_images(s_value1,s_value2,0,0);
        if (sk->get_value_string("next_text",s_value1)) buttons[BTN_NEXT]->set_text(gui_gettext(s_value1));
        buttons[BTN_NEXT]->text_color = sk->get_value_color("next_text_color");
        buttons[BTN_NEXT]->text_color_over = sk->get_value_color("next_text_color_over");
        buttons[BTN_NEXT]->font_sz = sk->get_value_int("next_text_size");
        buttons[BTN_NEXT]->pad_y = sk->get_value_int("next_pad_y");
        buttons[BTN_NEXT]->pad_x = sk->get_value_int("next_pad_x");

        //back
        if (!sk->get_value_file("back_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("back_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("back_x");
        y = sk->get_value_int("back_y");
        buttons[BTN_PRIOR] = new gui_button(guibuffer,f,x,y,NULL,0,false);
        buttons[BTN_PRIOR]->set_images(s_value1,s_value2,0,0);
        if (sk->get_value_string("back_text",s_value1)) buttons[BTN_PRIOR]->set_text(gui_gettext(s_value1));
        buttons[BTN_PRIOR]->text_color = sk->get_value_color("back_text_color");
        buttons[BTN_PRIOR]->text_color_over = sk->get_value_color("back_text_color_over");
        buttons[BTN_PRIOR]->font_sz = sk->get_value_int("back_text_size");
        buttons[BTN_PRIOR]->pad_y = sk->get_value_int("back_pad_y");
        buttons[BTN_PRIOR]->pad_x = sk->get_value_int("back_pad_x");


        //genre sel
        if (!sk->get_value_file("genres_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("genres_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("genres_x");
        y = sk->get_value_int("genres_y");
        buttons[BTN_GENRES_SELECT] = new gui_button(guibuffer,f,x,y,NULL,0,false);
        buttons[BTN_GENRES_SELECT]->set_images(s_value1,s_value2,0,0);
        if (sk->get_value_string("genres_text",s_value1)) buttons[BTN_GENRES_SELECT]->set_text(gui_gettext(s_value1));
        buttons[BTN_GENRES_SELECT]->font_sz = sk->get_value_int("genres_text_size");
        buttons[BTN_GENRES_SELECT]->pad_y = sk->get_value_int("genres_pad_y");
        buttons[BTN_GENRES_SELECT]->pad_x = sk->get_value_int("genres_pad_x");
        buttons[BTN_GENRES_SELECT]->text_color = sk->get_value_color("genres_text_color");
        buttons[BTN_GENRES_SELECT]->text_color_over = sk->get_value_color("genres_text_color_over");

        //playlists sel
        if (!sk->get_value_file("pls_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("pls_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("pls_x");
        y = sk->get_value_int("pls_y");

        buttons[BTN_PLAYLISTS] = new gui_button(guibuffer,f,x,y,NULL,0,false);
        buttons[BTN_PLAYLISTS]->set_images(s_value1,s_value2,0,0);
        if (sk->get_value_string("pls_text",s_value1)) buttons[BTN_PLAYLISTS]->set_text(gui_gettext(s_value1));
        buttons[BTN_PLAYLISTS]->font_sz = sk->get_value_int("pls_text_size");
        buttons[BTN_PLAYLISTS]->pad_y = sk->get_value_int("pls_pad_y");
        buttons[BTN_PLAYLISTS]->pad_x = sk->get_value_int("pls_pad_x");
        buttons[BTN_PLAYLISTS]->text_color = sk->get_value_color("pls_text_color");
        buttons[BTN_PLAYLISTS]->text_color_over = sk->get_value_color("pls_text_color_over");


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



            buttons_listing[i] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,NULL,color,true);
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

            //favs
            buttons_playlists[i] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,NULL,color,true);
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

            //genres
            buttons_genre[i] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,NULL,color,false);
            buttons_genre[i]->set_images(s_value1,s_value2,0,0);
            buttons_genre[i]->limit_text = sk->get_value_int("listing_limit_text");//530;
            buttons_genre[i]->bind_screen = S_GENRES;
            buttons_genre[i]->font_sz = sk->get_value_int("listing_font_size");
            buttons_genre[i]->pad_y = sk->get_value_int("listing_pad_text_y");
            buttons_genre[i]->pad_x = sk->get_value_int("listing_pad_text_x");
            buttons_genre[i]->text_color = sk->get_value_color("listing_font_color");
            buttons_genre[i]->text_color_over = sk->get_value_color("listing_font_color_over");

            //delete buttons
            x_offset = sk->get_value_int("delete_start_x");//530;


            if (!sk->get_value_file("delete_out",s_value1,dir)) exit(0);
            if (!sk->get_value_file("delete_over",s_value2,dir)) exit(0);

            buttons_delete[i] = new gui_button(guibuffer,f,x_offset,y_offset+i*height+sk->get_value_int("delete_pad_y"),NULL,0,false);
            buttons_delete[i]->set_images(s_value1,s_value2,0,0);
            buttons_delete[i]->z_order = 1;
            buttons_delete[i]->bind_screen = S_PLAYLISTS;

            height = buttons_listing[i]->s_h + sk->get_value_int("listing_pad_y");

        }

        // -- playing area
        if (!sk->get_value_file("playing_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("playing_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("playing_x");
        y = sk->get_value_int("playing_y");
        buttons[BTN_PLAYING] = new gui_button(guibuffer,f,x,y,NULL,0,true);
        buttons[BTN_PLAYING]->set_images(s_value1,s_value2,0,0);
        buttons[BTN_PLAYING]->limit_text = sk->get_value_int("playing_limit_text");
        buttons[BTN_PLAYING]->auto_scroll_text = true;
        buttons[BTN_PLAYING]->center_text = false;
        buttons[BTN_PLAYING]->font_sz = sk->get_value_int("playing_text_size");
        buttons[BTN_PLAYING]->pad_y = sk->get_value_int("playing_pad_text_y");
        buttons[BTN_PLAYING]->pad_x = sk->get_value_int("playing_pad_text_x");
        buttons[BTN_PLAYING]->text_color = sk->get_value_color("playing_text_color");
        buttons[BTN_PLAYING]->text_color_over = sk->get_value_color("playing_text_color_over");
        // --------


        // logo
        if (!sk->get_value_file("logo_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("logo_over",s_value2,dir)) exit(0);
        x = sk->get_value_int("logo_x");
        y = sk->get_value_int("logo_y");
        buttons[BTN_LOGO] = new gui_button(guibuffer,f,x,y,NULL,0,false);
        buttons[BTN_LOGO]->set_images(s_value1,s_value2,0,0);
        // --------

        // cancel
        if (!sk->get_value_file("info_cancel_out",s_value1,dir)) exit(0);
        if (!sk->get_value_file("info_cancel_over",s_value2,dir)) exit(0);
        buttons[BTN_CANCEL] = new gui_button(guibuffer,f,0,25,NULL,0,false);
        buttons[BTN_CANCEL]->set_images(s_value1,s_value2,0,0);
        buttons[BTN_CANCEL]->center_text = true;
        buttons[BTN_CANCEL]->z_order =  1;
        if (sk->get_value_string("info_cancel_text",s_value1)) buttons[BTN_CANCEL]->set_text(gui_gettext(s_value1));
        buttons[BTN_CANCEL]->text_color = sk->get_value_color("info_cancel_text_color");
        buttons[BTN_CANCEL]->text_color_over = sk->get_value_color("info_cancel_text_color_over");
        buttons[BTN_CANCEL]->font_sz = sk->get_value_int("info_cancel_text_size");
        buttons[BTN_CANCEL]->pad_y = sk->get_value_int("info_cancel_text_pad_y");

        // --- optional skin buttons
        // add to pls
        if (!sk->get_value_file("add_out",s_value1,dir));
        if (!sk->get_value_file("add_over",s_value2,dir));
        x = sk->get_value_int("add_x");
        y = sk->get_value_int("add_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_ADD] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_ADD]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("add_text",s_value1)) buttons[BTN_ADD]->set_text(gui_gettext(s_value1));
            buttons[BTN_ADD]->text_color = sk->get_value_color("add_text_color");
            buttons[BTN_ADD]->text_color_over = sk->get_value_color("add_text_color_over");
            buttons[BTN_ADD]->font_sz = sk->get_value_int("add_text_size");
            buttons[BTN_ADD]->pad_y = sk->get_value_int("add_pad_y");
            buttons[BTN_ADD]->pad_x = sk->get_value_int("add_pad_x");
        }

        // stop button
        if (!sk->get_value_file("stop_out",s_value1,dir));
        if (!sk->get_value_file("stop_over",s_value2,dir));
        x = sk->get_value_int("stop_x");
        y = sk->get_value_int("stop_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_STOP] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_STOP]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("stop_text",s_value1)) buttons[BTN_STOP]->set_text(gui_gettext(s_value1));
            buttons[BTN_STOP]->text_color = sk->get_value_color("stop_text_color");
            buttons[BTN_STOP]->text_color_over = sk->get_value_color("stop_text_color_over");
            buttons[BTN_STOP]->font_sz = sk->get_value_int("stop_text_size");
            buttons[BTN_STOP]->pad_y = sk->get_value_int("stop_pad_y");
            buttons[BTN_STOP]->pad_x = sk->get_value_int("stop_pad_x");
        }

        // an exit button
        if (!sk->get_value_file("exit_out",s_value1,dir));
        if (!sk->get_value_file("exit_over",s_value2,dir));
        x = sk->get_value_int("exit_x");
        y = sk->get_value_int("exit_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_EXIT] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_EXIT]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("exit_text",s_value1)) buttons[BTN_EXIT]->set_text(gui_gettext(s_value1));
            buttons[BTN_EXIT]->text_color = sk->get_value_color("exit_text_color");
            buttons[BTN_EXIT]->text_color_over = sk->get_value_color("exit_text_color_over");
            buttons[BTN_EXIT]->font_sz = sk->get_value_int("exit_text_size");
            buttons[BTN_EXIT]->pad_y = sk->get_value_int("exit_pad_y");
            buttons[BTN_EXIT]->pad_x = sk->get_value_int("exit_pad_x");

        }

        // browser button (optional)
        if (!sk->get_value_file("browser_out",s_value1,dir));
        if (!sk->get_value_file("browser_over",s_value2,dir));
        x = sk->get_value_int("browser_x");
        y = sk->get_value_int("browser_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_BROWSER] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_BROWSER]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("browser_text",s_value1)) buttons[BTN_BROWSER]->set_text(gui_gettext(s_value1));
            buttons[BTN_BROWSER]->text_color = sk->get_value_color("browser_text_color");
            buttons[BTN_BROWSER]->text_color_over = sk->get_value_color("browser_text_color_over");
            buttons[BTN_BROWSER]->font_sz = sk->get_value_int("browser_text_size");
            buttons[BTN_BROWSER]->pad_y = sk->get_value_int("browser_pad_y");
            buttons[BTN_BROWSER]->pad_x = sk->get_value_int("browser_pad_x");

        }

        // -- show visuals button

        if (!sk->get_value_file("showvisuals_out",s_value1,dir));
        if (!sk->get_value_file("showvisuals_over",s_value2,dir));
        x = sk->get_value_int("showvisuals_x");
        y = sk->get_value_int("showvisuals_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_VISUALS] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_VISUALS]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("showvisuals_text",s_value1)) buttons[BTN_VISUALS]->set_text(gui_gettext(s_value1));
            buttons[BTN_VISUALS]->text_color = sk->get_value_color("showvisuals_text_color");
            buttons[BTN_VISUALS]->text_color_over = sk->get_value_color("showvisuals_text_color_over");
            buttons[BTN_VISUALS]->font_sz = sk->get_value_int("showvisuals_text_size");
            buttons[BTN_VISUALS]->pad_y = sk->get_value_int("showvisuals_pad_y");
            buttons[BTN_VISUALS]->pad_x = sk->get_value_int("showvisuals_pad_x");

        }

        // -- show search button

        if (!sk->get_value_file("search_out",s_value1,dir));
        if (!sk->get_value_file("search_over",s_value2,dir));
        x = sk->get_value_int("search_x");
        y = sk->get_value_int("search_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_SEARCH] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_SEARCH]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("search_text",s_value1)) buttons[BTN_SEARCH]->set_text(gui_gettext(s_value1));
            buttons[BTN_SEARCH]->text_color = sk->get_value_color("search_text_color");
            buttons[BTN_SEARCH]->text_color_over = sk->get_value_color("search_text_color_over");
            buttons[BTN_SEARCH]->font_sz = sk->get_value_int("search_text_size");
            buttons[BTN_SEARCH]->pad_y = sk->get_value_int("search_pad_y");
            buttons[BTN_SEARCH]->pad_x = sk->get_value_int("search_pad_x");

        }


        // -- optional - next skin button

        if (!sk->get_value_file("nextskin_out",s_value1,dir));
        if (!sk->get_value_file("nextskin_over",s_value2,dir));
        x = sk->get_value_int("nextskin_x");
        y = sk->get_value_int("nextskin_y");
        if (*s_value1 && *s_value2)
        {
            buttons[BTN_NX_SKIN] = new gui_button(guibuffer,f,x,y,NULL,0,false);
            buttons[BTN_NX_SKIN]->set_images(s_value1,s_value2,0,0);
            if (sk->get_value_string("nextskin_text",s_value1)) buttons[BTN_NX_SKIN]->set_text(gui_gettext(s_value1));
            buttons[BTN_NX_SKIN]->text_color = sk->get_value_color("nextskin_text_color");
            buttons[BTN_NX_SKIN]->text_color_over = sk->get_value_color("nextskin_text_color_over");
            buttons[BTN_NX_SKIN]->font_sz = sk->get_value_int("nextskin_text_size");
            buttons[BTN_NX_SKIN]->pad_y = sk->get_value_int("nextskin_pad_y");
            buttons[BTN_NX_SKIN]->pad_x = sk->get_value_int("nextskin_pad_x");

        }


        // --- other stuff

        info_text_x = sk->get_value_int("info_txt_x");
        info_text_y = sk->get_value_int("info_txt_y");
        info_text_size = sk->get_value_int("info_txt_size");
        info_cancel_x = sk->get_value_int("info_cancel_x");
        info_cancel_y = sk->get_value_int("info_cancel_y");

        // -- general text items
        loopi(TXT_MAX)
        {
            char txt_tmp[SMALL_MEM] = {0};

            sprintf(txt_tmp,"txt%d_x",i+1);
            x = sk->get_value_int(txt_tmp);
            sprintf(txt_tmp,"txt%d_y",i+1);
            y = sk->get_value_int(txt_tmp);

            sprintf(txt_tmp,"txt%d_text",i+1);
            sk->get_value_string(txt_tmp,s_value1);

            if (*s_value1)
            {
                text_areas[i] = new gui_textbox(guibuffer,f,x,y,NULL,0,false);
                text_areas[i]->set_text(gui_gettext(s_value1));
                sprintf(txt_tmp,"txt%d_font_color",i+1);
                text_areas[i]->text_color = sk->get_value_color(txt_tmp);
                sprintf(txt_tmp,"txt%d_font_size",i+1);
                text_areas[i]->font_sz = sk->get_value_int(txt_tmp);
            }
        }




    };

    ~gui()
    {
        loopi(TXT_MAX)
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
        handle_options();

        if (visualize /*|| ((g_screen_status != S_OPTIONS))*/) return 0;

        loopi(BTN_MAX) {
            if(buttons[i]) buttons[i]->obj_state = B_OUT; //reset
        }
        loopi(max_listings)
        {
            buttons_delete[i]->obj_state = B_OUT; //reset
            buttons_genre[i]->obj_state = B_OUT; //reset
            buttons_playlists[i]->obj_state = B_OUT; //reset
            buttons_listing[i]->obj_state = B_OUT; //reset

        }

        if(g_screen_status == S_OPTIONS)
        {
             guis[GUI_OPTIONS]->handle_events(events);
             return 0;
        }
        if(g_screen_status == S_LOG)
        {
             return guis[GUI_LOG]->handle_events(events);
        }

        if (S_SEARCHGENRE == g_screen_status)
        {
            return guis[GUI_SEARCH]->handle_events(events);
        }


        if (g_screen_status == S_SEARCHING) return 0; // no hit test

        // -- cancel buffering
        if (status == BUFFERING)
        {
            if(buttons[BTN_CANCEL]->hit_test(events,1)==B_CLICK)
            {
                status = STOPPED;
            }

            return 0;
        }


        //loop through z-order
        int obj_state = 0;
        bloopj(MAX_Z_ORDERS)
        {

            //logo

#ifdef LOG_ENABLED
            if (buttons[BTN_LOGO]->hit_test(events,j)==B_CLICK) g_screen_status = S_LOG;
#else
            if (buttons[BTN_LOGO]->hit_test(events,j)==B_CLICK) g_screen_status = S_OPTIONS;
#endif

            //playing area
             if(buttons[BTN_PLAYING]->hit_test(events,j)==B_CLICK) {
                buttons[BTN_PLAYING]->scroll_reset();
                return 0;
             }
            // genre select
            if( buttons[BTN_GENRES_SELECT]->hit_test(events,j)==B_CLICK) {
                g_screen_status = S_GENRES;
                reset_scrollings();
                return 0;
            }
            // playlist select
            if(buttons[BTN_PLAYLISTS]->hit_test(events,j)==B_CLICK) {
                g_screen_status = S_PLAYLISTS;
                reset_scrollings();
                return 0;
            }

            // screen buttons
            loopi(max_listings) {
                if (g_screen_status == S_BROWSER) {

                    if ((obj_state = buttons_listing[i]->hit_test(events,j)))
                    {
                        if (obj_state == B_CLICK)
                        {
                            reset_scrollings();
                            connect_to_stream(i,I_STATION);
                        }

                        return 0;
                    }
                }

                if (g_screen_status == S_PLAYLISTS)
                {

                    if ((obj_state = buttons_delete[i]->hit_test(events,j)))
                    {
                        if (obj_state == B_CLICK)
                        {
                            int list = i + pls_display;
                            if (list >= total_num_playlists)
                                return 0;

                            delete_playlist(list);
                            reset_scrollings();
                        }
                        return 0;
                    }

                    if ((obj_state = buttons_playlists[i]->hit_test(events,j)))
                    {
                        if (obj_state == B_CLICK)
                        {
                            int connect = i + pls_display;
                            if (connect>= total_num_playlists || connect < 0)
                                return 0;

                            connect_to_stream(connect,I_PLAYLIST);
                            reset_scrollings();
                        }

                        return 0;
                    }
                }

                if (g_screen_status == S_GENRES) {
                    if (buttons_genre[i]->hit_test(events,j)==B_CLICK) {
                        genre_selected = i;
                        genre_selected += genre_display;

                        if (genre_selected >= gl.total || genre_selected < 0)
                            return 0;

                        char* g = gl.get_genre(genre_selected);
                        if (!g) return 0;


                        search_genre(g); // do the search .. switch to browser
                        reset_scrollings();
                        return 0;
                    }
                }

            }

            if (buttons[BTN_ADD]) // -- optional button
            {
                if (buttons[BTN_ADD]->hit_test(events,j)==B_CLICK) {
                    if (status == PLAYING)
                        request_save_fav(); // save the pls

                    return 0;
                }
            }
            if (buttons[BTN_STOP]) // -- optional button
            {
                if (buttons[BTN_STOP]->hit_test(events,j)==B_CLICK)
                {
                    status = STOPPED;
                    return 0;
                }
            }

            if (buttons[BTN_EXIT]) // -- optional button
            {
                if (buttons[BTN_EXIT]->hit_test(events,j)==B_CLICK)
                {
                    g_running = false;
                    return 0;
                }
            }

            if (buttons[BTN_BROWSER]) // -- optional button
            {
                if (buttons[BTN_BROWSER]->hit_test(events,j)==B_CLICK)
                {
                    g_screen_status = S_BROWSER;
                    return 0;
                }
            }

            if (buttons[BTN_VISUALS]) // -- optional button
            {
                if (buttons[BTN_VISUALS]->hit_test(events,j)==B_CLICK)
                {
                    visualize ? visualize = false : visualize = true;
                    return 0;
                }
            }

            if (buttons[BTN_SEARCH]) // -- optional button
            {
                if (buttons[BTN_SEARCH]->hit_test(events,j)==B_CLICK)
                {
                    g_screen_status = S_SEARCHGENRE;
                    return 0;
                }
            }

            if (buttons[BTN_NX_SKIN]) // -- optional button
            {
                if (buttons[BTN_NX_SKIN]->hit_test(events,j)==B_CLICK)
                {
                    next_skin();
                    return 0;
                }
            }

             if (buttons[BTN_NEXT]->hit_test(events,j)==B_CLICK) {
                if (g_screen_status == S_BROWSER) {
                    char* g = gl.get_genre(genre_selected);
                    genre_nex_prev(true,g);//(char*)genres[genre_selected]);

                    reset_scrollings();

                    return 0;
                }

                if (g_screen_status == S_GENRES) {

                  //  if (genre_display + max_listings  >= MAX_GENRE) return 0;
                  //  else genre_display += max_listings;
                    if (genre_display + max_listings  >= gl.total) return 0;
                    else genre_display += max_listings;

                    reset_scrollings();
                    return 0;
                }

                if (g_screen_status == S_PLAYLISTS) {

                    if (pls_display + max_listings  >= total_num_playlists) return 0;
                    else pls_display += max_listings;

                    reset_scrollings();
                    return 0;
                }

             }

            if(buttons[BTN_PRIOR]->hit_test(events,j)==B_CLICK) {
                if (g_screen_status == S_BROWSER) {
                    char* g = gl.get_genre(genre_selected);
                    if (display_idx>1) genre_nex_prev(false,g);
                    //if (display_idx>1) genre_nex_prev(false,(char*)genres[genre_selected]);

                    reset_scrollings();
                }

                if (g_screen_status == S_PLAYLISTS) {
                    pls_display -= max_listings;
                    if (pls_display < 0) {
                        pls_display = 0;
                    }

                    reset_scrollings();

                }

                 if (g_screen_status == S_GENRES) {
                    genre_display -= max_listings;
                    if (genre_display < 0) {
                        genre_display = 0;
                    }

                    reset_scrollings();
                 }

                 return 0;
            }
        } //z-order loop


        return -1;

    };

    void draw_favs(fav_item* lst)
    {

        int i = 0;
        int p = 0;
        fav_item* cl = lst;

        while(i<max_listings && cl != 0)
        {
            if (i==max_listings) break;
            if (p >= pls_display) {
                buttons_playlists[i]->set_text((cl==0 ? (char*)"" : cl->station_name));
                buttons_playlists[i]->draw();
                // delete option
                buttons_delete[i]->draw();

                i++;
            }

            if(cl) cl = cl->nextnode;
            p++;

        }
    }

    unsigned long visual_show_title;
    char visual_last_track_title[SMALL_MEM];
    void draw(station_list* current_list, icy* ic, favorites* favs)
    {
         draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear the buffer

        if (visualize)
        {
            // TO DO ... add more
            black_screen_saver();
            if (visualize_number < MAX_VISUALS) {
                vis->user_data = ic->track_title;
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

        }else{ //if (g_screen_status != S_OPTIONS){


            SDL_BlitSurface(guibackground,0, guibuffer,0); //background
            buttons[BTN_LOGO]->draw(); // title

            if (g_screen_status != S_OPTIONS && g_screen_status != S_SEARCHGENRE &&
                g_screen_status != S_LOG)
            {
                if(vis_on_screen)
                {
                    render_gui_bars();
                }
                if (status != STOPPED && status != FAILED)
                {
                    buttons[BTN_PLAYING]->auto_scroll_text = true;
                    buttons[BTN_PLAYING]->set_text(ic->track_title);
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

                if (g_screen_status == S_BROWSER) {

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

                }else if(g_screen_status == S_PLAYLISTS) {
                    draw_favs(favs->first);
                }else if (g_screen_status == S_GENRES) {

                    loopi(max_listings) {

                        //if (i+genre_display < MAX_GENRE) {
                        if (i+genre_display < gl.total)
                        {
                            char* g = gl.get_genre(i+genre_display);
                            if (g) buttons_genre[i]->set_text(g);
                            buttons_genre[i]->draw();
                        }

                    }
                }

                if (g_screen_status != S_STREAM_INFO) {
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
                    //genre select
                    buttons[BTN_GENRES_SELECT]->draw();
                    // playlists
                    buttons[BTN_PLAYLISTS]->draw();
                }

                if (g_screen_status == S_STREAM_INFO)
                {
                    draw_stream_details(ic);
                }

            }
        }

        if (g_screen_status == S_SEARCHGENRE) guis[GUI_SEARCH]->draw();
        if (g_screen_status == S_OPTIONS) draw_about();
        if (g_screen_status == S_LOG) draw_log();


        // always inform user if buffering
        if (status == BUFFERING) draw_info(vars.search_var("$LANG_TXT_BUFFERING"));
        if (status == CONNECTING) draw_info(vars.search_var("$LANG_TXT_CONNECTING"));
        if (g_screen_status == S_SEARCHING) draw_info(vars.search_var("$LANG_TXT_SEARCHING"));

        if (!visualize) {
            visual_show_title = get_tick_count();
            strcpy(visual_last_track_title, ic->track_title);
        }

        // volume display ... like an OSD
        draw_volume();

    };

    // -- different mapping ?
    bool override_keys()
    {
        if (visualize) return vis->remap_keys;

        return false;
    };

    void draw_info(char* txt)
    {
        if (!txt) return;

        fnts->set_size(info_text_size);
        fnts->change_color((dialog_text_color >> 16), ((dialog_text_color >> 8) & 0xff),(dialog_text_color & 0xff));
        fade(guibuffer,SDL_MapRGB(guibuffer->format,0,0,0),100);
        SDL_Rect t = {(SCREEN_WIDTH/2)-(info_dlg->w / 2),(440 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(info_dlg,0, guibuffer,&t);
        fnts->text(guibuffer,txt,t.x + info_text_x,t.y + info_text_y,0);

        if (status == BUFFERING || status == CONNECTING)
        {
            fnts->set_size(FS_MED);
            buttons[BTN_CANCEL]->s_x = t.x + info_cancel_x;
            buttons[BTN_CANCEL]->s_y = t.y + info_cancel_y;
            buttons[BTN_CANCEL]->draw();
        }

    };
    void draw_sc_error()
    {
        fnts->change_color((dialog_text_color >> 16), ((dialog_text_color >> 8) & 0xff),(dialog_text_color & 0xff));
        SDL_Rect t = {48,(400 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(dialog,0, guibuffer,&t);

        fnts->text(guibuffer,vars.search_var("$LANG_TXT_SC_ERROR_l1"),t.x + 94,t.y + 45,0);
        fnts->text(guibuffer,vars.search_var("$LANG_TXT_SC_ERROR_l2"),t.x + 94,t.y + 72,0);
    }

    void draw_about()
    {
        guis[GUI_OPTIONS]->draw();
    };

    void draw_log()
    {
        guis[GUI_LOG]->draw();
    };

    char *logtext()
    {
        return guis[GUI_LOG]->text;
    };

    void draw_stream_details(icy* ic)
    {
        fnts->change_color((dialog_text_color >> 16), ((dialog_text_color >> 8) & 0xff),(dialog_text_color & 0xff));
        fnts->set_size(FS_SMALL);
        fade(guibuffer,SDL_MapRGB(guibuffer->format,0,0,0),100);

        SDL_Rect t = {(SCREEN_WIDTH / 2) - (dialog->w / 2),(SCREEN_HEIGHT / 2) - (dialog->h / 2),dialog->w,dialog->h};

        SDL_BlitSurface(dialog,0, guibuffer,&t);
        char* lang = vars.search_var("$LANG_TXT_STATION");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s", lang,trim_string(ic->icy_name,50)),t.x + 35,t.y + 38,0);
        lang = vars.search_var("$LANG_TXT_URL");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s",lang,trim_string(ic->icy_url,50)),t.x + 35,t.y + 58,0);
        lang = vars.search_var("$LANG_TXT_BR");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %dKbps",lang,ic->icy_br),t.x + 35,t.y + 78,0);
        lang = vars.search_var("$LANG_TXT_TITLE");
        if (lang) fnts->text(guibuffer,make_string((char*)"%s: %s",lang,trim_string(ic->track_title,50)),t.x + 35,t.y + 98,0);

    };

    void draw_volume()
    {
        if (mute && !screen_sleeping) // alway show if muted
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
            int pc =  (int)(((float)mp3_volume / 255.0) * (float)vol_ol->w);

            pc > vol_ol->w ? pc = vol_ol->w : pc < 0 ? pc = 0 : 0; // clip

            // overlay
            r.x += 18;
            SDL_Rect s = { 0,0, pc,vol_ol->h };
            SDL_BlitSurface( vol_ol,&s, guibuffer,&r);
        }
    };

    void draw_cursor(int x,int y, float angle)
    {
        SDL_Rect r = { x,y,cursor->w,cursor->h };
      /*
         -- need to also adjust hotspot angle as the hit tests are off when rotated
        SDL_Surface *rot = rotozoomSurface (cursor, angle, 1, 0);
        if(!rot) return; // don't count on rotozoomSurface to always allocate surface
        SDL_BlitSurface(rot,0, guibuffer, &r);
        SDL_FreeSurface(rot); // free
        */
        SDL_BlitSurface(cursor,0, guibuffer, &r);
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

    void black_screen_saver()
    {
        draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    };

    // -- for on screen bars
    void render_gui_bars()
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
