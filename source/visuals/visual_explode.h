#ifndef VISUAL_EXPLODE_H_INCLUDED
#define VISUAL_EXPLODE_H_INCLUDED

class vis_explode {
    public:

    fft*            f;
    bool            loaded;
    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;

    #define NUMBER_OF_PARTICLES 500

    unsigned int*   fire;
    int             colors[256];

    /*particle structure*/
    typedef struct
    {
      Uint16 xpos,ypos,xdir,ydir;
      Uint8 colorindex;
      Uint8 dead;
    } PARTICLE;

    PARTICLE particles[NUMBER_OF_PARTICLES];

    vis_explode(fft* _f) : f(_f), loaded(false), fire(0)
    {
        DRAW_WIDTH = SCREEN_WIDTH / 2;
        DRAW_HEIGHT = SCREEN_HEIGHT / 2;

        fire = new unsigned int[DRAW_WIDTH * DRAW_HEIGHT];
        if(!fire) exit(0);

        srand(get_tick_count());


    };

    ~vis_explode()
    {
        delete [] fire;
        fire = 0;
    };

    void init_particle(PARTICLE* particle)
    {
      /* randomly init particles, generate them in the center of the screen */

      particle->xpos =  (DRAW_WIDTH >> 1) - 20 + (int)(40.0 * (rand()/(RAND_MAX+1.0)));
      particle->ypos =  (DRAW_HEIGHT >> 1) - 20 + (int)(40.0 * (rand()/(RAND_MAX+1.0)));
      particle->xdir =   -10 + (int)(20.0 * (rand()/(RAND_MAX+1.0)));
      particle->ydir =   -17 + (int)(19.0 * (rand()/(RAND_MAX+1.0)));
      particle->colorindex = 255;
      particle->dead = 0;
    }

    void load()
    {
        memset(fire, 0, DRAW_WIDTH * DRAW_HEIGHT * sizeof(unsigned int));

        for(int x = 0; x < 256; x++)
            colors[x] = hsl_rgba(x / 3, 255, lmin(255, x * 2));

        loopi(NUMBER_OF_PARTICLES)
            init_particle(particles + i);


        loaded = true;
    };

    void render(SDL_Surface* s)
    {
        unsigned char* image;
        unsigned int buf, index, temp;
        int num_dead = 0;
        loopi(NUMBER_OF_PARTICLES) if(particles[i].dead) num_dead++;

        if (!loaded || (num_dead >= NUMBER_OF_PARTICLES -5)) load();

        loopi(NUMBER_OF_PARTICLES)
        {
            if (!particles[i].dead)
            {
                particles[i].xpos += particles[i].xdir;
                particles[i].ypos += particles[i].ydir;

              /* is particle dead? */

                if ((particles[i].ypos >= DRAW_HEIGHT - 3) || particles[i].colorindex == 0 ||
                    particles[i].xpos <= 3 || particles[i].xpos >= DRAW_WIDTH - 3 || particles[i].ypos <= 3)
                {
                    particles[i].dead = 1;
                    continue;
                }

                /* gravity takes over */

                particles[i].ydir++;

                /* particle cools off */

                particles[i].colorindex--;

                temp = particles[i].ypos * DRAW_WIDTH + particles[i].xpos;

                fire[temp] = particles[i].colorindex;
                fire[temp - 1] = particles[i].colorindex;
                fire[temp + DRAW_WIDTH] = particles[i].colorindex;
                fire[temp - DRAW_WIDTH] = particles[i].colorindex;
                fire[temp + 1] = particles[i].colorindex;
            }
        }

      /* create fire effect */

        for (int ii = 1; ii < DRAW_HEIGHT - 2; ii++)
        {
            index = (ii  - 1) * DRAW_WIDTH;

            for (int j = 1; j < DRAW_WIDTH - 2; j++)
            {
                buf = index + j;

                temp = fire[buf];
                temp += fire[buf + 1];
                temp += fire[buf - 1];
                buf += DRAW_WIDTH;
                temp += fire[buf - 1];
                temp += fire[buf + 1];
                buf += DRAW_WIDTH;
                temp += fire[buf];
                temp += fire[buf + 1];
                temp += fire[buf - 1];

                temp >>= 3;

                if (temp > 4) temp -= 4;
                else temp = 0;


                fire[buf - DRAW_WIDTH] = temp;
            }
        }

        image = (unsigned char*)s->pixels;

      /* draw fire array to screen from bottom to top*/

        for (int y = 0; y < DRAW_HEIGHT; y++)
        {
            for (int x = 0;x < DRAW_WIDTH; x++)
            {
                temp = y * DRAW_WIDTH;

                *image = fire[temp + x];
                image += 3;
            }
        }

    };
};

#endif // VISUAL_EXPLODE_H_INCLUDED