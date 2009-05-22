#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

class visualizer
{
    public:

    fft*  f;

    int             peakResults[MAX_FFT_RES];
    int             fft_results[MAX_FFT_RES];

    visualizer(fft* p_f) : f(p_f)
    {
         loopi(MAX_FFT_RES) {
            fft_results[i] = 0;
            peakResults[i] = 0;
        }

    };

    ~visualizer() {};

    void draw_visuals(SDL_Surface* s,int number)
    {
        int x = 0;
        int y = SCREEN_HEIGHT - 50;
        int bar_height = 300;
        int bar_width = SCREEN_WIDTH / MAX_FFT_RES;
        double percent = ((double)bar_height / (double)32767);
        int local_fft[MAX_FFT_RES];
        int sx,sy;
        int leftzerolevel = (SCREEN_HEIGHT/2-10)/2;
//        int rightzerolevel = SCREEN_HEIGHT/2+10+leftzerolevel;
        int len = (8192/4) - 1;
        int ii;
        double ts = static_cast<double>(SCREEN_WIDTH)/static_cast<double>(len/2);
//        int max = 20;
 //       int min = 0;
        double dRangePerStep = 20;
        double timescale = leftzerolevel/dRangePerStep;

        switch(number)
        {
            case V_BARS:

            loopi(MAX_FFT_RES) local_fft[i] = fft_results[i]; //could change as this is set via callback
            loopi(MAX_FFT_RES)
            {
                // peak
                peakResults[i] -= (32767 / bar_height) * 4;

                if(peakResults[i] < 0) peakResults[i] = 0;
                if(peakResults[i] < local_fft[i]) peakResults[i] = local_fft[i];

                int peaks_newy = (y - (int)(percent * (double)peakResults[i]));

                //fft
                int newY = (y - (int)(percent * (double)local_fft[i]));
                int h = abs(y - newY);



                //draw
                draw_rect(s,x+i*bar_width,newY,bar_width-4,h,0xcc0022 +  i*10 );
                draw_rect(s,x+i*bar_width,peaks_newy,bar_width-4,5,0xcc0022 + i*10 << 16 );
            }
            break;

            case V_OSC:

                sx = 0;
                sy = leftzerolevel;
                for(ii=20; ii<len-1; ii+=1)
                {
                    x = (int)((double)ii*ts);
                    y = (int)((double)leftzerolevel - (short)(f->real[ii])*timescale);

                    bresenham_line(s,x,y,sx,sy,0xffffffff);

                    sy = y;
                    sx = x;
                }


            break;

        };

		/*sx = 0;
		sy = rightzerolevel;

		for(ii=0; ii<len; ii++)
		{
			x= (int)(ii*ts);
			y = (int)((double)rightzerolevel - (short)(vis->real[ii])*timescale);

			bresenham_line(sx,sy,x,y,0xffffffff);
            sy = y;
            sx = x;
		}*/

		//

		/*for(ii=0; ii<len-1; ii+=2)
		{
			x = (int)((double)ii*ts);
            y = (int)((double)100 - (short)(vis->real[ii])*timescale);

            bresenham_line(x,y,x,y-10,0xff00ffff);

            sy = y;
            sx = x;
		}*/


    };



    void bresenham_line(SDL_Surface* s,int x1, int y1, int x2, int y2,unsigned long color)
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

        pixelColor(s,x1,y1,color);

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

                pixelColor(s,x1,y1,color);
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

                pixelColor(s,x1,y1,color);

            }
        }
    }

};

#endif // VISUALS_H_INCLUDED
