#include "globals.h"
#include "application.h"
#include "fonts.h"

fonts::fonts() :  size(FS_SMALL)
{
    loopi(FS_MAX) fontsets[i] = 0;

    if (TTF_Init() == -1) exit(0);;

}

void fonts::init()
{
   if (!(fontsets[FS_SYSTEM_TINY] = TTF_OpenFont(make_path((char*)"data/FreeSansBold.ttf"), 8)))
        exit(0); // can't really continue ... probably should do better than exit tho.
    if (!(fontsets[FS_SYSTEM] = TTF_OpenFont(make_path((char*)"data/FreeSansBold.ttf"), 14)))
        exit(0); // can't really continue ... probably should do better than exit tho.
    if (!(fontsets[FS_SYSTEM_MED] = TTF_OpenFont(make_path((char*)"data/FreeSansBold.ttf"), 22)))
        exit(0); // can't really continue ... probably should do better than exit tho.
    if (!(fontsets[FS_SYSTEM_LARGE] = TTF_OpenFont(make_path((char*)"data/FreeSansBold.ttf"), 26)))
        exit(0); // can't really continue ... probably should do better than exit tho.
}
void fonts::reload_fonts(char* file,int* sizes)
{
    loopi(FS_SYSTEM) // Don't include system font
    {
        if (fontsets[i])
            TTF_CloseFont(fontsets[i]);

        if(!(fontsets[i] = TTF_OpenFont(make_path(file), sizes[i])))
        {
            exit(0);
        }
    }

}


fonts::~fonts()
{
    loopi(FS_MAX)
    {
        if (fontsets[i])
            TTF_CloseFont(fontsets[i]);
    }

    TTF_Quit();

}

void fonts::change_color(const int r, const int g, const int b)
{
    color_r = r;
    color_g = g;
    color_b = b;
}

// set the size of the font
void fonts::set_size(int s)
{
    size = s;
}

int fonts::get_length_px(const char* t, int sizein)
{

    int w,h;

    TTF_SizeUTF8 (fontsets[size], t, &w, &h);

    return w;
}

int fonts::text(SDL_Surface* s, const char* textin, int x, int y, const int limit_width,int align)
{
    if (!textin) return 0;
    if (strlen(textin) <=0) return 0;

    SDL_Color tmpfontcolor = {color_r,color_g,color_b,0};

    SDL_Surface *resulting_text = 0;

    //resulting_text = TTF_RenderText_Blended(ft, textin, tmpfontcolor); //better/slower
    resulting_text = TTF_RenderUTF8_Blended(fontsets[size], textin, tmpfontcolor); //better/slower


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

}
