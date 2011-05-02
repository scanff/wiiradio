#ifndef VISUAL_BOBS_H_INCLUDED
#define VISUAL_BOBS_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_bobs : public visual_object
{
    public:

    #define NUMBER_OF_BOBS (40)

    int xpath[512];
    int ypath[512];
    SDL_Rect rects[NUMBER_OF_BOBS];
    SDL_Rect oldrects[NUMBER_OF_BOBS];
    Uint32  i, j, k, l, m;
    SDL_Surface* eye_surface;

    vis_bobs(app_wiiradio*   _theapp) : eye_surface(0)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        layer = 0;

        for(int x=0;x<MAX_FFT_RES;x++)
        {
            peakResults[x] = 0;
            fft_results[x] = 0;
        }

        i = 0;
        j = 40;
    };

    ~vis_bobs()
    {
        if (eye_surface)
        {
            SDL_FreeSurface(eye_surface);
            eye_surface = 0;
        }
    };


    void load(void* userdata)
    {
        int i, hw, hh;
        unsigned int k, l, m;
        if (!eye_surface) eye_surface = IMG_Load(make_path("data/visdata/eye.png"));

        hw = (DRAW_WIDTH - 60) >> 1;
        hh = (DRAW_HEIGHT - 60) >> 1;


        for(i=0;i<512;i++)
        {
            double rad = ((float)i * 0.703125) * 0.0174532;
            xpath[i] = (int)(sin(rad) * hw + hw);
            ypath[i] = (int)(cos(rad) * hh + hh);
        }

        l = i;
        m = j;

        for (k = 0; k < NUMBER_OF_BOBS; ++k)
        {
            rects[k].w = oldrects[k].w = eye_surface->w;
            rects[k].h = oldrects[k].h = eye_surface->h;
            rects[k].x = xpath[l & 511];
            rects[k].y = ypath[m & 511];
            l += 20;
            m += 20;
        }

        loaded = true;
    };

    void render(SDL_Surface* s,void* user_data, const int max)
    {
        visual_object::render(s,user_data,max);

        if (!loaded) load(user_data);
        if(!eye_surface) return;

        l = i;
        m = j;


        for (k = 0; k < NUMBER_OF_BOBS; ++k)
        {
            oldrects[k].x = rects[k].x;
            rects[k].x = xpath[l & 511];
            oldrects[k].y = rects[k].y;
            rects[k].y = ypath[m & 511];

            l += 30;
            m += 20;

            SDL_BlitSurface(eye_surface, 0 , s, rects + k);
        }

        double percent = ((double)20 / (double)32767);
        int peak = (int)(percent * (double)fft_results[2]);

        i += 2+ peak;
        j += 3+ peak;
        i &= 511;
        j &= 511;
    };
};
#endif
