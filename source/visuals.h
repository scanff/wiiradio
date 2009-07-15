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

#include "visuals/visual_tunnel.h"
#include "visuals/visual_fire.h"
#include "visuals/visual_osc.h"
#include "visuals/visual_bars.h"
#include "visuals/visual_explode.h"
#include "visuals/visual_game1.h"


class visualizer
{
    public:

    fft*            f;
    int             fft_results[MAX_FFT_RES];
    SDL_Surface*    vis_surface;
    void*           visuals_ptr[MAX_VISUALS];
    bool            remap_keys;

    visualizer(fft* p_f) : f(p_f), remap_keys(false)
    {
         loopi(MAX_FFT_RES) {
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
        delete_visuals(-1);
    };

    void delete_visuals(int number)
    {

        if (number != V_BARS)
        {
            if (visuals_ptr[V_BARS])
            {
                delete (vis_bars*)visuals_ptr[V_BARS];
                visuals_ptr[V_BARS] = 0;
            }
        }

        if (number != V_OSC)
        {
            if (visuals_ptr[V_OSC])
            {
                delete (vis_osc*)visuals_ptr[V_OSC];
                visuals_ptr[V_OSC] = 0;
            }
        }

        if (number != V_TUNNEL)
        {
            if (visuals_ptr[V_TUNNEL]) {
                delete (vis_tunnel*)visuals_ptr[V_TUNNEL];
                visuals_ptr[V_TUNNEL] = 0;
            }
        }
        if (number != V_FIRE)
        {
            if (visuals_ptr[V_FIRE]) {
                delete (vis_fire*)visuals_ptr[V_FIRE];
                visuals_ptr[V_FIRE] = 0;
            }
        }

        if (number != V_GAME1)
        {
            remap_keys = false;
            if (visuals_ptr[V_GAME1])
            {
                delete (vis_game1*)visuals_ptr[V_GAME1];
                visuals_ptr[V_GAME1] = 0;
            }
        }

    /*    if (number != V_EXPLODE)
        {
            if (visuals_ptr[V_EXPLODE]) {
                delete (vis_explode*)visuals_ptr[V_EXPLODE];
                visuals_ptr[V_EXPLODE] = 0;
            }

        }*/

    }

    void draw_visuals(SDL_Surface* s,int number)
    {


        SDL_Rect sr = {0,0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2};

        switch(number)
        {
            case V_BARS:
                draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
                if (!visuals_ptr[V_BARS]) visuals_ptr[V_BARS] = new vis_bars(f,this);
                else {
                    vis_bars* a = (vis_bars*)visuals_ptr[V_BARS];
                    loopi(MAX_FFT_RES)  a->fft_results[i] =  fft_results[i];
                    a->render(vis_surface);
                }

                SDL_BlitSurface(vis_surface,0,s,0);

            break;

            case V_OSC:
                draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
                if (!visuals_ptr[V_OSC]) {
                     visuals_ptr[V_OSC] = new vis_osc(f);

                }else {
                    vis_osc*  a = (vis_osc*)visuals_ptr[V_OSC];
                    a->render(vis_surface);
                }
                SDL_BlitSurface(vis_surface,0,s,0);
            break;

            case V_TUNNEL:

                if (!visuals_ptr[V_TUNNEL]) {

                     visuals_ptr[V_TUNNEL] = new vis_tunnel(f);
                     draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
                }else {
                    vis_tunnel* a = (vis_tunnel*)visuals_ptr[V_TUNNEL];
                    a->render(vis_surface);
                }

                SDL_SoftStretch(vis_surface,&sr,s,0);
            break;

            case V_FIRE:
                draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
                if (!visuals_ptr[V_FIRE])
                {
                     visuals_ptr[V_FIRE] = new vis_fire(f);
                }else{
                    vis_fire* a =(vis_fire*)visuals_ptr[V_FIRE];
                    a->render(vis_surface);
                }

                SDL_SoftStretch(vis_surface,&sr,s,0);
            break;
            case V_GAME1:

                draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
                if (!visuals_ptr[V_GAME1])
                {
                    visuals_ptr[V_GAME1] = new vis_game1(f);
                    remap_keys = true;
                }else{
                    vis_game1* a =(vis_game1*)visuals_ptr[V_GAME1];
                    remap_keys = a->render(vis_surface);
                }

                 SDL_BlitSurface(vis_surface,0,s,0);
            break;
/*            case V_EXPLODE:

                if (!visuals_ptr[V_EXPLODE]) visuals_ptr[V_EXPLODE] = new vis_explode(f);
                else{
                    vis_explode* a = (vis_explode*)visuals_ptr[V_EXPLODE];
                    a->render(vis_surface);
                }
                SDL_BlitSurface(vis_surface,0,s,0);
                //SDL_SoftStretch(vis_surface,&sr,s,0);
            break;
*/

        };

        // clean up if not using
       delete_visuals(number);
    };






};

#endif // VISUALS_H_INCLUDED
