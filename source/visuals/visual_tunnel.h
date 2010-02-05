#ifndef VISUAL_TUNNEL_H_INCLUDED
#define VISUAL_TUNNEL_H_INCLUDED

class vis_tunnel {
    public:

    fft*            f;
    bool            tunnel_loaded;

    #define texWidth 256
    #define texHeight 256
    int             DRAW_WIDTH;
    int             DRAW_HEIGHT;

    unsigned int* texture;
    int* distanceTable;
    int angleTable[(SCREEN_WIDTH/2)*2][(SCREEN_HEIGHT/2)*2];

    vis_tunnel(fft* _f) : f(_f), tunnel_loaded(false), texture(0), distanceTable(0)
    {
        DRAW_WIDTH = SCREEN_WIDTH / 2;
        DRAW_HEIGHT = SCREEN_HEIGHT / 2;

        texture = new unsigned int[texWidth*texHeight];
        if (!texture) exit(0);

        distanceTable = new int[(DRAW_WIDTH*2)*(DRAW_HEIGHT*2)];
        if (!distanceTable) exit(0);

        srand(get_tick_count());


    };

    ~vis_tunnel()
    {
        delete [] distanceTable;
        distanceTable = 0;

        delete [] texture;
        texture = 0;

    };

      // tunnel effects
    void load_tunnel_text()
    {
        int x,y;

        //get peek
        int peak = 0;
        loopi((MAX_FFT_SAMPLE) - 1) f->real[i] > peak ? peak = (int)f->real[i] : 0;



        //clean
         for(x = 0; x < texWidth; x++)
            for(y = 0; y < texHeight; y++)
                texture[x+(y*texWidth)] = 0;


        unsigned int color = 0;
        for(x = 0; x < texWidth; x++)
        {
            for(y = 0; y < texHeight; y++)
            {
                // if ((y < (texHeight/2) -10 || y > (texHeight/2) +10) &&
                //    (x< (texWidth/2)-10) || x > (texWidth/2)+10)
                    color = hsl_rgba((x / 3), lmin(255,(int)(peak * 1.8)), lmin(100, x * 2));
                // else color = 0;
                 /*if (   y < 10 || x < 10 ||  x > texWidth - 10 ||
                        y > texHeight - 10 || ((y > (texHeight/2)- 10)&& (y <(texHeight/2)+ 10))) texture[x+(y*texWidth)] = color;
                 else  */
                 if (rand()% 255 > 245) texture[x+(y*texWidth)] = color;
            }

        }
        if(!tunnel_loaded)
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
        tunnel_loaded = true;
    };

    void render(SDL_Surface* s)
    {


        load_tunnel_text();

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

                color = texture[((unsigned int)(distanceTable[(x + shiftLookX)+((y + shiftLookY)*(2*DRAW_WIDTH))] + shiftX)  % texWidth)+
                               (((unsigned int)(angleTable[x + shiftLookX][y + shiftLookY]+ shiftY) % texHeight)*texWidth)];


                pixelColor(s,x,y,color);
            }
        }

    }
};

#endif // VISUAL_TUNNEL_H_INCLUDED
