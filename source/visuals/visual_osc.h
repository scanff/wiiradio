#ifndef VISUAL_OSC_H_INCLUDED
#define VISUAL_OSC_H_INCLUDED


class vis_osc {
    public:

    fft*            f;
    bool            loaded;

    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;

    int             colors[256];


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
        for(int x = 0; x < 256; x++)
        {
            colors[x] = hsl_rgba(x / 2, 255, lmin(255, x * 2));
           //palette[x] = hsl_rgba((int)(x / 1.5), 255, lmin(255,(int)(peak * 1.1)));
        }

        loaded = true;


    };

    void render(SDL_Surface* s)
    {
        if (!loaded) load();

        int sx,sy,x,y;
        int zerolevel = ((DRAW_HEIGHT/2)-10);
        int len = (MAX_FFT_SAMPLE) - 1;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len/2);
        double range = 20;
        double timescale = zerolevel/range;

        sx = 0;
        sy = zerolevel;
        int index = 0;

         fade(s,SDL_MapRGB(s->format,0,0,0),40);

        for(int i = 8; i < (len-1); i+=8)
        {
            x = (int)((double)i*ts);
            y = (int)((double)zerolevel - (short)(f->real[i])*timescale);

            index = (int)y/8;
            index < 0 ? index = 0 : index > 255 ? index = 255 : 0;

         //   unsigned long c = colors[index];
            bresenham_line(s,x,y,sx,sy,0xffffffff);

            sy = y;
            sx = x;

        }


    };
};
#endif // VISUAL_OSC_H_INCLUDED
