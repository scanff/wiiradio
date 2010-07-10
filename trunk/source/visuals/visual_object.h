#ifndef VISUAL_OBJECT_H_INCLUDED
#define VISUAL_OBJECT_H_INCLUDED

#include "../globals.h"

class visual_object {
    public:

    fft*            f;
    bool            loaded;

    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;

    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    int             layer;
    visual_object(){ layer = 0; };
    virtual ~visual_object(){};

    virtual void load(void*) {};
    virtual void render(SDL_Surface*,void*) {};


};

#endif // VISUAL_OBJECT_H_INCLUDED