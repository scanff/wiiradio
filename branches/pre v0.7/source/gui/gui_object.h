#ifndef GUI_OBJECT_H_INCLUDED
#define GUI_OBJECT_H_INCLUDED

enum
{
    B_OUT = 0,
    B_OVER,
    B_CLICK,
    B_ON,
    B_OFF

};
enum
{
    GUI_BUTTON = 0,
    GUI_TOGGLE,
    GUI_GROUP,
    GUI_TEXTBOX

};
class gui_object {

    public:

    int     obj_type;

    int     s_x;
    int     s_y;
    int     s_h;
    int     s_w;
    long    text_color;
    long    text_color_over;


    SDL_Surface*    bnt_down;
    SDL_Surface*    bnt_out;
    SDL_Surface*    bnt_over;
    SDL_Surface*    bnt_on;
    SDL_Surface*    bnt_off;

    int             font_sz;
    int             z_order;    // zorder ... max of 3
    int             bind_screen; // what screen is this button bound to

    fonts*          fnts;

    bool            center_text;
    int             limit_text;

    public:
    int             obj_state; // -- out, over, click

    public:
    SDL_Surface*    guibuffer; // where to blit!

    char*           text_l1;
    int             pad_x;
    int             pad_y;

    bool            isvariable; // is the text a variable ?

    app_wiiradio*   theapp;

    func_void       click_func;
    func_void       over_func;
    func_void       right_click_func;

    int             object_id;
    bool            visible;

    gui_object() :
        obj_type(0),
        s_x(0),
        s_y(0),
        s_h(0),
        s_w(0),
        text_color(0),
        text_color_over(0),
        bnt_down(0),
        bnt_out(0),
        bnt_over(0),
        bnt_on(0),
        bnt_off(0),
        font_sz(0),
        z_order(0),
        bind_screen(0),
        center_text(true),
        limit_text(0),
        obj_state(B_OUT),
        pad_x(0),
        pad_y(0),
        isvariable(false),
        click_func(0),
        over_func(0),
        right_click_func(0),
        object_id(0),
        visible(true)
    {
        text_l1 = new char[SMALL_MEM + 1];
        memset(text_l1,0,SMALL_MEM + 1);
    };

    ~gui_object()
    {
        delete [] text_l1;
    };

    // -- set the images for this gui object
    void set_images(char* out, char* over,char* off, char* on)
    {
        texture_cache* tx = theapp->tx;

        switch(obj_type)
        {
            case GUI_TEXTBOX:
            case GUI_BUTTON:

            bnt_out = tx->texture_lookup(out);
            bnt_over = tx->texture_lookup(over);
            bnt_down = bnt_over;

            if(!bnt_out || !bnt_over)
                exit(0);


            //re-calculate the hit area based from the images
            if(bnt_out) {
                s_w = bnt_out->w;
                s_h = bnt_out->h;
            }

            break;

            case GUI_TOGGLE:

            bnt_off = tx->texture_lookup(off);
            bnt_on = tx->texture_lookup(on);


            if(!bnt_off || !bnt_on) exit(0);


            //re-calculate the hit area based from the images
            if(bnt_off) {
                s_w = bnt_off->w;
                s_h = bnt_off->h;
            }

            break;

            default: return;

        }
    };

    // -- draw to screen
    void draw()
    {

        if(bnt_out && obj_state == B_OUT){

            SDL_Rect ds = {s_x,s_y,bnt_out->w,bnt_out->h};
            SDL_BlitSurface( bnt_out,0, guibuffer,&ds );
            fnts->change_color((text_color >> 16), ((text_color >> 8) & 0xff),(text_color & 0xff));

        }else if (bnt_over && obj_state == B_OVER){

            SDL_Rect ds = {s_x,s_y,bnt_over->w,bnt_over->h};
            SDL_BlitSurface( bnt_over,0, guibuffer,&ds );
            fnts->change_color((text_color_over >> 16), ((text_color_over >> 8) & 0xff),(text_color_over & 0xff));

        }else if (bnt_down && obj_state == B_CLICK){

            SDL_Rect ds = {s_x,s_y,bnt_down->w,bnt_down->h};
            SDL_BlitSurface( bnt_down,0, guibuffer,&ds );
            fnts->change_color((text_color_over >> 16), ((text_color_over >> 8) & 0xff),(text_color_over & 0xff));

        }else if(bnt_off && obj_state == B_OFF){

            SDL_Rect ds = {s_x,s_y,bnt_off->w,bnt_off->h};
            SDL_BlitSurface( bnt_off,0, guibuffer,&ds );

        }else if (bnt_on && obj_state == B_ON){

            SDL_Rect ds = {s_x,s_y,bnt_on->w,bnt_on->h};
            SDL_BlitSurface( bnt_on,0, guibuffer,&ds );

        }

    };

    bool inline point_in_rect(const int x, const int y)
    {
        return (bool)(x > s_x) && (x < (s_x + s_w)) && (y > s_y) && (y < (s_y + s_h));
    }

    //-- hit test
    int hit_test(SDL_Event *event, int current_z)
    {
        int x;
        int y;

        switch (event->type) {
            case SDL_MOUSEMOTION:
                x = event->motion.x;
                y = event->motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                x = event->button.x;
                y = event->button.y;
                break;
            default:
                return obj_state;
        }

        obj_type == GUI_TOGGLE ? B_OFF: obj_state = B_OUT;

        if (point_in_rect(x,y))
        {
            if (current_z > z_order) return obj_state;

            switch(obj_type)
            {
                case GUI_BUTTON:
                case GUI_TEXTBOX:

                  if ((obj_state = (event->type == SDL_MOUSEBUTTONDOWN) ? B_CLICK : B_OVER) == B_CLICK)
                    if (click_func) click_func(this);

                  break;

                case GUI_TOGGLE:
                  if (event->type == SDL_MOUSEBUTTONDOWN)
                      obj_state = (obj_state == B_ON) ? B_OFF : B_ON;
                break;
            }
        }

        return obj_state;
    };

    // -- set text
    void set_text(const char* t)
    {
        if(!t) return;
        else { strcpy(text_l1,t); }
    };

    // -- test the z-order
    bool test_zorder(const int c)
    {
        return (c > z_order ? true : false);
    };
};


#endif // GUI_OBJECT_H_INCLUDED
