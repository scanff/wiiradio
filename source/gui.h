#ifndef _GUI_H_
#define _GUI_H_


#define MAX_Z_ORDERS (3)
#define MAX_GENRE (19)
char* genres[] = {
  "all","ambient","country","classical",
  "dance","indie","jazz","metal","pop","rave",
  "rap","rock","techno","trance","90s","80s","70s","60s",
  "50s"
};


enum _button_states {
    B_OUT = 0,
    B_OVER,
    B_CLICK
};

#define X_OFFSET 17 // The pointer's hotspot is 17px from the left
#define Y_OFFSET 10

//simple button class
class gui_button {
    public:

    int s_x;
    int s_y;
    int s_h;
    int s_w;

    long text_color;
    char* text_l1;

    int pad_x;
    int pad_y;
    int btn_state;



    SDL_Surface*    bnt_down;
    SDL_Surface*    bnt_out;
    SDL_Surface*    bnt_over;
    SDL_Surface*    guibuffer;
    SDL_Surface*    scroll_area;

    bool            use_images;
    bool            center_text;
    fonts*          fnts;
    int             limit_text;
    int             scroll_x;
    bool            auto_scroll_text;
    int             font_sz;
    int             z_order;    // zorder ... max of 3
    int             bind_screen; // what screen is this button bound to

    int             scroll_speed;

    gui_button(SDL_Surface* g,fonts* f,int x,int y,int w,int h,char* t,long tc,bool scroll) :
    s_x(x), s_y(y), s_h(h),s_w(w),
    text_color(tc), text_l1(t), pad_x(30), pad_y(12), btn_state(B_OUT),
    bnt_down(0), bnt_out(0), bnt_over(0), guibuffer(g), scroll_area(0),
    use_images(false), center_text(true), fnts(f), limit_text(0), scroll_x(0),
    auto_scroll_text(false), font_sz(FS_LARGE), z_order(0), bind_screen(0),
    scroll_speed(3)

    {

        Uint32 rmask, gmask, bmask, amask;

//for 24bit

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0x00000000;
#endif


        if (scroll) {
            scroll_area = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,44,BITDEPTH,
                                         rmask, gmask, bmask,amask);

           SDL_SetColorKey(scroll_area,SDL_SRCCOLORKEY, SDL_MapRGB(scroll_area->format,200,200,200));


        }

    }

    ~gui_button()
    {
        if (scroll_area) SDL_FreeSurface(scroll_area);
    };

    void set_text(char* t)
    {
        if(!t) return;
        else text_l1 = t;

    };

    void scroll_reset() { scroll_x = 0; };

    void scroll_text()
    {
        draw_rect(scroll_area,0,0,SCREEN_WIDTH,45,0xC8C8C8);
        fnts->text(scroll_area,text_l1,scroll_x,0,0);

        if (btn_state == B_OVER || auto_scroll_text) scroll_x-=scroll_speed; //scroll speed

        SDL_Rect ds = {s_x+pad_x,s_y+pad_y,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_Rect sr = {0,0,( limit_text ? limit_text : scroll_area->w ),scroll_area->h};
        SDL_BlitSurface( scroll_area,&sr , guibuffer,&ds );

        int fontwidth = fnts->get_length_px(text_l1,font_sz);//this needs to be returned from fonts->text
        if (scroll_x < -(fontwidth)) scroll_x = limit_text;

    };

    void set_images(char* over, char* out,char* down)
    {
        bnt_out = tx->texture_lookup(out);
        bnt_over = tx->texture_lookup(over);
        bnt_down = tx->texture_lookup(over);

        use_images = true;

        //re-calculate the hit area based from the images
        s_w = bnt_out->w;
        s_h = bnt_out->h;
    };

    void draw()
    {

        if(bnt_out && btn_state == B_OUT){
            SDL_Rect ds = {s_x,s_y,bnt_out->w,bnt_out->h};
            SDL_BlitSurface( bnt_out,0, guibuffer,&ds );
        }else if (bnt_over && btn_state == B_OVER){
            SDL_Rect ds = {s_x,s_y,bnt_over->w,bnt_over->h};
            SDL_BlitSurface( bnt_over,0, guibuffer,&ds );
        }else if (bnt_down && btn_state == B_CLICK){
            SDL_Rect ds = {s_x,s_y,bnt_down->w,bnt_down->h};
            SDL_BlitSurface( bnt_down,0, guibuffer,&ds );
        }

        if (text_l1) {
            fnts->set_size(font_sz);

            if (scroll_area ) {
                scroll_text();

            }else{
                if (center_text) {
                    int text_len = fnts->get_length_px(text_l1,font_sz);
                    int cx = 0;
                    text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                    fnts->text(guibuffer,text_l1,cx+s_x,s_y+pad_y,limit_text);
                }else{
                    fnts->text(guibuffer,text_l1,s_x+pad_x,s_y+pad_y,limit_text);
                }
            }
        }

    };

    bool test_zorder(int c)
    {
        return (c == z_order ? true : false);
    };

    int mouse_over(int x, int y,int current_z)
    {
        if (!test_zorder(current_z))
            return B_OUT;

        x+=X_OFFSET;
        y+=Y_OFFSET;

        if (x > s_x && x < (s_x+s_w) && y > s_y && y < (s_y+s_h)) btn_state = B_OVER;
        else btn_state = B_OUT;

        return btn_state;
    }

    int hit_test(int x, int y, int current_z)
    {
        if (!test_zorder(current_z))
            return B_OUT;

        x+=X_OFFSET;
        y+=Y_OFFSET;

        if ((x > s_x && x < (s_x+s_w) && y > s_y && y < (s_y+s_h))) btn_state = B_CLICK;
        else btn_state = B_OUT;

        return btn_state;
    };
};

