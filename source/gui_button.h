#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_



#define MAX_GENRE (19)
const char* genres[] = {
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
    bool            can_scroll;

    gui_button(SDL_Surface* g,fonts* f,int x,int y,int w,int h,char* t,long tc,bool scroll) :
    s_x(x), s_y(y), s_h(h),s_w(w),
    text_color(tc), text_l1(t), pad_x(30), pad_y(12), btn_state(B_OUT),
    bnt_down(0), bnt_out(0), bnt_over(0), guibuffer(g), scroll_area(0),
    use_images(false), center_text(true), fnts(f), limit_text(0), scroll_x(0),
    auto_scroll_text(false), font_sz(FS_LARGE), z_order(0), bind_screen(0),
    scroll_speed(3), can_scroll(true)

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

    void set_text(const char* t)
    {
        if(!t) return;
        else text_l1 = (char*)t;

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

    void set_images(const char* over, const char* out, const char* down)
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

            if (scroll_area && can_scroll) {
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

#endif
