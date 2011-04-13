#ifndef VISUAL_MIST_H_INCLUDED
#define VISUAL_MIST_H_INCLUDED

#include "visual_object.h"

class vis_mist : public visual_object
{
    public:

    int             shifter;
    int             direction;

    vis_mist(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 1;

        DRAW_WIDTH = SCREEN_WIDTH  ;
        DRAW_HEIGHT = SCREEN_HEIGHT  ;
        direction = shifter = 0;

    };

    ~vis_mist()
    {
    };


    void render(SDL_Surface* s,void* userdata)
    {

        float divisor = 0.0008;
        int y = 0;
        int len = (MAX_FFT_SAMPLE) - 1;
        int padding = 70;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len-(padding*2));
        long color;
        int x = 0;
        int y2 = 0;

        fade(s,SDL_MapRGB(s->format,0,0,0),50);

        for(int l=padding;l<(len-1-padding);l++)
        {
            x = (int)((double)(l-padding)*ts);
            y = (int)(2 * (sqrt(abs((int)(f->getPosition(l) / divisor)))));
            y = DRAW_HEIGHT - y;

            if (y >= 0) // filter bad reading
            {
                for(y2=DRAW_HEIGHT/2;y2 > y/2; y2--)
                {
                    int y3 = y2;
                    int d = y2;
                    d = d / 2;
                    d+=shifter;
                    if (d > 255) d = 255;
                    color = hsl_rgba(d, 255, d);

                    pixelColor(s,x,y3, color);
                    pixelColor(s,x,DRAW_HEIGHT-y3, color);

                }

            }

        }
        if (!direction) shifter < 150 ? shifter++ : direction = 1;
        else shifter > 0 ? shifter-- : direction = 0;


    };
};


#endif // VISUAL_MIST_H_INCLUDED
