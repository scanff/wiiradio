#ifndef VISUAL_PLASMA_H_INCLUDED
#define VISUAL_PLASMA_H_INCLUDED

#include "visual_object.h"

class vis_plasma : public visual_object
{
    public:

    SDL_Color       colors[256];
    int             aSin[512];
    Uint16          pos1, pos2, pos3, pos4, tpos1, tpos2, tpos3, tpos4;

    vis_plasma(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 0;

        pos1 = pos2 = pos3 = pos4 =  tpos1 = tpos2 = tpos3 =  tpos4 = 0;

        DRAW_WIDTH = SCREEN_WIDTH  ;
        DRAW_HEIGHT = SCREEN_HEIGHT  ;

    };

    ~vis_plasma()
    {

    };


    void load(void* userdata)
    {

        if (loaded) return;

        // init plasma
        float rad;

        /*create sin lookup table */
        loopi(512)
        {
          rad =  ((float)i * 0.703125) * 0.0174532; /* 360 / 512 * degree to rad, 360 degrees spread over 512 values to be able to use AND 512-1 instead of using modulo 360*/
          aSin[i] = (int)(sin(rad) * 1024); /*using fixed point math with 1024 as base*/
        }

        /* create palette */
        loopi(64)
        {
          colors[i].r = i << 2;
          colors[i].g = 255 - ((i << 2) + 1);
          colors[i+64].r = 255;
          colors[i+64].g = (i << 2) + 1;
          colors[i+128].r = 255 - ((i << 2) + 1);
          colors[i+128].g = 255 - ((i << 2) + 1);
          colors[i+192].g = (i << 2) + 1;
        }

        loaded = true;

    };


    void render(SDL_Surface* s,void* userdata)
    {
        load(userdata);

        unsigned char* image;
        unsigned char index;
        int x;
        int rd,gn,bl;
        int peak = 0;

        double percent = ((double)200 / (double)32767);
        // peak beat
        peak = (int)(percent * (double)fft_results[2]);

        /* draw plasma */

        tpos4 = pos4;
        tpos3 = pos3;

        image = (unsigned char*)s->pixels;

        loopi(DRAW_HEIGHT)
        {
            tpos1 = pos1 + 5;
            tpos2 = pos2 + 3;

            tpos3 &= 511;
            tpos4 &= 511;

            loopj(DRAW_WIDTH)
            {
              tpos1 &= 511;
              tpos2 &= 511;

              x = aSin[tpos1] + aSin[tpos2] + aSin[tpos3] + aSin[tpos4]; /*actual plasma calculation*/

              index = 128 + (x >> 4); /*fixed point multiplication but optimized so basically it says (x * (64 * 1024) / (1024 * 1024)), x is already multiplied by 1024*/



#ifdef _WII_
              rd = colors[index].r + peak;
              bl = colors[index].b + peak;
#else
              rd = colors[index].b + peak;
              bl = colors[index].r + peak;
#endif
              gn = colors[index].g + peak;


              *image++ = rd;
              *image++ = gn;
              *image++ = bl;


              tpos1 += 5;
              tpos2 += 3;
            }

            tpos4 += 3;
            tpos3 += 1;
        }

        /* move plasma */

        pos1 +=9;
        pos3 +=8;


    };
};


#endif // VISUAL_MIST_H_INCLUDED
