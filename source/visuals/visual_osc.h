#ifndef VISUAL_OSC_H_INCLUDED
#define VISUAL_OSC_H_INCLUDED


class vis_osc {
    public:

    fft*            f;
    bool            loaded;

    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;




    vis_osc(fft* _f) : f(_f), loaded(false)
    {
        DRAW_WIDTH = SCREEN_WIDTH;
        DRAW_HEIGHT = SCREEN_HEIGHT;
    };

    ~vis_osc()
    {

    };


    void load()
    {

        loaded = true;
    };

    void render(SDL_Surface* s)
    {
        if (!loaded) load();

        int sx,sy,x,y;
        int zerolevel = ((DRAW_HEIGHT/2)-10);
        int len = (8192/4) - 1;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len/2);
        double range = 20;
        double timescale = zerolevel/range;

        sx = 0;
        sy = zerolevel;
        loopi(len-1)
        {
            x = (int)((double)i*ts);
            y = (int)((double)zerolevel - (short)(f->real[i])*timescale);

            bresenham_line(s,x,y,sx,sy,0xffffffff);

            sy = y;
            sx = x;
        }


    };
};
#endif // VISUAL_OSC_H_INCLUDED
