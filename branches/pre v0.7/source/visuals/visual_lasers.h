#ifndef VISUAL_LASERS_H_INCLUDED
#define VISUAL_LASERS_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_lasers : public visual_object
{
    public:

    #define MAX_LASERS  (8)

    int ly[MAX_LASERS];
    int step[MAX_LASERS];
    int dir[MAX_LASERS];

    vis_lasers(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        layer = 1;

        DRAW_WIDTH = SCREEN_WIDTH ;
        DRAW_HEIGHT = SCREEN_HEIGHT;

        loopi(MAX_FFT_RES) {
            peakResults[i] = 0;
            fft_results[i] = 0;
        }

        loopi(MAX_LASERS)
        {
            ly[i] = 0;
            step[i] = 8 * (i+1);
            dir[i] = 0;
        }
    };

    ~vis_lasers()
    {
    };

    void load(void* userdata)
    {
        loaded = true;
    };

    void render(SDL_Surface* s,void* user_data)
    {
        if (!loaded) load(user_data);

        double percent = ((double)200 / (double)32767);
        int peak = (int)(percent * (double)fft_results[3]);

        fade(s,SDL_MapRGB(s->format,0,0,0),40);

        int xx = 0;
        int yy = 0;

        loopi(MAX_LASERS)
        {

            for(int x = 0; x < DRAW_WIDTH; x+=step[i])
            {
                unsigned int color = hsl_rgba((10+(int)(x*0.5)) % 255, 100,lmin(peak,255));
                bresenham_line(s, DRAW_WIDTH / 2, DRAW_HEIGHT / 2 ,x*(xx+1),ly[i]*(yy+1),color);
            }

            yy++;

            if (yy >=2)
            {
                xx++;
                yy=0;
            }

            if (!dir[i]) ly[i]+=step[i]*2;
            else ly[i]-=step[i]*2;

            if (ly[i] > DRAW_HEIGHT)  dir[i] = 1;
            if (ly[i] < 0)  dir[i] = 0;
        }

    };
};
#endif
