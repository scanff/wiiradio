#ifndef VISUAL_ROTZOOM_H_INCLUDED
#define VISUAL_ROTZOOM_H_INCLUDED

#include "visual_object.h"

class vis_rotzoom : public visual_object
{
    public:

    int roto[256];
    int roto2[256];
    unsigned int path, zpath;
    SDL_Surface* tex;

    vis_rotzoom(app_wiiradio*   _theapp) : tex(0)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 0;
        path = zpath = 0;
        DRAW_WIDTH = SCREEN_WIDTH;
        DRAW_HEIGHT = SCREEN_HEIGHT;



    };

    ~vis_rotzoom()
    {
        if (tex)
        {
            SDL_FreeSurface(tex);
            tex = 0;
        }
    };


    void load(void* userdata)
    {
        if (!tex) tex = IMG_Load(make_path("visdata/2.png"));

        if (loaded) return;


        loopi(256)
        {
            float rad =  (float)i * 1.41176 * 0.0174532;
            float c = sin(rad);
            roto[i] = (int)((c + 0.8) * 4096.0);
            roto2[i] = (int)((2.0 * c) * 4096.0);
        }

        loaded = true;

    };


    void render(SDL_Surface* s,void* userdata)
    {
        load(userdata);

        /* draw the tile at current zoom and rotation */
        draw_tile(s,roto[path], roto[(path + 128) & 255], roto2[zpath]);

        path = (path - 1) & 255;
        zpath = (zpath + 1) & 255;


    };

    void draw_tile(SDL_Surface* s, int stepx, int stepy, int zoom)
    {
        unsigned char* image = (unsigned char*)s->pixels;
        unsigned char* texture = (unsigned char*)tex->pixels;
        if (!texture) return;

        int x, y, i, j, xd, yd, a, b, sx, sy;

        // peak beat
        double percent = ((double)200 / (double)32767);
        int peak = (int)(percent * (double)fft_results[3]);

        sx = sy = 0;
        xd = (stepx * zoom) >> 12;
        yd = (stepy * zoom) >> 12;

        for (j = 0; j < DRAW_HEIGHT; j++)
        {
            x = sx; y = sy;
            for (i = 0; i < DRAW_WIDTH; i++)
            {
              a = x >> 14 & 255;
              b = y >> 14 & 255;

                long pos = b * (tex->w) + a;
                pos *= tex->format->BitsPerPixel / 8;

                if (pos < 0 || pos+2 >= ((DRAW_WIDTH*3)*DRAW_HEIGHT)) continue;

#ifdef _WII_
                *image++ = texture[pos] + peak;
                *image++ = texture[pos+1] + peak;
                *image++ = texture[pos+2] + peak;

#else
                *image++ = texture[pos+2] + peak;
                *image++ = texture[pos+1] + peak;
                *image++ = texture[pos] + peak;
#endif
                x += xd; y += yd;
            }

            sx -= yd; sy += xd;
        }
    };
};


#endif //
