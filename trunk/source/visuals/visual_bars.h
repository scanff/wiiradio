#ifndef VISUAL_BARS_H_INCLUDED
#define VISUAL_BARS_H_INCLUDED

class visualizer;
class vis_bars {
    public:

    fft*            f;
    bool            loaded;

    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;

    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    visualizer*     parent;

    vis_bars(fft* _f,visualizer* _p) : f(_f), loaded(false), parent(_p)
    {
        DRAW_WIDTH = SCREEN_WIDTH;
        DRAW_HEIGHT = SCREEN_HEIGHT;

        loopi(MAX_FFT_RES) {
            peakResults[i] = 0;
            fft_results[i] = 0;
        }
    };

    ~vis_bars()
    {

    };


    void load()
    {

        loaded = true;
    };

    void render(SDL_Surface* s)
    {
        if (!loaded) load();

        int x = 0;
        int y = SCREEN_HEIGHT - 50;
        int bar_height = 300;
        int bar_width = SCREEN_WIDTH / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);

        loopi(MAX_FFT_RES)
        {
            // peak
            peakResults[i] -= (32767 / bar_height) * 4;

            if(peakResults[i] < 0) peakResults[i] = 0;
            if(peakResults[i] < fft_results[i]) peakResults[i] = fft_results[i];

            int peaks_newy = (y - (int)(percent * (double)peakResults[i]));

            //fft
            int newY = (y - (int)(percent * (double)fft_results[i]));
            int h = abs(y - newY);

            //draw
            draw_rect(s,x+i*bar_width,newY,bar_width-4,h,0xcc0022 +  i*10 );
            draw_rect(s,x+i*bar_width,peaks_newy,bar_width-4,5,0xcc0022 + i*10 << 16 );
        }

    };
};
#endif // VISUAL_BARS_H_INCLUDED