class gui {
    public:

    enum _gui_buttons {

        BTN_STATION_1 = 0,
        BTN_STATION_2,
        BTN_STATION_3,
        BTN_STATION_4,
       // BTN_STATION_5,

        BTN_FAVS_1,
        BTN_FAVS_2,
        BTN_FAVS_3,
        BTN_FAVS_4,
        //BTN_FAVS_5,

        BTN_FAV_DEL_1,
        BTN_FAV_DEL_2,
        BTN_FAV_DEL_3,
        BTN_FAV_DEL_4,
        //BTN_FAV_DEL_5,

        BTN_GENRES_1,
        BTN_GENRES_2,
        BTN_GENRES_3,
        BTN_GENRES_4,
        //BTN_GENRES_5,

        BTN_NEXT,
        BTN_PRIOR,
        BTN_ADD,
        BTN_PLAYLISTS,
        BTN_GENRES_SELECT,
        BTN_PLAYING,
        BTN_QUIT,
        BTN_RETURN,
        BTN_LOGO,

        BTN_MAX

    };


    fonts*          fnts;
    fft*            vis;

    gui_button*     buttons[BTN_MAX];

    int             select_pos;
    int             row_height;
    int             start_y;

    SDL_Surface*    cursor;
    SDL_Surface*    guibuffer;
    SDL_Surface*    guibackground;
    SDL_Surface*    info_dlg;
    SDL_Surface*    vol_bg;
    SDL_Surface*    vol_ol;
    SDL_Surface*    dialog;


