#ifndef VISUAL_BARS_H_INCLUDED
#define VISUAL_BARS_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_bars : public visual_object
{
    public:

    vis_bars(fft* _f)
    {
        f = _f;
        loaded = false;
        layer = 1;

        DRAW_WIDTH = SCREEN_WIDTH / 2;
        DRAW_HEIGHT = SCREEN_HEIGHT / 2;

        loopi(MAX_FFT_RES) {
            peakResults[i] = 0;
            fft_results[i] = 0;
        }
    };

    ~vis_bars()
    {

    };


    void load()
    {
        loaded = true;
    };

    void render(SDL_Surface* s)
    {
        if (!loaded) load();

        //draw_rect(s,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer

        int x = 0;
        int y = DRAW_HEIGHT;
        int bar_height = 200;
        int bar_width = DRAW_WIDTH / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);

        loopi(MAX_FFT_RES)
        {
            // peak
            peakResults[i] -= (32767 / bar_height) * 4;

            if(peakResults[i] < 0) peakResults[i] = 0;
            if(peakResults[i] < fft_results[i]) peakResults[i] = fft_results[i];

            int peaks_newy = (y - (int)(percent * (double)peakResults[i]));

            //fft
            int newY = (y - (int)(percent * (double)fft_results[i]));
            int h = abs(y - newY);

            //draw
            draw_rect(s,x+i*bar_width,newY,bar_width-4,h,0xcc0022 +  i*10 );
            draw_rect(s,x+i*bar_width,peaks_newy,bar_width-4,2,(0xcc0022 + i*10) << 16 );
        }

    };
};
#endif // VISUAL_BARS_H_INCLUDED
