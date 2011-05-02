#ifndef VISUAL_STARS_H_INCLUDED
#define VISUAL_STARS_H_INCLUDED

#include "visual_object.h"

class visualizer;
class vis_stars : public visual_object
{
    public:

    #define MAX_STARS  (300)

    struct _stars
    {
        int x;
        int y;
        int speed;
        int color;
//        int DRAW_WIDTH;
//        int DRAW_HEIGHT;

        void init(SDL_Surface* s)
        {
 //           DRAW_WIDTH = s->w;
 //           DRAW_HEIGHT = s->h;

            x = (rand() % DRAW_WIDTH);
            y = (rand() % DRAW_HEIGHT);
            speed = (rand() % 10) + 5;

            color = 0x333333ff * speed;

            pixelColor(s,x,y,color);
        };

        void move(SDL_Surface* s,int peak)
        {
           // pixelColor(s,x,y,0x0);

            y += speed;
            if (y >= DRAW_HEIGHT) y = 0;

            pixelColor(s,x,y,color|peak);
        }


    } starfield[MAX_STARS];

    vis_stars(app_wiiradio*   _theapp)
    {
        theapp = _theapp;
        f = theapp->fourier;
        loaded = false;
        layer = 1;


    };

    ~vis_stars()
    {
    };

    void load(void* userdata)
    {

        loaded = true;
    };

    void render(SDL_Surface* s,void* user_data, const int max)
    {
        if (!loaded)
        {
            load(user_data);
            loopi(MAX_STARS) starfield[i].init(s);
        }

        loopi(MAX_STARS) starfield[i].move(s,fft_results[4]);

    };
};
#endif
