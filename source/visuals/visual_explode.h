#ifndef VISUAL_EXPLODE_H_INCLUDED
#define VISUAL_EXPLODE_H_INCLUDED

#include "visual_object.h"

class vis_explode : public visual_object
{
    public:

    #define NUMBER_OF_PARTICLES 100
    struct ex_colors
    {
#ifdef _WII_
        u8 r, g, b;
#else
        u8 b, g, r;
#endif
    };

    unsigned int*   fire;
    ex_colors       colors[256];

    /*particle structure*/
    typedef struct
    {
      Uint16 xpos,ypos,xdir,ydir;
      Uint8 colorindex;
      Uint8 dead;
    } PARTICLE;

    PARTICLE particles[NUMBER_OF_PARTICLES];

    vis_explode(app_wiiradio*   _theapp) : fire(0)
    {
        loaded = false;
        theapp = _theapp;
        f = theapp->fourier;

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

    void load(void* userdata)
    {


        memset(fire, 0, DRAW_WIDTH * DRAW_HEIGHT * sizeof(unsigned int));

        for (int i = 0; i < 32; ++i)
        {
            /* black to blue, 32 values*/
            colors[i].b = i << 1;

            /* blue to red, 32 values*/
            colors[i + 32].r = i << 3;
            colors[i + 32].b =  64 - (i << 1);

            /*red to yellow, 32 values*/
            colors[i + 64].r = 255;
            colors[i + 64].g = i << 3;

            /* yellow to white, 162 */
            colors[i + 96].r = 255;
            colors[i + 96].g = 255;
            colors[i + 96].b = i << 2;
            colors[i + 128].r = 255;
            colors[i + 128].g = 255;
            colors[i + 128].b = 64 + (i << 2);
            colors[i + 160].r = 255;
            colors[i + 160].g = 255;
            colors[i + 160].b = 128 + (i << 2);
            colors[i + 192].r = 255;
            colors[i + 192].g = 255;
            colors[i + 192].b = 192 + i;
            colors[i + 224].r = 255;
            colors[i + 224].g = 255;
            colors[i + 224].b = 224 + i;
        }
        colors[0].r = colors[0].g = colors[0].b = 0;

        loopi(NUMBER_OF_PARTICLES)
            init_particle(particles + i);


        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata, const int max)
    {
        double percent = ((double)10 / (double)32767);
        int peak = (int)(percent * (double)fft_results[2]);

        if(peak < 2) return;

        unsigned char* image;
        unsigned int buf, index, temp;
        int num_dead = 0;
        loopi(NUMBER_OF_PARTICLES) if(particles[i].dead) num_dead++;

        if (!loaded || (num_dead >= NUMBER_OF_PARTICLES -5)) load(userdata);

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

        for (int y = 0; y < DRAW_HEIGHT; y++)
        {
            for (int x = 0;x < DRAW_WIDTH; x++)
            {
                temp = (y * DRAW_WIDTH) + x;

                if (colors[fire[temp]].r == 0 && colors[fire[temp]].g == 0 && colors[fire[temp]].b == 0)
                    continue;


                image[(y * s->pitch)+ (x*3)+2]  = colors[fire[temp]].b * peak;
                image[(y * s->pitch)+ (x*3)+1]  = colors[fire[temp]].g * peak;
                image[(y * s->pitch)+ (x*3)]    = colors[fire[temp]].r * peak;
            }
        }

    };
};

#endif // VISUAL_EXPLODE_H_INCLUDED
