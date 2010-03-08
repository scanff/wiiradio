/*

    I've used lots of sources to help me with these effects.  To name a few :

    http://www.student.kuleuven.be/~m0216922/CG/ (Lode's Computer Graphics Tutorial)
    and
    http://demo-effects.sourceforge.net/
*/

#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

 void bresenham_line(SDL_Surface* s, int x1, int y1, int x2, int y2,unsigned long color)
{
    // clip
    x1 > SCREEN_WIDTH ? x1 = SCREEN_WIDTH : x1 < 0 ? x1 = 0 : 0;
    x2 > SCREEN_WIDTH ? x2 = SCREEN_WIDTH : x2 < 0 ? x2 = 0 : 0;
    y1 > SCREEN_HEIGHT ? y1 = SCREEN_HEIGHT : y1 < 0 ? y1 = 0 : 0;
    y2 > SCREEN_HEIGHT ? y2 = SCREEN_HEIGHT : y2 < 0 ? y2 = 0 : 0;


    int delta_x = abs(x2 - x1) << 1;
    int delta_y = abs(y2 - y1) << 1;

    // if x1 == x2 or y1 == y2, then it does not matter what we set here
    signed char ix = x2 > x1?1:-1;
    signed char iy = y2 > y1?1:-1;

    pixelColor(s,x1,y1,color);

    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error = delta_y - (delta_x >> 1);

        while (x1 != x2)
        {
            if (error >= 0)
            {
                if (error || (ix > 0))
                {
                    y1 += iy;
                    error -= delta_x;
                }
                // else do nothing
            }
            // else do nothing

            x1 += ix;
            error += delta_y;

            pixelColor(s,x1,y1,color);
        }
    }
    else
    {
        // error may go below zero
        int error = delta_x - (delta_y >> 1);

        while (y1 != y2)
        {
            if (error >= 0)
            {
                if (error || (iy > 0))
                {
                    x1 += ix;
                    error -= delta_y;
                }
                // else do nothing
            }
            // else do nothing

            y1 += iy;
            error += delta_x;

            pixelColor(s,x1,y1,color);

        }
    }
};


unsigned int hsl_rgba(int ia, int ib, int ic)
{
    float r, g, b, h, s, l;
    float temp1, temp2, tempr, tempg, tempb;
    static unsigned int rgb = 0;
    h = ia / 256.0;
    s = ib / 256.0;
    l = ic / 256.0;

    if(s == 0) r = g = b = l;
    else
    {
        //Set the temporary values
        if(l < 0.5) temp2 = l * (1 + s);
        else temp2 = (l + s) - (l * s);
        temp1 = 2 * l - temp2;
        tempr = h + 1.0 / 3.0;
        if(tempr > 1) tempr--;
        tempg = h;
        tempb = h - 1.0 / 3.0;
        if(tempb < 0) tempb++;

        //Red
        if(tempr < 1.0 / 6.0) r = temp1 + (temp2 - temp1) * 6.0 * tempr;
        else if(tempr < 0.5) r = temp2;
        else if(tempr < 2.0 / 3.0) r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
        else r = temp1;

        //Green
        if(tempg < 1.0 / 6.0) g = temp1 + (temp2 - temp1) * 6.0 * tempg;
        else if(tempg < 0.5) g = temp2;
        else if(tempg < 2.0 / 3.0) g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
        else g = temp1;

        //Blue
        if(tempb < 1.0 / 6.0) b = temp1 + (temp2 - temp1) * 6.0 * tempb;
        else if(tempb < 0.5) b = temp2;
        else if(tempb < 2.0 / 3.0) b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
        else b = temp1;
    }
#ifdef _WII_
    rgb = (int(r * 255.0) << 8);
    rgb += (int(g * 255.0) << 16);
    rgb += (int(b * 255.0) << 24);
    rgb += 0xff; // no alpha
#else
    rgb = (int(r * 255.0) << 24);
    rgb += (int(g * 255.0) << 16);
    rgb += (int(b * 255.0) << 8);
    rgb += 0xff; // no alpha
#endif
    return rgb;
};

int lmin(int a, int b)
{
  if (a <= b) return a;
  else return b;

  return a;
};
//#include "visuals/visual_helli.h"
#include "visuals/visual_tunnel.h"
#include "visuals/visual_fire.h"
#include "visuals/visual_osc.h"
#include "visuals/visual_bars.h"
#include "visuals/visual_explode.h"
#include "visuals/visual_game1.h"
#include "visuals/visual_mist.h"
#include <SDL/SDL_imageFilter.h>

#include "visuals/visual_object.h"

class visualizer
{
    public:

    fft*            f;
    int             fft_results[MAX_FFT_RES];
    SDL_Surface*    vis_surface;
    visual_object*  visuals_ptr[MAX_VISUALS];
    bool            remap_keys;
    double          angle;

    visualizer(fft* p_f) : f(p_f), remap_keys(false), angle(0.0)
    {
        loopi(MAX_FFT_RES)
        {
            fft_results[i] = 0;
        }

        loopi(MAX_VISUALS) visuals_ptr[i] = 0;

        // surface for visuals
        Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#else
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#endif

        vis_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH,
                                          rmask, gmask, bmask,amask);

    };

    ~visualizer()
    {
        SDL_FreeSurface(vis_surface);
        loopi(MAX_VISUALS)
        {
            if (visuals_ptr[i])
            {
                delete visuals_ptr[i];
                visuals_ptr[i] = 0;
            }
        }
    };

    void delete_visuals(int number)
    {
        loopi(MAX_VISUALS)
        {
            if (i != number && visuals_ptr[i])
            {
                delete visuals_ptr[i];
                visuals_ptr[i] = 0;
            }
        }
    };

    visual_object* newvisual(int number)
    {
        switch(number)
        {
            case V_BARS:
                return new vis_bars(f);
            break;

            case V_OSC:
                return new vis_osc(f);
            break;

            case V_TUNNEL:
                return new vis_tunnel(f);
            break;

            case V_FIRE:
                return new vis_fire(f);
            break;

            case V_MIST:
               return new vis_mist(f);
            break;
        }

        return 0;
    };

    void draw_visuals(SDL_Surface* s,int number)
    {
        // clean up if not using
       delete_visuals(number);

        if (!visuals_ptr[number])
        {
            draw_rect(s,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
            visuals_ptr[number] = newvisual(number);
        }

        loopi(MAX_FFT_RES)  visuals_ptr[number]->fft_results[i] =  fft_results[i];

        visuals_ptr[number]->render(vis_surface);

        if (visuals_ptr[number]->DRAW_WIDTH != SCREEN_WIDTH)
        {
            SDL_Rect sr = {0,0,visuals_ptr[number]->DRAW_WIDTH,visuals_ptr[number]->DRAW_HEIGHT};
            SDL_SoftStretch(vis_surface,&sr,s,0);
        }else SDL_BlitSurface(vis_surface,0,s,0);

    };






};

#endif // VISUALS_H_INCLUDED