    int             genre_selected;

    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    gui(fonts* f,fft* v) : fnts(f), vis(v), select_pos(0), row_height(30), start_y(70), genre_selected(0)
    {

        loopi(MAX_FFT_RES) {
            fft_results[i] = 0;
            peakResults[i] = 0;
        }

        Uint32 rmask, gmask, bmask, amask;
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


        loopi(BTN_MAX) buttons[i] = 0; // NULL

        guibackground = tx->texture_lookup("imgs/bg.png");
        cursor = tx->texture_lookup("imgs/cursor.png");

        // --- info
        info_dlg = tx->texture_lookup("imgs/info.png");

        // -- volume overlay
        vol_bg = tx->texture_lookup("imgs/volume_bg.png");
        vol_ol = tx->texture_lookup("imgs/volume_ol.png");

        // --- Diaglog bg
        dialog = tx->texture_lookup("imgs/dialog.png");
        // ----- bottom of screen

        //more
        buttons[BTN_NEXT] = new gui_button(guibuffer,f,344,390,82,51,NULL,0,false);
        buttons[BTN_NEXT]->set_images("imgs/right_over.png","imgs/right_out.png","imgs/right_over.png");

        //back
        buttons[BTN_PRIOR] = new gui_button(guibuffer,f,200,390,82,51,NULL,0,false);
        buttons[BTN_PRIOR]->set_images("imgs/left_over.png","imgs/left_out.png","imgs/left_over.png");


        buttons[BTN_ADD] = new gui_button(guibuffer,f,287,390,54,52,NULL,0,false);
        buttons[BTN_ADD]->set_images("imgs/plus_over.png","imgs/plus_out.png","imgs/plus_over.png");

        //genre sel
        buttons[BTN_GENRES_SELECT] = new gui_button(guibuffer,f,437,390,163,52,NULL,0,false);
        buttons[BTN_GENRES_SELECT]->set_images("imgs/genre_over.png","imgs/genre_out.png","imgs/genre_over.png");

        //playlists sel
        buttons[BTN_PLAYLISTS] = new gui_button(guibuffer,f,28,390,163,52,NULL,0,false);
        buttons[BTN_PLAYLISTS]->set_images("imgs/playlists_over.png","imgs/playlists_out.png","imgs/playlists_over.png");

        int height, width, y_offset, x_offset;

        loopi(MAX_BUTTONS)
        {
            height = 66;
            width = 589;
            x_offset = 20;
            y_offset = 115;

            // stations
            buttons[i+BTN_STATION_1] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,width/*415*/,height/*36*/,NULL,0x3c1080,true);
            buttons[i+BTN_STATION_1]->set_images("imgs/listing_over.png","imgs/listing_out.png","imgs/listing_over.png");
            buttons[i+BTN_STATION_1]->center_text = false;
            buttons[i+BTN_STATION_1]->limit_text = 530;
            buttons[i+BTN_STATION_1]->scroll_speed = 5;
            buttons[i+BTN_STATION_1]->bind_screen = S_BROWSER;
            //favs
            buttons[i+BTN_FAVS_1] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,width,height,NULL,0x3c1080,true);
            buttons[i+BTN_FAVS_1]->set_images("imgs/listing_over.png","imgs/listing_out.png","imgs/listing_over.png");
            buttons[i+BTN_FAVS_1]->center_text = false;
            buttons[i+BTN_FAVS_1]->limit_text = 530;
            buttons[i+BTN_FAVS_1]->bind_screen = S_PLAYLISTS;
            buttons[i+BTN_FAVS_1]->scroll_speed = 5;
            //genres
            buttons[BTN_GENRES_1+i] = new gui_button(guibuffer,f,x_offset,y_offset+i*height,width,height,NULL,0x3c1080,false);
            buttons[BTN_GENRES_1+i]->set_images("imgs/listing_over.png","imgs/listing_out.png","imgs/listing_over.png");
            buttons[BTN_GENRES_1+i]->limit_text = 530;
            buttons[i+BTN_GENRES_1]->bind_screen = S_GENRES;

            //delete buttons
            x_offset = 530;
            width = 63;

