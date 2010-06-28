#ifndef VISUAL_WATER_H_INCLUDED
#define VISUAL_WATER_H_INCLUDED

#include "visual_object.h"
/*
    Original example of water effect from www.darwin3d.com/gamedev.htm.

    Modified and updated for WiiRadio by Scanff.
*/
class visualizer;
class vis_water : public visual_object
{
    public:

    #define WATER_SIZE	256

    private:
    // useful macros
   void   SETBUFFER(char* xbuf,int x,int y,char vl)
   {
       xbuf[(y * WATER_SIZE) + x] = vl;
   }

    char READBUFFER(char* xbuf,int x,int y)
    {
        return (xbuf[(y * WATER_SIZE) + x]);
    }

    SDL_Surface*    watertexture;

    int             drip_radius;
    int             drip_radius_sqr;
    char*           water_read_buf;
    char*           water_write_buf;
    float           damp_fact;


    public:

    vis_water(fft* _f) :
        watertexture(0),
        drip_radius(12),
        drip_radius_sqr(drip_radius*drip_radius),
        damp_fact(0.04),
        water_read_buf(0),
        water_write_buf(0)
    {
        f = _f;
        loaded = false;
        layer = 0;

        DRAW_WIDTH = WATER_SIZE;
        DRAW_HEIGHT = WATER_SIZE;

        watertexture = IMG_Load(make_path("visdata/water.png"));

        water_read_buf = (char*)memalign(32,WATER_SIZE * WATER_SIZE);
        water_write_buf = (char*)memalign(32,WATER_SIZE * WATER_SIZE);

        memset(water_read_buf,0,WATER_SIZE * WATER_SIZE);
        memset(water_write_buf,0,WATER_SIZE * WATER_SIZE);



    };

    ~vis_water()
    {
        free(water_write_buf);
        free(water_read_buf);


        if (watertexture)
        {
            SDL_FreeSurface(watertexture);
            watertexture = 0;
        }
    };

    void load(void* userdata)
    {
        loaded = true;
    };
    void render(SDL_Surface* is,void* user_data)
    {
        //load(user_data);
       // double percent = ((double)18 / (double)32767);
       // int peak = (int)(percent * (double)fft_results[3]);

        for(int d=0; d < 4;d++)
            rain_drops((rand() % WATER_SIZE),(rand() % WATER_SIZE),16);

        process_water();

        // main buffer pixel set
        char	*temp;
        int		r,g,b,xoff,yoff;
        int     cnt = 0;

        temp = this->water_read_buf;
        this->water_read_buf = this->water_write_buf;
        this->water_write_buf = temp;

        for (int jj = 0; jj < WATER_SIZE; jj++)
        {
            for (int ii = 0; ii < WATER_SIZE; ii++, cnt++)
            {
                xoff = ii;
                if (ii > 0 && ii < WATER_SIZE - 1)
                {
                    xoff -= (this->water_read_buf[cnt - 1]);
                    xoff += (this->water_read_buf[cnt + 1]);
                }

                yoff = jj;
                if (jj > 0 && jj < WATER_SIZE - 1)
                {
                    yoff -= this->water_read_buf[cnt - WATER_SIZE];
                    yoff += this->water_read_buf[cnt + WATER_SIZE];
                }

                if (xoff < 0) xoff = 0;
                if (yoff < 0) yoff = 0;
                if (xoff >= this->watertexture->w) xoff = this->watertexture->w - 1;
                if (yoff >= this->watertexture->h) yoff = this->watertexture->h - 1;

                unsigned char* image_data_water = (unsigned char*)this->watertexture->pixels;


                r = image_data_water[(yoff*this->watertexture->pitch) + (xoff*3)];
                g = image_data_water[(yoff*this->watertexture->pitch) + (xoff*3)+1];
                b = image_data_water[(yoff*this->watertexture->pitch) + (xoff*3)+2];

                r += this->water_read_buf[cnt];
                g += this->water_read_buf[cnt];
                b += this->water_read_buf[cnt];

                if (r < 0) r = 0;
                if (g < 0) g = 0;
                if (b < 0) b = 0;
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;

                unsigned char	*buffer_water;
                buffer_water = (unsigned char*)is->pixels;
#ifdef _WII_
                buffer_water[jj*is->pitch+(ii*3)] = r;
                buffer_water[jj*is->pitch+(ii*3)+1] = g;
                buffer_water[jj*is->pitch+(ii*3)+2] = b;
#else
                buffer_water[jj*is->pitch+(ii*3)] = b;
                buffer_water[jj*is->pitch+(ii*3)+1] = g;
                buffer_water[jj*is->pitch+(ii*3)+2] = r;
#endif


            }
        }

    };

    int sqrdt(int sx, int sy, int dx, int dy)
    {
        return ((dx - sx) * (dx - sx)) + ((dy - sy) * (dy - sy));
    }

    void rain_drops(int x, int y, int depth)
    {
        int ii,jj,dist,finaldepth;

        for (jj = y - drip_radius; jj < y + drip_radius; jj++)
        {
            for (ii = x - drip_radius; ii < x + drip_radius; ii++)
            {
                if (ii < WATER_SIZE && jj < WATER_SIZE && ii >= 0 && jj >=0)
                {
                    dist = sqrdt(x,y,ii,jj);
                    if (dist < drip_radius_sqr)
                    {
                        finaldepth = (int)((float)depth * ((float)(drip_radius - sqrt((double)dist))/(float)drip_radius));
                        if (finaldepth > 127) finaldepth = 127;
                        if (finaldepth < -127) finaldepth = -127;
                        SETBUFFER(water_write_buf,ii,jj,(char)finaldepth);
                    }
                }
            }
        }
    };

    void process_water()
    {
        int ii,jj;
        float water_value;

        for (jj = 2; jj < WATER_SIZE - 2; jj++)
        {
            for (ii = 2; ii < WATER_SIZE - 2; ii++)
            {
                water_value = (float)(
                    READBUFFER(water_read_buf,ii-2,jj) +
                    READBUFFER(water_read_buf,ii+2,jj) +
                    READBUFFER(water_read_buf,ii,jj-2) +
                    READBUFFER(water_read_buf,ii,jj+2) +
                    READBUFFER(water_read_buf,ii-1,jj) +
                    READBUFFER(water_read_buf,ii+1,jj) +
                    READBUFFER(water_read_buf,ii,jj-1) +
                    READBUFFER(water_read_buf,ii,jj+1) +
                    READBUFFER(water_read_buf,ii-1,jj-1) +
                    READBUFFER(water_read_buf,ii+1,jj-1) +
                    READBUFFER(water_read_buf,ii-1,jj+1) +
                    READBUFFER(water_read_buf,ii+1,jj+1));

                water_value /= 6.0f;
                water_value -= (float)READBUFFER(water_write_buf,ii,jj);
                water_value -= (water_value * damp_fact);

                SETBUFFER(water_write_buf,ii,jj,(char)water_value);
            }
        }
    }

};
#endif
