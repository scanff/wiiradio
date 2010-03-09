#ifndef VISUAL_SINTEXT_H_INCLUDED
#define VISUAL_SINTEXT_H_INCLUDED

#include "visual_object.h"

class vis_sintext : public visual_object
{
    public:

    int pos_x;
    short aSin[360];
    int xoffsets[255];
    int p;

    vis_sintext(fft* _f)
    {
        loaded = false;
        f = _f;
        p = pos_x = 0;

        DRAW_WIDTH = SCREEN_WIDTH;
        DRAW_HEIGHT = SCREEN_HEIGHT;


    };

    ~vis_sintext()
    {

    };

    void load(void* user_data)
    {
        if(loaded) return;

        short centery = DRAW_HEIGHT >> 1;
        loopi(360)
        {
            float rad =  (float)i * 0.0174532;
            aSin[i] = centery + (short)((sin(rad) * 45.0));
        }

        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {

        load(userdata);

        char* a = 0;
        if (userdata) a = (char*)userdata;

        if (!a) return;

        fade(s,SDL_MapRGB(s->format,0,0,0),40);

        int peak = 0;
        loopi((MAX_FFT_SAMPLE) - 1) f->real[i] > peak ? peak = (int)f->real[i] : 0;

        fnts->set_size(FS_LARGE);
        fnts->change_color(200,0,0);

        char single[2] = {0};
        loopi(strlen(a))
        {
            long color = hsl_rgba(i, 255, lmin(100,(int)(peak)));
            int r,g,b;
            r = color >> 16 & 0xff;
            g = color >> 8 & 0xff;
            b = color & 0xff;

            fnts->change_color(r,g,b);

            single[0] = a[i];
            single[1] = 0;

            xoffsets[i] = fnts->get_length_px(single,FS_LARGE);
            int x = 0;
            loopj(i) x += xoffsets[j];

            x += pos_x;
            fnts->text(s,single,x,aSin[(p + i) % 360],640);

        }

        p += 8;
        pos_x-=8;

        if (pos_x < (-fnts->get_length_px(a,FS_LARGE))) pos_x = 640;
    }
};

#endif // VISUAL_TUNNEL_H_INCLUDED
