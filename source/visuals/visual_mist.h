#ifndef VISUAL_MIST_H_INCLUDED
#define VISUAL_MIST_H_INCLUDED

class vis_mist {
    public:

    fft*            f;
    bool            loaded;
    float*          fft_r;
    int             DRAW_WIDTH;// = SCREEN_WIDTH / 2;
    int             DRAW_HEIGHT;// = SCREEN_HEIGHT / 2;

    int             luma;
    int             luma_dir;

    vis_mist(fft* _f) : f(_f), loaded(false)
    {
        DRAW_WIDTH = SCREEN_WIDTH ;
        DRAW_HEIGHT = SCREEN_HEIGHT;
        luma = 50;
        luma_dir = 1;
    };

    ~vis_mist()
    {

    };


    void load()
    {

        if (loaded) return;
        loaded = true;
    };

    void render(SDL_Surface* s)
    {
        load();
        float divisor = 0.008;

        int y = 0;
        int len = (MAX_FFT_SAMPLE) - 1;
        int padding = 70;
        double ts = static_cast<double>(DRAW_WIDTH)/static_cast<double>(len-(padding*2));
        long color,color_p;
        int x = 0;
        int y2 = 0;

        color = hsl_rgba(150, 180, luma);
        color_p = hsl_rgba(luma, 180, 200);

        fade(s,SDL_MapRGB(s->format,0,0,0),50);

        for(int l=padding;l<(len-1-padding);l++)
        {
            x = (int)((double)(l-padding)*ts);
            y = (int)(8 * (sqrt(abs((int)(f->real[l] / divisor)))));
            y = DRAW_HEIGHT - y;

            if (y >= 0) // filter bad reading
            {
                for(y2=DRAW_HEIGHT;y2>y;y2--)
                {
                    pixelColor(s,x,y2, color);
                }

                pixelColor(s,x,y2,color_p);
            }

        }

        luma += luma_dir;
        if (luma >= 200) luma_dir = -1;
        if (luma <= 50) luma_dir = 1;

    };
};


#endif // VISUAL_MIST_H_INCLUDED
