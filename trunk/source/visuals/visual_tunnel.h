#ifndef VISUAL_TUNNEL_H_INCLUDED
#define VISUAL_TUNNEL_H_INCLUDED

#include "visual_object.h"

class vis_tunnel : public visual_object
{
    public:

    #define TDIVIDER    (2)
    #define texWidth    (256)
    #define texHeight   (256)

    unsigned int* texture;
    int* distanceTable;
    int angleTable[(SCREEN_WIDTH/TDIVIDER)*2][(SCREEN_HEIGHT/TDIVIDER)*2];

    vis_tunnel(fft* _f) : texture(0), distanceTable(0)
    {
        loaded = false;
        f = _f;
        layer = 1;

        DRAW_WIDTH = SCREEN_WIDTH / TDIVIDER;
        DRAW_HEIGHT = SCREEN_HEIGHT / TDIVIDER;

        texture = (unsigned int*)memalign(32,texWidth*texHeight*sizeof(int));
        if (!texture) exit(0);

        distanceTable = (int*)memalign(32,((DRAW_WIDTH*2)*(DRAW_HEIGHT*2))*sizeof(int));
        if (!distanceTable) exit(0);

        srand(get_tick_count());


    };

    ~vis_tunnel()
    {
        free(distanceTable);
        distanceTable = 0;

        free(texture);
        texture = 0;

    };

      // tunnel effects
    void load(void* user_data)
    {
        int x,y;

        int peak = f->getPeak();

        //clean
         for(x = 0; x < texWidth; x++)
            for(y = 0; y < texHeight; y++)
                texture[x+(y*texWidth)] = 0;

        unsigned int color = 0;
        unsigned int* pixels = 0;

        if (user_data) pixels = (unsigned int*)user_data;

        for(x = 0; x < texWidth; x++)
        {
            for(y = 0; y < texHeight; y++)
            {
                 if (user_data) color = *(pixels+=3);
                 else color = hsl_rgba((x / 3), lmin(255,(int)(peak * 1.8)), lmin(100, x * 2));


                 if (!user_data)
                 {
                    if (rand()% 255 > 245) texture[x+(y*texWidth)] = color;
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
                float ratio = 2.0;

                distance = int(ratio * texHeight / sqrt(float((x - DRAW_WIDTH) * (x - DRAW_WIDTH) + (y - DRAW_HEIGHT) * (y - DRAW_HEIGHT)))) % texHeight;
                angle = (unsigned int)(0.5 * texWidth * atan2(float(y - DRAW_HEIGHT), float(x - DRAW_WIDTH)) / 3.1416);
                distanceTable[x+(y*(DRAW_WIDTH*2))] = distance;
                angleTable[x][y] = angle;

            }
        }
        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {

        load(userdata);

        fade(s,SDL_MapRGB(s->format,0,0,0),50);

        float animation =  (get_tick_count() / 1000.0);
        int tex_width = texWidth;
        int tex_height = texHeight;

        int shiftX = int(tex_width * 1.0 * animation);
        int shiftY = int(tex_height * 0.05 * animation);


        int shiftLookX = DRAW_WIDTH / 2 + int(DRAW_WIDTH / 2 * sin(animation));
        int shiftLookY = DRAW_HEIGHT / 2 + int(DRAW_HEIGHT / 2 * sin(animation * 2.0));

        unsigned int color = 0;
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            for(int y = 0; y < DRAW_HEIGHT; y++)
            {

                long p = ((unsigned int)(distanceTable[(x + shiftLookX)+((y + shiftLookY)*(2*DRAW_WIDTH))] + shiftX)  % texWidth)+
                               (((unsigned int)(angleTable[x + shiftLookX][y + shiftLookY]+ shiftY) % texHeight)*texWidth);

                if (p < 0 || p >= (((SCREEN_WIDTH/TDIVIDER)*2)*((SCREEN_WIDTH/TDIVIDER)*2))) continue;

                color = texture[p];

                pixelColor(s,x,y,color);
            }
        }

    }
};

#endif // VISUAL_TUNNEL_H_INCLUDED
