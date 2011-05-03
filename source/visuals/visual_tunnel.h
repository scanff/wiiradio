#ifndef VISUAL_TUNNEL_H_INCLUDED
#define VISUAL_TUNNEL_H_INCLUDED

#include "visual_object.h"
#include "../fft.h"
class vis_tunnel : public visual_object
{
    public:

    struct _color_24{
        u8 r, g, b;
    };

   // _color_24* c24;
    #define TDIVIDER    (2)
    #define texWidth    (DRAW_WIDTH)
    #define texHeight   (DRAW_HEIGHT)

    _color_24* texture;
    int* distanceTable;
    int angleTable[DRAW_WIDTH*2][DRAW_HEIGHT*2];
    SDL_Surface* tex_surface;

    vis_tunnel(app_wiiradio*   _theapp) : texture(0), distanceTable(0)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 1;

        tex_surface = IMG_Load(make_path("data/visdata/1.png"));

        texture = (_color_24*)malloc(DRAW_WIDTH*DRAW_HEIGHT*sizeof(_color_24));
        if (!texture) exit(0);

        distanceTable = (int*)malloc(((DRAW_WIDTH*2)*(DRAW_HEIGHT*2))*sizeof(int));
        if (!distanceTable) exit(0);

        srand(get_tick_count());


    };

    ~vis_tunnel()
    {
        free(distanceTable);
        distanceTable = 0;

        free(texture);
        texture = 0;

        SDL_FreeSurface(tex_surface);

    };

      // tunnel effects
    void load(void* user_data)
    {
        int x,y;

        int peak = (int)(samp_max*SAM_SCALE);

        //clean
         for(x = 0; x < texWidth; x++)
         {
            for(y = 0; y < texHeight; y++)
            {
                texture[x+(y*texWidth)].r = 0;
                texture[x+(y*texWidth)].g = 0;
                texture[x+(y*texWidth)].b = 0;
            }
         }
        _color_24 color;
        _color_24* c24;
        _color_24 cc24;
        float fac = 50.0 / peak;

        if (user_data) c24 = (_color_24*)user_data;
        else c24 = (_color_24*)tex_surface->pixels;



        for(x = 0; x < texWidth; x++)
        {
            for(y = 0; y < texHeight; y++)
            {
                cc24.r = c24->r;
                cc24.g = c24->g;
                cc24.b = c24->b;

                cc24.r *= fac; cc24.g *= fac; cc24.b *=fac;
                color = cc24;//(cc24.r << 16 | cc24.g << 8 | cc24.b);
                 //if (user_data) color = (u32)(cc24.r << 16 | cc24.g << 8 | cc24.b);
                 //else color = //hsl_rgba((x / 3), lmin(255,(int)(peak * 1.8)), lmin(100, x * 2));

                 c24++;

                 if (!user_data)
                 {
                    if (rand()% 255 > 100) texture[x+(y*texWidth)] = color;
                 }
                 else texture[x+(y*texWidth)] = color;
            }

        }
        if(!loaded)
        {
            for(x = 0; x < DRAW_WIDTH*2; x++)
            for(y = 0; y < DRAW_HEIGHT*2; y++)
            {
                int angle, distance;
                float ratio = 10.0;

                distance = int(ratio * texHeight / sqrt(float((x - DRAW_WIDTH) * (x - DRAW_WIDTH) + (y - DRAW_HEIGHT) * (y - DRAW_HEIGHT)))) % texHeight;
                angle = (unsigned int)(0.5 * texWidth * atan2(float(y - DRAW_HEIGHT), float(x - DRAW_WIDTH)) / 3.1416);
                distanceTable[x+(y*(DRAW_WIDTH*2))] = distance;
                angleTable[x][y] = angle;

            }
        }
        loaded = true;
    };

    void render(SDL_Surface* s,void* user_data, const int max)
    {
        visual_object::render(s,user_data,max);

        load(user_data);

        float animation =  (theapp->app_timer / 1000.0);
        int tex_width = texWidth;
        int tex_height = texHeight;

        int shiftX = int(tex_width * 1.0 * animation);
        int shiftY = int(tex_height * 0.05 * animation);


        int shiftLookX = DRAW_WIDTH / 2 + int(DRAW_WIDTH / 2 * sin(animation));
        int shiftLookY = DRAW_HEIGHT / 2 + int(DRAW_HEIGHT / 2 * sin(animation * 2.0));

        u32 color = 0;
        int mx = (DRAW_HEIGHT<<1)*(DRAW_WIDTH<<1);

        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            for(int y = 0; y < DRAW_HEIGHT; y++)
            {

                s32 p = ((u32)(distanceTable[(x + shiftLookX)+((y + shiftLookY)*(2*DRAW_WIDTH))] + shiftX)  % texWidth)+
                        (((u32)(angleTable[x + shiftLookX][y + shiftLookY]+ shiftY) % texHeight)*texWidth);

                if (p < 0 || p >= mx ) continue;

                color = (texture[p].r << 16 | texture[p].g << 8 | texture[p].b);

                FastSetPixel24(s,x,y,color);
            }
        }

    }
};

#endif // VISUAL_TUNNEL_H_INCLUDED
