#ifndef VISUAL_CIRCULAR_H_INCLUDED
#define VISUAL_CIRCULAR_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_cicular : public visual_object
{
    public:

    vis_cicular(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        layer = 1;

        loopi(MAX_FFT_RES) {
            peakResults[i] = 0;
            fft_results[i] = 0;
        }

    }

    void load(void* userdata)
    {
        loaded = true;
    }

    void render(SDL_Surface* s,void* user_data, const int max)
    {
        int r = (DRAW_WIDTH>>2);
        int iterations=(int)((2*r*PI)+0.5) >> 1;
        double angle;
        int x1;
        int y1;
        int ox =  (DRAW_WIDTH >> 1);
        int oy =  (DRAW_HEIGHT >> 1);
        u32 color = 0xFF0000;
        int x2 = ox;
        int y2 = oy;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(MAX_FFT_SAMPLE/8);
        double scale = ((double)(DRAW_HEIGHT >> 2) / 16384.0);
        int x, ynuml;
        short *wavedata = (short*)theapp->audio_data;

        for(int i=0;i<=iterations;i++)
        {
             x = (int)(double)(i*ts);
             ynuml = abs((int)(scale * wavedata[x * 2 + 0]));
             angle=(((2*M_PI*i)/iterations)+0.5);

             x1=int(ox+(r*cos(angle))+0.5);
             y1=int(oy+(r*sin(angle))+0.5);

             if(x1 > ox) x1 += (ynuml>>1);
             else x1 -= (ynuml>>1);

             if(y1 > oy) y1 += (ynuml>>1);
             else y1 -= (ynuml>>1);

             if(i > 1)
             {
                bresenham_line(s,x2,y2,x1,y1,color);
             }

             x2 = x1;
             y2 = y1;
        }

    };
};
#endif
