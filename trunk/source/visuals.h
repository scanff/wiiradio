#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

class visualizer
{
    public:

    fft*  f;

    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    // tunnel effect
    bool            tunnel_loaded;
    SDL_Surface*    tun_text;
    SDL_Surface*    vis_surface;
    #define texWidth 256
    #define texHeight 256
    #define DRAW_WIDTH (SCREEN_WIDTH/2)
    #define DRAW_HEIGHT (SCREEN_HEIGHT/2)

    unsigned int* texture;
    int* distanceTable;
    int angleTable[DRAW_WIDTH*2][DRAW_HEIGHT*2];

    visualizer(fft* p_f) : f(p_f), tunnel_loaded(false), texture(0), distanceTable(0)
    {
         loopi(MAX_FFT_RES) {
            fft_results[i] = 0;
            peakResults[i] = 0;
        }

        texture = new unsigned int[texWidth*texHeight];
        if (!texture) exit(0);

        distanceTable = new int[(DRAW_WIDTH*2)*(DRAW_HEIGHT*2)];
        if (!distanceTable) exit(0);

        // surface for visuals
        Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#else
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
        amask = 0x00000000;
#endif

        vis_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH,
                                          rmask, gmask, bmask,amask);

    };

    ~visualizer()
    {
        delete [] distanceTable;
        distanceTable = 0;

        delete [] texture;
        texture = 0;

        SDL_FreeSurface(vis_surface);
    };

    void draw_visuals(SDL_Surface* s,int number)
    {
        int x = 0;
        int y = SCREEN_HEIGHT - 50;
        int bar_height = 300;
        int bar_width = SCREEN_WIDTH / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);
        int sx,sy;
        int zerolevel = (SCREEN_HEIGHT/2-10);
        int len = (8192/4) - 1;
        int ii;
        double ts = static_cast<double>(SCREEN_WIDTH)/static_cast<double>(len/2);
        double range = 20;
        double timescale = zerolevel/range;

        draw_rect(vis_surface,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer

        switch(number)
        {
            case V_BARS:


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
                    draw_rect(vis_surface,x+i*bar_width,newY,bar_width-4,h,0xcc0022 +  i*10 );
                    draw_rect(vis_surface,x+i*bar_width,peaks_newy,bar_width-4,5,0xcc0022 + i*10 << 16 );
                }

                SDL_BlitSurface(vis_surface,0,s,0);

            break;

            case V_OSC:

                sx = 0;
                sy = zerolevel;
                for(ii=0; ii<len-1; ii+=1)
                {
                    x = (int)((double)ii*ts);
                    y = (int)((double)zerolevel - (short)(f->real[ii])*timescale);

                    bresenham_line(x,y,sx,sy,0xffffffff);

                    sy = y;
                    sx = x;
                }

                SDL_BlitSurface(vis_surface,0,s,0);
            break;
            case V_TUNNEL:
                tunnel_effect();
                SDL_Rect sr = {0,0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
                SDL_SoftStretch(vis_surface,&sr,s,0);
            break;


        };


		/*for(ii=0; ii<len-1; ii+=2)
		{
			x = (int)((double)ii*ts);
            y = (int)((double)100 - (short)(vis->real[ii])*timescale);

            bresenham_line(x,y,x,y-10,0xff00ffff);

            sy = y;
            sx = x;
		}*/


    };


    // tunnel effects
    void load_tunnel_text()
    {
        tun_text = tx->texture_lookup("imgs/test1.png");
        BYTE * thepixels = (BYTE*)tun_text->pixels;
        unsigned int r = 0;
        unsigned int g = 0;
        unsigned int b = 0;
        unsigned int u = 0;
        for(int x = 0; x < texWidth; x++)
        {
            for(int y = 0; y < texHeight; y++)
            {
                r = *thepixels++;
                g = *thepixels++;
                b = *thepixels++;

                u = (r << 8);
                u += (g << 16);
                u += (b << 24);
                u += 0xff; // no alpha

                texture[x+(y*texWidth)] = u;

            }
        }
        for(int x = 0; x < DRAW_WIDTH*2; x++)
        for(int y = 0; y < DRAW_HEIGHT*2; y++)
        {
            int angle, distance;
            float ratio = 32.0;

            distance = int(ratio * texHeight / sqrt(float((x - DRAW_WIDTH) * (x - DRAW_WIDTH) + (y - DRAW_HEIGHT) * (y - DRAW_HEIGHT)))) % texHeight;
            angle = (unsigned int)(0.5 * texWidth * atan2(float(y - DRAW_HEIGHT), float(x - DRAW_WIDTH)) / 3.1416);
            distanceTable[x+(y*(DRAW_WIDTH*2))] = distance;
            angleTable[x][y] = angle;

        }

        tunnel_loaded = true;
    };

    void tunnel_effect()
    {
        if (!tunnel_loaded) load_tunnel_text();

        float animation =  (get_tick_count() / 1000.0);
        int tex_width = texWidth;
        int tex_height = texHeight;

        int shiftX = int(tex_width * 1.0 * animation);
        int shiftY = int(tex_height * 0.15 * animation);


        int shiftLookX = DRAW_WIDTH / 2 + int(DRAW_WIDTH / 2 * sin(animation));
        int shiftLookY = DRAW_HEIGHT / 2 + int(DRAW_HEIGHT / 2 * sin(animation * 2.0));

        unsigned int color = 0;
        for(int x = 0; x < DRAW_WIDTH; x++)
        {
            for(int y = 0; y < DRAW_HEIGHT; y++)
            {
                color = texture[((unsigned int)(distanceTable[(x + shiftLookX)+((y + shiftLookY)*(2*DRAW_WIDTH))] + shiftX)  % texWidth)+
                               (((unsigned int)(angleTable[x + shiftLookX][y + shiftLookY]+ shiftY) % texHeight)*texWidth)];


                pixelColor(vis_surface,x,y,color);
            }
        }

    }


    void bresenham_line(int x1, int y1, int x2, int y2,unsigned long color)
    {
        // clip
        x1 > SCREEN_WIDTH ? x1 = SCREEN_WIDTH : x1 < 0 ? x1 = 0 : 0;
        x2 > SCREEN_WIDTH ? x2 = SCREEN_WIDTH : x2 < 0 ? x2 = 0 : 0;
        y1 > SCREEN_HEIGHT ? y1 = SCREEN_HEIGHT : y1 < 0 ? y1 = 0 : 0;
        y2 > SCREEN_HEIGHT ? y2 = SCREEN_HEIGHT : y2 < 0 ? y2 = 0 : 0;


        int delta_x = abs(x2 - x1) << 1;
        int delta_y = abs(y2 - y1) << 1;

        // if x1 == x2 or y1 == y2, then it does not matter what we set here
        signed char ix = x2 > x1?1:-1;
        signed char iy = y2 > y1?1:-1;

        pixelColor(vis_surface,x1,y1,color);

        if (delta_x >= delta_y)
        {
            // error may go below zero
            int error = delta_y - (delta_x >> 1);

            while (x1 != x2)
            {
                if (error >= 0)
                {
                    if (error || (ix > 0))
                    {
                        y1 += iy;
                        error -= delta_x;
                    }
                    // else do nothing
                }
                // else do nothing

                x1 += ix;
                error += delta_y;

                pixelColor(vis_surface,x1,y1,color);
            }
        }
        else
        {
            // error may go below zero
            int error = delta_x - (delta_y >> 1);

            while (y1 != y2)
            {
                if (error >= 0)
                {
                    if (error || (iy > 0))
                    {
                        x1 += ix;
                        error -= delta_y;
                    }
                    // else do nothing
                }
                // else do nothing

                y1 += iy;
                error += delta_x;

                pixelColor(vis_surface,x1,y1,color);

            }
        }
    }

};

#endif // VISUALS_H_INCLUDED