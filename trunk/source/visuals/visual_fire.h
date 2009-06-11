#ifndef VISUAL_FIRE_H_INCLUDED
#define VISUAL_FIRE_H_INCLUDED

class vis_fire {
    public:

    fft*            f;
    bool            loaded;

    int             DRAW_WIDTH;// = SCREEN_WIDTH / 2;
    int             DRAW_HEIGHT;// = SCREEN_HEIGHT / 2;

    unsigned int fire[SCREEN_WIDTH / 2][SCREEN_HEIGHT / 2];  //this buffer will contain the fire
    unsigned int palette[256]; //this will contain the color palette




    vis_fire(fft* _f) : f(_f), loaded(false)
    {
        DRAW_WIDTH = SCREEN_WIDTH / 2;
        DRAW_HEIGHT = SCREEN_HEIGHT / 2;
    };

    ~vis_fire()
    {

    };


    void load()
    {
        for(int x = 0; x < DRAW_WIDTH; x++)
            for(int y = 0; y < DRAW_HEIGHT; y++)
                fire[x][y] = 0;


          for(int x = 0; x < 256; x++)
          {

            palette[x] = hsl_rgba(x / 3, 255, lmin(255, x * 2));
          }

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

      //randomize the bottom row of the fire buffer
        for(int x = 0; x < DRAW_WIDTH; x++) fire[x][DRAW_HEIGHT - 1] = abs(32768 + rand()) % 256;
        //do the fire calculations for every pixel, from top to bottom
        for(int y = 0; y < DRAW_HEIGHT - 1; y++)
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
          fire[x][y] =
            ((fire[(x - 1 + DRAW_WIDTH) % DRAW_WIDTH][(y + 1) % DRAW_HEIGHT]
            + fire[(x) % DRAW_WIDTH][(y + 1) % DRAW_HEIGHT]
            + fire[(x + 1) % DRAW_WIDTH][(y + 1) % DRAW_HEIGHT]
            + fire[(x) % DRAW_WIDTH][(y + 2) % DRAW_HEIGHT])
            * 32) / 129;
        }

        //set the drawing buffer to the fire buffer, using the palette colors
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            int yy = (int)((double)DRAW_HEIGHT/2 - (short)(f->real[x+50])*timescale) / 1.2;
            for(int y = 0; y < DRAW_HEIGHT; y++)
            {
               if (y>yy) pixelColor(s,x,y,palette[fire[x][y]]);
            }
        }

    }
};


#endif // VISUAL_FIRE_H_INCLUDED
