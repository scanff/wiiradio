
#ifndef _FONTS_H_
#define _FONTS_H_

enum _font_sizes {
    FS_LARGE = 0,
    FS_MED,
    FS_SMALL,
    // -- never delete system font
    FS_SYSTEM,
    FS_SYSTEM_MED,
    FS_SYSTEM_LARGE,
    FS_SYSTEM_TINY,
    FS_MAX
};

class fonts
{
    public:

    private:

    int         size;
    TTF_Font*   fontsets[FS_MAX];
    int         color_r;
    int         color_g;
    int         color_b;

    public:

    fonts();
    void reload_fonts(char* file,int* sizes);
    ~fonts();

    void init();

    void change_color(const int r, const int g, const int b);
    void set_size(int s);
    int get_length_px(const char* t, int sizein);
    int text(SDL_Surface* s, const char* textin, int x, int y, const int limit_width,int align = 0);
};

#endif //_FONTS_H_
