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
    GUI_UNKNOWN = 0,
    GUI_BUTTON,
    GUI_TOGGLE,
    GUI_GROUP,
    GUI_TEXTBOX,
    GUI_SLIDER,
    GUI_LIST,
    GUI_IMG,
    GUI_SHAPE,
    GUI_IMG_LIST,
    GUI_SCREEN,
    GUI_POPUP,
    GUI_IMG_ANIMATION,
    GUI_VISUAL,
    GUI_LABEL,
    GUI_FONT,
    GUI_KEYBOARD,
    GUI_ART_GRID

};

enum
{
    GUI_IMG_DOWN = 0,
    GUI_IMG_OUT,
    GUI_IMG_OVER,
    GUI_IMG_ON,
    GUI_IMG_OFF,

    GUI_IMG_DOWN_OL,
    GUI_IMG_OUT_OL,
    GUI_IMG_OVER_OL,
    GUI_IMG_ON_OL,
    GUI_IMG_OFF_OL,

    GUI_IMG_BG,
    GUI_IMG_SLIDERPOS_OUT,
    GUI_IMG_SLIDERPOS_OVER,

    GUI_IMG_PLAYING,

 //   GUI_IMG_KEY_OUT,
//    GUI_IMG_KEY_OVER,

    GUI_IMG_MAX

};

class gui_object {

    public:

    int     obj_type;

    int     s_x;
    int     s_y;
    int     s_h;
    int     s_w;
    u32     text_color;
    u32     text_color_over;
    u32     text_color_playing;
    u32     text_color_down;

    /*
        stores item images
        i.e.
            mouse over and click
            mouse NOT over
            mouse over no click
            mouse over and state is set to on
            mouse NOT over state is set off

            repeat above for overlay states
    */
    SDL_Surface*    object_images[GUI_IMG_MAX];


    int             font_sz;
    int             z_order;    // zorder ... max of 3
    int             bind_screen;

    fonts*          fnts;

    bool            center_text;
    int             limit_text;

    int             obj_state; // -- out, over, click
    public:
    int             obj_sub_state; // -- on off

    public:
    SDL_Surface*    guibuffer; // where to blit!
private:
    char*           text;

public:
    int             pad_x;
    int             pad_y;

    bool            isvariable; // is the text a variable ?

    app_wiiradio*   theapp;

    func_void       click_func;
    func_void       over_func;
    func_void       right_click_func;

    char            click_script[MED_MEM];

    int             object_id;
    bool            visible;

    u32             name;
    gui_object*     parent;

    char            text_name[SMALL_MEM]; // actual name
    char*           script_onclick;

    int             center_img; // prolly should move

    union
    {
        struct _rgb
        {
#ifdef _WII_
            u8 a, r, g, b;
#else
            u8 b, g, r, a;
#endif
        }cbyte;
        u32 color;
    } bgcolor;


   // u32             bgcolor; // back color

    int             show_on_status; // if this status is not met the object will not be active
    char            show_on_var[SMALL_MEM];
    char            fun_arg[SMALL_MEM];

    public:

    gui_object();
    virtual ~gui_object();

    const int GetType() const { return obj_type; };

    virtual const int IsModal() { return 0; };

    // -- set the images for this gui object
    void set_image_img(char* img);
    void set_image_out(char* img);
    void set_image_down(char* img);
    void set_image_over(char* img);
    void set_image_onout(char* img);
    void set_image_ondown(char* img);
    void set_image_onover(char* img);
    void set_show_status(const char* status);
    void set_image_playing(const char* img);
    void set_images(const char* out, const char* over, const char* off, const char* on);

    void set_binds(char* instr);
    // -- draw to screen
    virtual int draw();
    bool  point_in_rect(const int x, const int y);
    //-- hit test
    virtual int hit_test(const SDL_Event *event);

    // -- set text
    virtual void set_text(const char* t);
    // -- test the z-order
    bool test_zorder(const int c);

    // gets the REAL value, usful if variable
    const char *get_text();
    const char *get_var();
    // Set the id (name) of the current object
    void set_name(char* name, unsigned long name_hash);
    void set_script_onclick(char* script_data);

    // is this object part of the current screen?
    bool check_screen(unsigned long cur);

    virtual bool IsVisible();



    const char* get_text_raw() { return text; }
};


#endif // GUI_OBJECT_H_INCLUDED
