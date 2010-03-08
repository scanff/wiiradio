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

    visual_object(){};
    ~visual_object(){};

    virtual void load() {};
    virtual void render(SDL_Surface*) {};


};

#endif // VISUAL_OBJECT_H_INCLUDED
