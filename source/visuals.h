/*

    I've used lots of sources to help me with these effects.  To name a few :

    http://www.student.kuleuven.be/~m0216922/CG/ (Lode's Computer Graphics Tutorial)
    and
    http://demo-effects.sourceforge.net/
*/

#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

static void bresenham_line(SDL_Surface* s, int x1, int y1, int x2, int y2,unsigned long color)
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


static unsigned int hsl_rgba(int ia, int ib, int ic)
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

static int lmin(int a, int b)
{
  if (a <= b) return a;
  else return b;

  return a;
};

#include "visuals/visual_water.h"
#include "visuals/visual_tunnel.h"
#include "visuals/visual_fire.h"
#include "visuals/visual_osc.h"
#include "visuals/visual_bars.h"
#include "visuals/visual_explode.h"
#include "visuals/visual_mist.h"
#include "visuals/visual_circles.h"
#include "visuals/visual_plasma.h"
#include "visuals/visual_sintext.h"
#include "visuals/visual_rotzoom.h"
#include "visuals/visual_bobs.h"
#include "visuals/visual_lasers.h"
#include "visuals/visual_raycaster.h"
#include "visuals/visual_matrix.h"
#include "visuals/visual_stars.h"

#include "visuals/visual_object.h"

class visualizer
{
    public:

    fft*            f;
    int             fft_results[MAX_FFT_RES+1];
    SDL_Surface*    vis_surface;
    visual_object*  visuals_ptr[MAX_VISUALS];
    void*           user_data;
    int             r1, r2;
    unsigned long   vt;
    int             mode;

    visualizer(fft* p_f) : f(p_f), mode(1)
    {
        r1 = r2 = vt = 0;
        loopi(MAX_FFT_RES)
        {
            fft_results[i] = 0;
        }

        loopi(MAX_VISUALS) visuals_ptr[i] = 0;

        // surface for visuals
        unsigned long rmask, gmask, bmask, amask;

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


        // create all visuals
        visuals_ptr[V_BARS]     = new vis_bars(f);
        visuals_ptr[V_OSC]      = new vis_osc(f);
        visuals_ptr[V_TUNNEL]   = new vis_tunnel(f);
        visuals_ptr[V_FIRE]     = new vis_fire(f);
        visuals_ptr[V_MIST]     = new vis_mist(f);
        visuals_ptr[V_CIRCLES]  = new vis_circles(f);
        visuals_ptr[V_PLASMA]   = new vis_plasma(f);
        visuals_ptr[V_SINTEXT]  = new vis_sintext(f);
        visuals_ptr[V_ROTZOOM]  = new vis_rotzoom(f);
        visuals_ptr[V_BOBS]     = new vis_bobs(f);
        visuals_ptr[V_LASERS]   = new vis_lasers(f);
        visuals_ptr[V_RAYCASTER]= new vis_raycaster(f);
        visuals_ptr[V_MATRIX]   = new vis_matrix(f);
        visuals_ptr[V_STARS]    = new vis_stars(f);
        visuals_ptr[V_WATER]    = new vis_water(f);
        visuals_ptr[V_EXPLODE]  = new vis_explode(f);


        srand ( get_tick_count() );


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


    void draw_visuals(SDL_Surface* s,int number)
    {
       if (g_real_keys[SDLK_1] && !g_keys_last_state[SDLK_1])
       mode = !mode;//1;

        int num_of_visuals = 0; // number of visuals we are showing

        if (mode == 1)
        {
            num_of_visuals = 2;
            if ((get_tick_count() - vt) > 15000)
            {
                r1 = rand() % MAX_VISUALS;

                while(1)
                {
                    r2 = (rand() % MAX_VISUALS);
                    // only some effects on top and not the same effect as r1
                    if( !visuals_ptr[r2]->layer || (r2 == r1)) continue;

                    break;
                }
                vt = get_tick_count();
            }
        }else{
            draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
            num_of_visuals = 1;
            r1 = number;
        }


        loopj(num_of_visuals)
        {
            void* udata = 0;
            int v =0;

            j==0?v=r1:v=r2;

            loopi(MAX_FFT_RES)  visuals_ptr[v]->fft_results[i] =  fft_results[i];

            if (j > 0) SDL_BlitSurface(s,0,vis_surface,0);

            if (j > 0 && v == V_TUNNEL) udata = (void*)vis_surface->pixels;
            if(v == V_SINTEXT) udata = user_data;

            visuals_ptr[v]->render(vis_surface,udata);

            if (visuals_ptr[v]->DRAW_WIDTH != s->w)
            {
                SDL_Rect sr = {0,0,visuals_ptr[v]->DRAW_WIDTH,visuals_ptr[v]->DRAW_HEIGHT};
                SDL_Rect dr = {0,0,s->w,s->h};
                SDL_SoftStretch(vis_surface,&sr,s,&dr);
            }else{
                SDL_BlitSurface(vis_surface,0,s,0);
            }

        }

    };

};



#endif // VISUALS_H_INCLUDED
