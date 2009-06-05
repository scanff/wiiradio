#ifndef _GUI_TOGGLE_H_
#define _GUI_TOGGLE_H_

enum _toggle_button_states {
    B_ON = 0,
    B_OFF
};
//simple button class
class gui_toggle {
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

    SDL_Surface*    bnt_on;
    SDL_Surface*    bnt_off;

    SDL_Surface*    guibuffer;

    bool            use_images;
    bool            center_text;
    fonts*          fnts;
    int             limit_text;
    int             font_sz;
    int             z_order;    // zorder ... max of 3
    int             bind_screen; // what screen is this button bound to

    gui_toggle(SDL_Surface* g,fonts* f,int x,int y,int w,int h,char* t,long tc,bool scroll) :
    s_x(x), s_y(y), s_h(h),s_w(w),
    text_color(tc), text_l1(t), pad_x(30), pad_y(12), btn_state(B_OFF),
    bnt_on(0), bnt_off(0), guibuffer(g),
    use_images(false), center_text(true), fnts(f), limit_text(0),
    font_sz(FS_LARGE), z_order(0), bind_screen(0)
    {



    }

    ~gui_toggle()
    {
    };

    void set_text(const char* t)
    {
        if(!t) return;
        else text_l1 = (char*)t;

    };

    // need over / out and over / on states
    void set_images(const char* off, const char* on)
    {
        bnt_off = tx->texture_lookup(off);
        bnt_on = tx->texture_lookup(on);


        use_images = true;

        //re-calculate the hit area based from the images
        s_w = bnt_off->w;
        s_h = bnt_off->h;
    };

    void draw()
    {

        if(bnt_off && btn_state == B_OFF){
            SDL_Rect ds = {s_x,s_y,bnt_off->w,bnt_off->h};
            SDL_BlitSurface( bnt_off,0, guibuffer,&ds );
        }else if (bnt_on && btn_state == B_ON){
            SDL_Rect ds = {s_x,s_y,bnt_on->w,bnt_on->h};
            SDL_BlitSurface( bnt_on,0, guibuffer,&ds );
        }
        if (text_l1) {
            fnts->set_size(font_sz);

            if (center_text) {
                int text_len = fnts->get_length_px(text_l1,font_sz);
                int cx = 0;
                text_len>0 ? cx = (int)((s_w-(text_len))/2): cx=1;
                fnts->text(guibuffer,text_l1,cx+s_x,s_y+pad_y,limit_text);
            }else{
                fnts->text(guibuffer,text_l1,s_x+pad_x,s_y+pad_y,limit_text);
            }

        }

    };

    bool test_zorder(int c)
    {
        return (c == z_order ? true : false);
    };

    int mouse_over(int x, int y,int current_z)
    {
  /*    TODO!
        if (!test_zorder(current_z))
            return B_OUT;

        x+=X_OFFSET;
        y+=Y_OFFSET;

        if (x > s_x && x < (s_x+s_w) && y > s_y && y < (s_y+s_h)) btn_state = B_OVER;
        else btn_state = B_OUT;

        return btn_state;
        */

        return 0;
    }

    int hit_test(int x, int y, int current_z)
    {
        if (!test_zorder(current_z))
            return 0;

        x+=X_OFFSET;
        y+=Y_OFFSET;

        if ((x > s_x && x < (s_x+s_w) && y > s_y && y < (s_y+s_h))) btn_state == B_ON ? btn_state = B_OFF : btn_state = B_ON;


        return btn_state;
    };
};

#endif