            buttons[i+BTN_FAV_DEL_1] = new gui_button(guibuffer,f,x_offset,y_offset+i*height+2,width,height,NULL,0,false);
            buttons[i+BTN_FAV_DEL_1]->set_images("imgs/delete_over.png","imgs/delete_out.png","imgs/delete_over.png");
            buttons[i+BTN_FAV_DEL_1]->z_order = 1;
            buttons[i+BTN_FAV_DEL_1]->bind_screen = S_PLAYLISTS;

        }

        //playing area
        buttons[BTN_PLAYING] = new gui_button(guibuffer,f,259,45,340,48,NULL,0,true);
        buttons[BTN_PLAYING]->set_images("imgs/playing.png","imgs/playing.png","imgs/playing.png");
        buttons[BTN_PLAYING]->limit_text = 290;
        buttons[BTN_PLAYING]->auto_scroll_text = true;
        buttons[BTN_PLAYING]->pad_y = 5;
        buttons[BTN_PLAYING]->font_sz = FS_MED;

        //quit
        buttons[BTN_QUIT] = new gui_button(guibuffer,f,385,335,212,68,0,0,false);
        buttons[BTN_QUIT]->set_images("imgs/exit_over.png","imgs/exit_out.png","imgs/exit_over.png");
        buttons[BTN_QUIT]->bind_screen = S_ABOUT;

        //return
        buttons[BTN_RETURN] = new gui_button(guibuffer,f,48,335,212,68,0,0,false);
        buttons[BTN_RETURN]->set_images("imgs/return_over.png","imgs/return_out.png","imgs/return_over.png");
        buttons[BTN_RETURN]->bind_screen = S_ABOUT;

        // logo
        buttons[BTN_LOGO] = new gui_button(guibuffer,f,0,25,196,88,NULL,0,false);
        buttons[BTN_LOGO]->set_images("imgs/logo_over.png","imgs/logo_out.png","imgs/logo_over.png");

    };

    ~gui()
    {
        loopi(BTN_MAX) {
            delete buttons[i];
            buttons[i] = 0;
        }
    };

    void reset_scrollings()
    {
        loopj(MAX_BUTTONS) {
            buttons[j+BTN_STATION_1]->scroll_reset();
            buttons[j+BTN_FAVS_1]->scroll_reset();
        }
    };



    int handle_events(SDL_Event* events)
    {
        if (visualize || (status == BUFFERING && (g_screen_status != S_ABOUT))) return 0;

        loopi(BTN_MAX) buttons[i]->btn_state = B_OUT; //reset

        //over
        if (events->type != SDL_MOUSEBUTTONDOWN)
        {
            bloopj(MAX_Z_ORDERS) // top ordered first
            {
                loopi(BTN_MAX)
                {
                    if (buttons[i]->mouse_over(events->motion.x,events->motion.y,j) == B_OVER
                        && ((buttons[i]->bind_screen == g_screen_status) || (buttons[i]->bind_screen == S_ALL))
                        )
                        return 0; // found it, don't continue

                }
            }
        }

        //click
        if(events->type == SDL_MOUSEBUTTONDOWN)
        {

            //loop through z-order
            bloopj(MAX_Z_ORDERS)
            {

                 if(g_screen_status == S_ABOUT)
                 {
                     if(buttons[BTN_QUIT]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                        g_running = false;
                        return 0;
                     }

                      if(buttons[BTN_RETURN]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                        g_screen_status = S_BROWSER;
                        return 0;
                     }
                 }

                //logo

                if (buttons[BTN_LOGO]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) g_screen_status = S_ABOUT;

                //playing area
                 if(buttons[BTN_PLAYING]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    buttons[BTN_PLAYING]->scroll_reset();
                    return 0;
                 }
                // genre select
                if( buttons[BTN_GENRES_SELECT]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    menu_button_click(S_GENRES);
                    reset_scrollings();
                    return 0;
                }
                // playlist select
                if(buttons[BTN_PLAYLISTS]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    menu_button_click(S_PLAYLISTS);
                    reset_scrollings();
                    return 0;
                }

                // screen buttons
                loopi(MAX_BUTTONS) {
                    if (g_screen_status == S_BROWSER) {
                        if (buttons[i+BTN_STATION_1]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                            reset_scrollings();
                            connect_to_stream(i,false);
                            return 0;
                        }
                    }

                    if (g_screen_status == S_PLAYLISTS) {

                        if (buttons[i+BTN_FAV_DEL_1]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                            int connect = i + pls_display;
                            if (connect>= total_num_playlists)
                                return 0;

                            delete_playlist(connect);
                            reset_scrollings();
                            return 0;
                        }

                        if (buttons[i+BTN_FAVS_1]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                            int connect = i + pls_display;
                            if (connect>= total_num_playlists)
                                return 0;

                            connect_to_stream(connect,true);
                            reset_scrollings();
                            return 0;
                        }
                    }

                    if (g_screen_status == S_GENRES) {
                        if (buttons[i+BTN_GENRES_1]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                            genre_selected = i;
                            genre_selected += genre_display;
                            if (genre_selected >= MAX_GENRE || genre_selected < 0)
                                return 0;


                            search_genre((char*)genres[genre_selected]); // do the search .. switch to browser

                            g_screen_status = S_BROWSER;
                            reset_scrollings();
                            return 0;
                        }
                    }

                }


                if (buttons[BTN_ADD]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    if (status == PLAYING)
                        request_save_fav(); // save the pls

                    return 0;
                }

                 if (buttons[BTN_NEXT]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    if (g_screen_status == S_BROWSER) {
                        genre_nex_prev(true,(char*)genres[genre_selected]);

                        reset_scrollings();

                        return 0;
                    }

                    if (g_screen_status == S_GENRES) {

                        if (genre_display + MAX_BUTTONS  >= MAX_GENRE) return 0;
                        else genre_display += MAX_BUTTONS;

                        reset_scrollings();
                        return 0;
                    }

                    if (g_screen_status == S_PLAYLISTS) {

                        if (pls_display + MAX_BUTTONS  >= total_num_playlists) return 0;
                        else pls_display += MAX_BUTTONS;

                        reset_scrollings();
                        return 0;
                    }

                 }

                if(buttons[BTN_PRIOR]->hit_test(events->motion.x,events->motion.y,j)==B_CLICK) {
                    if (g_screen_status == S_BROWSER) {
                        if (display_idx>1) genre_nex_prev(false,(char*)genres[genre_selected]);

                        reset_scrollings();
                    }

                    if (g_screen_status == S_PLAYLISTS) {
                        pls_display -= MAX_BUTTONS;
                        if (pls_display < 0) {
                            pls_display = 0;
                        }

                        reset_scrollings();

                    }

                     if (g_screen_status == S_GENRES) {
                        genre_display -= MAX_BUTTONS;
                        if (genre_display < 0) {
                            genre_display = 0;
                        }

                        reset_scrollings();
                     }

                     return 0;
                }
            } //z-order loop

        }

        return -1;

    };

    void draw_favs(fav_item* lst)
    {

        int i = 0;
        int p = 0;
        fav_item* cl = lst;

        while(i<MAX_BUTTONS && cl != 0)
        {
            if (i==MAX_BUTTONS) break;
            if (p >= pls_display) {
                buttons[i+BTN_FAVS_1]->set_text((cl==0 ? (char*)"" : cl->station_name));
                buttons[i+BTN_FAVS_1]->draw();
                // delete option
                buttons[i+BTN_FAV_DEL_1]->draw();

                i++;
            }

            if(cl) cl = cl->nextnode;
            p++;

        }
    }

    void draw(station_list* current_list,icy* ic,favorites* favs)
    {
        if (visualize)
        {
            // TO DO ... add more
            black_screen_saver();
            if (visualize_number == 0) draw_visual();

        }else{

            fnts->change_color(60,60,60); // change for main font color

            SDL_BlitSurface(guibackground,0, guibuffer,0); //background

            // title
            buttons[BTN_LOGO]->draw();

            if (status != STOPPED && status != FAILED)
            {
                buttons[BTN_PLAYING]->auto_scroll_text = true;
                buttons[BTN_PLAYING]->set_text(ic->track_title);
            }else{
                buttons[BTN_PLAYING]->auto_scroll_text = true;
                char s_status[50] = {0};
                switch(status){
                    case 0: strcpy(s_status,"Stream Stopped");
                    break;
                    case 1: strcpy(s_status,"Stream Playing");
                    break;
                    case 2: strcpy(s_status,"Buffering Stream");
                    break;
                    case 3: strcpy(s_status,"Stream Error, Reconnect!");
                    break;
                }

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
                        if (i==MAX_BUTTONS) break;

                        if (p >= display_idx) {
                            buttons[i+BTN_STATION_1]->set_text(csl->station_name);
                            buttons[i+BTN_STATION_1]->draw();
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

                loopi(MAX_BUTTONS) {

                    if (i+genre_display < MAX_GENRE) {
                        buttons[i+BTN_GENRES_1]->set_text(genres[i+genre_display]);
                        buttons[i+BTN_GENRES_1]->draw();
                    }

                }

            }else if (g_screen_status == S_ABOUT) {

                draw_about();


            }else if (g_screen_status == S_OPTIONS) {
                // TO DO
            }
            if (    g_screen_status != S_ABOUT
                &&  g_screen_status != S_OPTIONS
                &&  g_screen_status != S_STREAM_INFO

                ) {
                //more / prior ... used for browser, playlist and genre selection
                buttons[BTN_NEXT]->draw();
                buttons[BTN_PRIOR]->draw();

                buttons[BTN_ADD]->draw();
                //genre select
                buttons[BTN_GENRES_SELECT]->draw();
                // playlists
                buttons[BTN_PLAYLISTS]->draw();
            }

            if (g_screen_status == S_STREAM_INFO)
            {
                draw_stream_details();
            }



            //cursor
            draw_cursor(event.motion.x,event.motion.y);
        }

        // always inform user if buffering
        if (status == BUFFERING) draw_info("Buffering");

        // volume display ... like an OSD
        draw_volume();

    };

    void draw_info(char* txt)
    {
        fnts->change_color(60,60,60);
        fade(guibuffer,SDL_MapRGB(guibuffer->format,0,0,0),100);
        SDL_Rect t = {48,(440 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(info_dlg,0, guibuffer,&t);
        fnts->text(guibuffer,txt,t.x + 130,t.y + 32,0);

    };
    void draw_sc_error()
    {
        fnts->change_color(60,60,60);
        SDL_Rect t = {48,(400 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(dialog,0, guibuffer,&t);
        fnts->text(guibuffer,"Sorry, Shoutcast is not responding!",t.x + 94,t.y + 45,0);
        fnts->text(guibuffer,"Please try again by selecting a genre.",t.x + 94,t.y + 72,0);
    }

    void draw_about()
    {
        fnts->change_color(60,60,60);
        fade(guibuffer,SDL_MapRGB(guibuffer->format,0,0,0),100);
        SDL_Rect t = {48,(400 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(dialog,0, guibuffer,&t);
        fnts->text(guibuffer,"WiiRadio, Version 0.3.",t.x + 134,t.y + 45,0);
        fnts->text(guibuffer,"By Scanff And TiMeBoMb",t.x + 134,t.y + 72,0);

        buttons[BTN_QUIT]->draw();
        buttons[BTN_RETURN]->draw();
    };

    void draw_stream_details()
    {
        fnts->change_color(60,60,60);
        fade(guibuffer,SDL_MapRGB(guibuffer->format,0,0,0),100);
        SDL_Rect t = {48,(400 / 2) - (info_dlg->h / 2),info_dlg->w,info_dlg->h};
        SDL_BlitSurface(dialog,0, guibuffer,&t);
        fnts->text(guibuffer,"Title:",t.x + 35,t.y + 38,0);
        fnts->text(guibuffer,"URL:",t.x + 35,t.y + 68,0);
        fnts->text(guibuffer,"Bit Rate:",t.x + 35,t.y + 98,0);

    };

    void draw_volume()
    {
        if (mute) // alway show if muted
        {
            draw_rect(guibuffer,10,45,95,55,0);
            fnts->change_color(200,60,60);
            fnts->set_size(FS_LARGE);
            fnts->text(guibuffer,"MUTE",20,50,0);
        }

        if((get_tick_count() - g_vol_lasttime) < 2000) // show for 2 secs after change
        {
            SDL_Rect r = { 80,(440 / 2) - (vol_bg->h / 2),vol_bg->w,vol_bg->h };
            // bg
            SDL_BlitSurface( vol_bg,0, guibuffer,&r);
            // actual volume bar
            int pc =  (int)(((float)mp3_volume / 255.0) * (float)vol_ol->w);

            pc > vol_ol->w ? pc = vol_ol->w : pc < 0 ? pc = 0 : 0; // clip

            // overlay
            SDL_Rect s = { 0,0, pc,vol_ol->h };
            SDL_BlitSurface( vol_ol,&s, guibuffer,&r);
        }
    };

    void draw_cursor(int x,int y)
    {
        SDL_Rect r = { x,y,cursor->w,cursor->h };
        SDL_BlitSurface( cursor,0, guibuffer,&r);
    };


    void draw_visual()
    {
        int x = 0;
        int y = SCREEN_HEIGHT - 50;
        int bar_height = 300;
        int bar_width = SCREEN_WIDTH / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);
        int local_fft[MAX_FFT_RES];

        loopi(MAX_FFT_RES) local_fft[i] = fft_results[i]; //could change as this is set via callback
        loopi(MAX_FFT_RES)
        {
            // peak
            peakResults[i] -= (32767 / bar_height) * 4;

            if(peakResults[i] < 0) peakResults[i] = 0;
            if(peakResults[i] < local_fft[i]) peakResults[i] = local_fft[i];

            int peaks_newy = (y - (int)(percent * (double)peakResults[i]));

            //fft
            int newY = (y - (int)(percent * (double)local_fft[i]));
            int h = abs(y - newY);



            //draw
            draw_rect(guibuffer,x+i*bar_width,newY,bar_width-4,h,0xcc0022 +  i*10 );
            draw_rect(guibuffer,x+i*bar_width,peaks_newy,bar_width-4,5,0xcc0022 + i*10 << 16 );
        }

        // oscilloscope
        int sx,sy;
        int leftzerolevel = (SCREEN_HEIGHT/2-10)/2;
        int rightzerolevel = SCREEN_HEIGHT/2+10+leftzerolevel;
        int len = 8192/4;
        int ii;
        double ts = static_cast<double>(SCREEN_WIDTH)/static_cast<double>(len/2);
        int max = 20;
        int min = 0;
        double dRangePerStep = 20;
        double timescale = leftzerolevel/dRangePerStep;
        sx = 0;
        sy = leftzerolevel;
		for(ii=0; ii<len-1; ii+=2)
		{
			x = (int)((double)ii*ts);
			y = (int)leftzerolevel - (double)((double)(vis->real[ii])*timescale);

		//	y *= percent;
			//dc.LineTo( x ,y );
            //aalineColor(guibuffer,sx,sy,sx+x,sy+y,0xffffffff);
            bresenham(x,y,sx,sy);
            sy = y;
            sx = x;
		}

		sx = 0;
		sy = rightzerolevel;

		for(ii=1; ii<len; ii+=2)
		{
			x=(int)(ii*ts);
			y = (int)((double)rightzerolevel - (short)(vis->real[ii])*timescale);
			//y *= percent;
			bresenham(sx,sy,x,y);
            sy = y;
            sx = x;
		}
    }

    void fade(SDL_Surface *screen2, Uint32 rgb, Uint8 a)
    {
        SDL_Surface *tmp=0;

        tmp=SDL_DisplayFormat(screen2);

        if (!tmp) return;

        SDL_FillRect(tmp,0,rgb);
        SDL_SetAlpha(tmp,SDL_SRCALPHA,a);
        SDL_BlitSurface(tmp,0,screen2,0);
        SDL_FreeSurface(tmp);
    };

    void black_screen_saver()
    {
        draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    };

    void bresenham(int x1, int y1, int x2, int y2)
    {

        int delta_x = abs(x2 - x1) << 1;
        int delta_y = abs(y2 - y1) << 1;

        // if x1 == x2 or y1 == y2, then it does not matter what we set here
        signed char ix = x2 > x1?1:-1;
        signed char iy = y2 > y1?1:-1;

        //plot(x1, y1);

        if (delta_x >= delta_y)
        {
            // error may go below zero
            int error = delta_y - (delta_x >> 1);

            while (x1 != x2)
            {
                if (error >= 0)
                {
                    if (error || (ix > 0))
                    {
                        y1 += iy;
                        error -= delta_x;
                    }
                    // else do nothing
                }
                // else do nothing

                x1 += ix;
                error += delta_y;
                pixelColor(guibuffer,x1,y1,0xffffffff);
                //plot(x1, y1);
            }
        }
        else
        {
            // error may go below zero
            int error = delta_x - (delta_y >> 1);

            while (y1 != y2)
            {
                if (error >= 0)
                {
                    if (error || (iy > 0))
                    {
                        x1 += ix;
                        error -= delta_y;
                    }
                    // else do nothing
                }
                // else do nothing

                y1 += iy;
                error += delta_x;

                pixelColor(guibuffer,x1,y1,0xffffffff);
                //plot(x1, y1);
            }
        }
    }



};
#endif//_GUI_H_
