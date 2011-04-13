#ifndef VISUAL_FIRE_H_INCLUDED
#define VISUAL_FIRE_H_INCLUDED

#include "visual_object.h"

class vis_fire : public visual_object
{
    public:

    unsigned int fire[SCREEN_WIDTH / 4][SCREEN_HEIGHT / 4];  //this buffer will contain the fire
    unsigned int palette[256]; //this will contain the color palette

    vis_fire(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        DRAW_WIDTH = SCREEN_WIDTH / 4;
        DRAW_HEIGHT = SCREEN_HEIGHT / 4;
        layer = 0;
    };

    ~vis_fire()
    {
    };


    void load(void* userdata)
    {

        int peak = f->getPeak();

        for(int x = 0; x < 256; x++)
        {
           // palette[x] = hsl_rgba(x / 3, 255, lmin(255, x * 2));
           palette[x] = hsl_rgba((int)(x / 1.5), 255, lmin(100,(int)(peak)));
        }

        if (loaded) return;

        for(int x = 0; x < DRAW_WIDTH; x++)
            for(int y = 0; y < DRAW_HEIGHT; y++)
                fire[x][y] = 0;



        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {
        load(userdata);

        int x1,x2,x3,x4,y1,y2;

      //randomize the bottom row of the fire buffer
        for(int x = 0; x < DRAW_WIDTH; x++) fire[x][DRAW_HEIGHT - 1] = abs(32768 + rand()) % 256;
        //do the fire calculations for every pixel, from top to bottom
        for(int y = 0; y < DRAW_HEIGHT - 1; y++)
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            x1 = (x - 1 + DRAW_WIDTH) % DRAW_WIDTH;
            x2 = (x) % DRAW_WIDTH;
            x3 = (x + 1) % DRAW_WIDTH;
            x4 = (x) % DRAW_WIDTH;
            y1 = (y + 1) % DRAW_HEIGHT;
            y2 = (y + 2) % DRAW_HEIGHT;

            if( x1 < DRAW_WIDTH &&
                x2 < DRAW_WIDTH &&
                x3 < DRAW_WIDTH &&
                x4 < DRAW_WIDTH &&
                y1 < DRAW_HEIGHT &&
                y2 < DRAW_HEIGHT
                )
                {

                    fire[x][y] =
                    ((fire[x1][y1]
                    + fire[x2][y1]
                    + fire[x3][y1]
                    + fire[x4][y2])
                    * 32) / 129;
                }else exit(0);
        }

        //set the drawing buffer to the fire buffer, using the palette colors
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            for(int y = 0; y < DRAW_HEIGHT; y++)
            {
                pixelColor(s,x,y,palette[fire[x][y]]);
            }
        }

    }
};


#endif // VISUAL_FIRE_H_INCLUDED
