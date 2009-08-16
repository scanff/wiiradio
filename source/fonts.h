
#ifndef _FONTS_H_
#define _FONTS_H_

enum _font_sizes {
    FS_LARGE = 0,
    FS_MED,
    FS_SMALL,
    FS_SYSTEM
};

class fonts {
    public:

    int size;
    TTF_Font* fontset_large;
    TTF_Font* fontset_medium;
    TTF_Font* fontset_small;
    TTF_Font* fontset_system;
    int color_r;
    int color_g;
    int color_b;


    fonts() :  size(FS_SMALL), fontset_large(0), fontset_medium(0), fontset_small(0) , fontset_system(0)
    {

        if (TTF_Init() == -1) return;

        fontset_system = TTF_OpenFont(make_path((char*)"FreeSansBold.ttf"), 14);
        if (!fontset_system) exit(0);


    };

    void reload_fonts(char* file,int s, int m, int l)
    {
        if(fontset_large) TTF_CloseFont(fontset_large);
        if(fontset_medium) TTF_CloseFont(fontset_medium);
        if(fontset_small) TTF_CloseFont(fontset_small);

        fontset_large = TTF_OpenFont(make_path(file), l);
        if (!fontset_large) exit(0);

        fontset_medium = TTF_OpenFont(make_path(file), m);
        if (!fontset_medium) exit(0);

        fontset_small = TTF_OpenFont(make_path(file), s);
        if (!fontset_small) exit(0);

    };


    ~fonts()
    {
        TTF_CloseFont(fontset_large);
        TTF_CloseFont(fontset_medium);
        TTF_CloseFont(fontset_small);

        TTF_CloseFont(fontset_system);

        TTF_Quit();

    };

    void change_color(int r, int g, int b)
    {

        color_r = r;
        color_g = g;
        color_b = b;

    };

    // set the size of the font
    void set_size(int s)
    {
        size = s;
    };

    int get_length_px(char* t, int sizein)
    {

        int w,h;
        TTF_Font* ft;
        if (sizein == FS_LARGE){
            ft = fontset_large;
        }else if (sizein == FS_MED) {
            ft = fontset_medium;
        }else if (sizein == FS_SYSTEM) {
            ft = fontset_system;
        }else{ ft = fontset_small;}

        //TTF_SizeText(ft, t, &w, &h);
        TTF_SizeUTF8 (ft, t, &w, &h);

        return w;
    };


    int text(SDL_Surface* s, const char* textin,int x, int y,int limit_width,int align = 0)
    {
        if (!textin) return 0;
        if (strlen(textin) <=0) return 0;

        SDL_Color tmpfontcolor = {color_r,color_g,color_b,0};

        SDL_Surface *resulting_text = 0;

        TTF_Font* ft;

        if (size == FS_LARGE) {
            ft = fontset_large;
        }else if (size == FS_MED){
            ft = fontset_medium;
        }else if (size == FS_SYSTEM){
            ft = fontset_system;
        }else{ ft = fontset_small; }

        //resulting_text = TTF_RenderText_Blended(ft, textin, tmpfontcolor); //better/slower
        resulting_text = TTF_RenderUTF8_Blended(ft, textin, tmpfontcolor); //better/slower


        if (!resulting_text)
            return 0;


        if (align == 1) x = x - get_length_px((char*)textin,size);
       // if (align == 2) x = (SCREEN_WIDTH / 2) - (get_length_px((char*)textin,size) / 2);

        SDL_Rect ds = { x, y , limit_width ,FONT_SIZE};

        if (limit_width) {

            SDL_Rect sr = { 0, 0 , limit_width ,FONT_SIZE};
            SDL_BlitSurface( resulting_text,&sr, s, &ds );

        } else SDL_BlitSurface( resulting_text,0, s, &ds );

        SDL_FreeSurface(resulting_text);

        return get_length_px((char*)textin,size); // ????

    };
};

#endif //_FONTS_H_
