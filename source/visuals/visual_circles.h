#ifndef VISUAL_CIRCLES_H_INCLUDED
#define VISUAL_CIRCLES_H_INCLUDED

#include "visual_object.h"

class vis_circles : public visual_object
{
    public:

    int             shifter;
    int             direction;

    vis_circles(fft* _f)
    {
        loaded = false;
        f = _f;
        layer = 3;

        DRAW_WIDTH = SCREEN_WIDTH  ;
        DRAW_HEIGHT = SCREEN_HEIGHT  ;
        direction = shifter = 0;

    };

    ~vis_circles()
    {

    };


    void load(void* userdata)
    {

        if (loaded) return;
        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {
        load(userdata);

        float divisor = 0.0008;

        int y = 0;
        int len = (MAX_FFT_SAMPLE/2) - 1;
        int padding = 70;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len-(padding*2));
        long color;
        int x = 0;
        int y2 = 0;

        fade(s,SDL_MapRGB(s->format,0,0,0),50);

        for(int l=padding;l<(len-1-padding);l+=2)
        {
            x = (int)((double)(l-padding)*ts);
            y = (int)(2 * (sqrt(abs((int)(f->real[l] / divisor)))));
            y = DRAW_HEIGHT - y;

            if (y >= 0) // filter bad reading
            {
                for(y2=DRAW_HEIGHT/2;y2>y/2;y2-=10)
                {
                    int y3 = y2;//(int)(y2/ 2);
                    int d = y2;
                    d = d / 2;
                    d+=shifter;
                    int r = d / 2;

                    if (d > 255) d = 255;
                    color = hsl_rgba(rand() % 255, d, d);
                    circleColor(s,x,y3,3,color);
                    circleColor(s,x,DRAW_HEIGHT-y3,3,color);

                }

            }

        }
        if (!direction) shifter < 150 ? shifter++ : direction = 1;
        else shifter > 0 ? shifter-- : direction = 0;


    };
};


#endif // VISUAL_MIST_H_INCLUDED
