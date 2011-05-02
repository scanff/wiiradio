#ifndef VISUAL_CIRCLES_H_INCLUDED
#define VISUAL_CIRCLES_H_INCLUDED

#include "visual_object.h"

class vis_circles : public visual_object
{
    public:

    int             shifter;
    int             direction;

    vis_circles(app_wiiradio*   _theapp)
    {
        loaded = false;
        theapp = _theapp;
        f = theapp->fourier;
        layer = 1;
        direction = shifter = 0;

    };

    ~vis_circles()
    {

    };


    void load(void* userdata)
    {

        if (loaded) return;
        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata, const int max)
    {
        visual_object::render(s,userdata,max);

        load(userdata);

        float divisor = 0.0008;

        int y = 0;
        int len = (MAX_FFT_SAMPLE/2) - 1;
        int padding = 70;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len-(padding*2));
        long color;
        int x = 0;
        int y2 = 0;

        for(int l=padding;l<(len-1-padding);l+=2)
        {
            x = (int)((double)(l-padding)*ts);
            y = (int)(2 * (sqrt(abs((int)(f->getPosition(l) / divisor)))));
            y = DRAW_HEIGHT - y;

            if (y >= 0) // filter bad reading
            {
                for(y2=DRAW_HEIGHT/2;y2>y/2;y2-=10)
                {
                    int y3 = y2;//(int)(y2/ 2);
                    int d = y2;
                    d = d / 2;
                    d+=shifter;


                    if (d > 255) d = 255;
                    color = hsl_rgba(rand() % 255, d, d);
                    circleColor(s,x,y3,2,color);
                    circleColor(s,x,DRAW_HEIGHT-y3,2,color);

                }

            }

        }
        if (!direction) shifter < 150 ? shifter++ : direction = 1;
        else shifter > 0 ? shifter-- : direction = 0;


    };
};


#endif // VISUAL_MIST_H_INCLUDED
