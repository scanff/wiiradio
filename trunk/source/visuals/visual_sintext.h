#ifndef VISUAL_SINTEXT_H_INCLUDED
#define VISUAL_SINTEXT_H_INCLUDED

#include "visual_object.h"
#include "../fonts.h"

class vis_sintext : public visual_object
{
    public:

    int pos_x;
    short aSin[360];
    int xoffsets[255];
    int p;

    vis_sintext(app_wiiradio* _theapp)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        p = pos_x = 0;
        layer = 1;


    };

    ~vis_sintext()
    {

    };

    void load(void* user_data)
    {
        if(loaded) return;

        short centery = DRAW_HEIGHT >> 1;
        loopi(360)
        {
            float rad =  (float)i * 0.0174532;
            aSin[i] = centery + (short)((sin(rad) * 45.0));
        }

        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata, const int max)
    {
        visual_object::render(s,userdata,max);

        load(userdata);

        char* a = 0;
        if (userdata) a = (char*)userdata;

        if (!a) return;

        int peak = samp_max;

        theapp->GetFonts()->set_size(FS_SYSTEM_MED);
        theapp->GetFonts()->change_color(200,0,0);

        char single[2] = {0};
        loopi(strlen(a))
        {
            long color = hsl_rgba(i, 255, lmin(100,(int)(peak)));
            int r,g,b;
            r = color >> 16 & 0xff;
            g = color >> 8 & 0xff;
            b = color & 0xff;

            theapp->GetFonts()->change_color(r,g,b);

            single[0] = a[i];
            single[1] = 0;

            xoffsets[i] = theapp->GetFonts()->get_length_px(single,FS_SYSTEM_MED);
            int x = 0;
            loopj(i) x += xoffsets[j];

            x += pos_x;
            theapp->GetFonts()->text(s,single,x,aSin[(p + i) % 360],640);

        }

        p += 8;
        pos_x-=8;

        if (pos_x < (-theapp->GetFonts()->get_length_px(a,FS_SYSTEM_MED))) pos_x = 640;
    }
};

#endif // VISUAL_TUNNEL_H_INCLUDED
