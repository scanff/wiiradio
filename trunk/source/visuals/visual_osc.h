#ifndef VISUAL_OSC_H_INCLUDED
#define VISUAL_OSC_H_INCLUDED

#include "visual_object.h"

class vis_osc : public visual_object
{
    public:


    vis_osc(fft* _f)
    {
        f = _f;
        loaded = false;

        DRAW_WIDTH = SCREEN_WIDTH ;
        DRAW_HEIGHT = SCREEN_HEIGHT;
    };

    ~vis_osc()
    {

    };


    void load(void* userdata)
    {
        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {
        if (!loaded) load(userdata);

        int sx,sy,x,y;
        int zerolevel = ((DRAW_HEIGHT/2)-10);
        int len = (MAX_FFT_SAMPLE) - 1;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len/2);
        double range = 20;
        double timescale = zerolevel/range;

        sx = 0;
        sy = zerolevel;
        int index = 0;
        long color;

        fade(s,SDL_MapRGB(s->format,0,0,0),40);


        for(int i = 8; i < (len-1); i+=8)
        {
            x = (int)((double)i*ts);
            y = (int)((double)zerolevel - (short)(f->real[i])*timescale);

            index = (int)y/8;
            index < 0 ? index = 0 : index > 255 ? index = 255 : 0;

            int d = y;
            d = d / 2;

            if (d > 255) d = 255;
            color = hsl_rgba(d, 200, 150);

            bresenham_line(s,x,y,sx,sy,color);

            sy = y;
            sx = x;

        }

    };
};
#endif // VISUAL_OSC_H_INCLUDED
