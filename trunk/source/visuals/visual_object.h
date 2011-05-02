#ifndef VISUAL_OBJECT_H_INCLUDED
#define VISUAL_OBJECT_H_INCLUDED

#include "../globals.h"
#include "../application.h"

#define DRAW_WIDTH      (320)
#define DRAW_HEIGHT     (320)

class visual_object {
    public:

    fft*            f;
    bool            loaded;

    int             mode;
    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    int             layer;
    int             samp_max;
    bool            noclearbg;

    app_wiiradio*   theapp;

    visual_object()  { layer = 0; samp_max = 0; noclearbg = false;};
    virtual ~visual_object(){};

    virtual void load(void*) {};
    virtual void render(SDL_Surface*,void*,const int max)
    {
        samp_max = max;
    }


};

#endif // VISUAL_OBJECT_H_INCLUDED
