#include "globals.h"
#include "application.h"
#include "visuals.h"

#include "visuals/visual_water.h"
#include "visuals/visual_tunnel.h"
#include "visuals/visual_fire.h"
#include "visuals/visual_osc.h"
#include "visuals/visual_bars.h"
#include "visuals/visual_explode.h"
#include "visuals/visual_mist.h"
#include "visuals/visual_circles.h"
#include "visuals/visual_plasma.h"
#include "visuals/visual_sintext.h"
#include "visuals/visual_rotzoom.h"
#include "visuals/visual_bobs.h"
#include "visuals/visual_lasers.h"
#include "visuals/visual_raycaster.h"
#include "visuals/visual_matrix.h"
#include "visuals/visual_stars.h"
#include "visuals/visual_waves.h"
#include "visuals/visual_object.h"


visualizer::visualizer(app_wiiradio* _theapp) : mode(1), theapp(_theapp)
{
    r1 = r2 = vt = 0;
    loopi(MAX_FFT_RES)
    {
        fft_results[i] = 0;
    }

    loopi(MAX_VISUALS) visuals_ptr[i] = 0;



    vis_surface[0] = SDL_CreateRGBSurface(SDL_SWSURFACE,DRAW_WIDTH,DRAW_HEIGHT,BITDEPTH,
                                      rmask, gmask, bmask,amask);

    vis_surface[1] = SDL_CreateRGBSurface(SDL_SWSURFACE,DRAW_WIDTH,DRAW_HEIGHT,BITDEPTH,
                                      rmask, gmask, bmask,amask);

    SDL_SetColorKey(vis_surface[1], SDL_SRCCOLORKEY, SDL_MapRGB(vis_surface[1]->format, 0, 0, 0));
    SDL_SetColorKey(vis_surface[0], SDL_SRCCOLORKEY, SDL_MapRGB(vis_surface[0]->format, 0, 0, 0));
    // create all visuals
    visuals_ptr[V_BARS]     = new vis_bars(theapp);
    visuals_ptr[V_OSC]      = new vis_osc(theapp);
    visuals_ptr[V_TUNNEL]   = new vis_tunnel(theapp);
    visuals_ptr[V_FIRE]     = new vis_fire(theapp);
    visuals_ptr[V_MIST]     = new vis_mist(theapp);
    visuals_ptr[V_CIRCLES]  = new vis_circles(theapp);
    visuals_ptr[V_PLASMA]   = new vis_plasma(theapp);
    visuals_ptr[V_SINTEXT]  = new vis_sintext(theapp);
    visuals_ptr[V_ROTZOOM]  = new vis_rotzoom(theapp);
    visuals_ptr[V_BOBS]     = new vis_bobs(theapp);
    visuals_ptr[V_CIRCULAR]   = new vis_cicular(theapp);
    visuals_ptr[V_RAYCASTER]= new vis_raycaster(theapp);
    visuals_ptr[V_MATRIX]   = new vis_matrix(theapp);
    visuals_ptr[V_STARS]    = new vis_stars(theapp);
    visuals_ptr[V_WATER]    = new vis_water(theapp);
    visuals_ptr[V_EXPLODE]  = new vis_explode(theapp);
    visuals_ptr[V_WAVES]    = new vis_waves(theapp);


    srand ( get_tick_count() );

    rads = 0.0;
    zoom = 1.0f;
    l_max = 0;
    dir = 0.01;
    eq = 0;
    look_pos = 0;

    // setup the look coords
    const int hw = (DRAW_WIDTH >> 1);
    const int hh = (DRAW_HEIGHT >> 1);

    for(int i=0;i<512;i++)
    {
        double rad = ((float)i * 0.703125) * 0.0174532;
        xpath[i] = (int)(sin(rad) * hw + hw);
        ypath[i] = (int)(cos(rad) * hh + hh);
    }


}

visualizer::~visualizer()
{
    SDL_FreeSurface(vis_surface[0]);
    SDL_FreeSurface(vis_surface[1]);

    loopi(MAX_VISUALS)
    {
        if (visuals_ptr[i])
        {
            delete visuals_ptr[i];
            visuals_ptr[i] = 0;
        }
    }
}


const FORCEINLINE float visualizer::EQ(int R, float A,int x, int y)
{
    switch(R)
    {
        case 0:
        return sin(A);
        case 1:
        return (float)sin(1 + 10 + 4.0f * sin(cos(A)));
        case 2:
        return  (0.15f * sin(A * pow(DRAW_WIDTH,2.0f) + 3.0f * pow(0,2.0f) + 0.014f + 6.0));
        case 3:
        return cos(A) * sin(A);
        case 4:
        return sin(pow(100.0,2.0f) + A) / (0.134f + 10.0) + sin(10.0 + (10.0f * sin(A)));
        case 5:
        return cos(A)*2.0;
        case 6:
        return sin(((float)x * 0.703125) * 0.0174532) / 1024;
        case 7:
        return sin(((float)y * 0.703125) / 0.0174532);
        case 8: return cos(A) * 0.5;

    }
    return 0.0;
}

void FORCEINLINE visualizer::rotate_surface(SDL_Surface* s, SDL_Surface* d,
                                       const float angle,
                                       const int x_origin,
                                       const int y_origin,
                                       float z,
                                       int preset)
{
    const int num = 8;

    const int h = s->h;
    const int w = s->w;

    const int w_4 = (w / (num >> 1));
    const int h_4 = (h / (num >> 1));

    int col = 0;
    int row = 0;
    int sourcex = 0;
    int sourcey = 0;

    unsigned char* pixels_in = (unsigned char*)s->pixels;
    unsigned char* pixels_out = (unsigned char*)d->pixels;

    for(row = 0 ; row < num >> 1; row ++)
    {

        for(col = 0 ; col < num >> 1; col ++)
        {

            for( int y = row * h_4; y < (row * h_4) + h_4; y++ )
            {
                for( int x = col * w_4; x < (col * w_4) + w_4; x++ )
                {

                    sourcex =  x_origin+(int)((x-x_origin)*thecos + (y-y_origin)*thesin);
                    sourcey =  y_origin+(int)((y-y_origin)*thecos - (x-x_origin)*thesin);

                    const int xpoint =  x * 3;
                    const int ypoint = (y * s->w) * 3;

                    sourcex = abs(sourcex % s->w-1);
                    sourcey = abs(sourcey % s->h-1);
/*
                    PX_MERGE(pixels_out[xpoint+ypoint],pixels_in[((sourcey*s->h*3)+(sourcex*3))] );
                    PX_MERGE(pixels_out[xpoint+ypoint+1],pixels_in[(sourcey*s->h*3)+(sourcex*3)+1]);
                    PX_MERGE(pixels_out[xpoint+ypoint+2],pixels_in[(sourcey*s->h*3)+(sourcex*3)+2]);
*/

                    const int source_pos = ((sourcey*s->h*3)+(sourcex*3));

                    PX_REPLACE(pixels_out[xpoint+ypoint],pixels_in[source_pos] );
                    PX_REPLACE(pixels_out[xpoint+ypoint+1],pixels_in[source_pos+1]);
                    PX_REPLACE(pixels_out[xpoint+ypoint+2],pixels_in[source_pos+2]);

                }
            }

        }
    }
}
/*
void stretch(SDL_Surface* s,SDL_Surface* d)
{
    float facx = ((float)s->w / (float)d->w);
    float facy = ((float)s->h / (float)d->h);

    int sx, sy, dx, dy;

    u8* pixels_in = (u8*)s->pixels;
    u8* pixels_out = (u8*)d->pixels;
    u32 c = 0;

    for(int y = 0; y < d->h; y++)
    {
        for(int x = 0; x < d->w; x++)
        {
                sx =  (int)(((float)x * facx) * 3);
                sy =  (((y * facy) * s->w) * 3);

                //dx =  (x * 3);
                //dy =  (y * d->w) * 3;

                c = (pixels_in[sx+sy]  << 16 | pixels_in[sx+sy+1]  << 8 |  pixels_in[sx+sy+2]);

                FastSetPixel24(d,x,y,c);



        }
    }

}*/

void visualizer::draw_visuals(SDL_Surface* s,const int number, const s16* samps)
{
   if (theapp->keys_buttons.current_keys[KEY_1] && !theapp->keys_buttons.last_keys[KEY_1])
   {
       mode++;
       if (mode > 2) mode = 0;
   }

    int a_max = 0;

    loopi(MAX_FFT_SAMPLE/2)
    {
        if (samps[i] > (u16)a_max)
            a_max = samps[i];
    }



    int c_max = SAM_SCALE * a_max;
    int num_of_visuals = 0; // number of visuals we are showing

    if (mode != 0)
    {
        if(mode == 2)
        {
            fade(vis_surface[1],20);
            SDL_BlitSurface(vis_surface[1],0,vis_surface[0],0);
        }

        num_of_visuals = 2;
        if ((theapp->app_timer - vt) > 15000)
        {
            zoom = 1.0f;
            rads = 0.0;
            eq = rand() % 8;
            r1 = rand() % MAX_VISUALS;


            while(1)
            {
                r2 = (rand() % MAX_VISUALS);
                // only some effects on top and not the same effect as r1
                if( !visuals_ptr[r2]->layer || (r2 == r1)) continue;

                break;
            }
            vt = theapp->app_timer;
        }
    }else{
        num_of_visuals = 1;
        r1 = number;
    }


    loopj(num_of_visuals)
    {
        void* udata = 0;
        int v =0;

        j==0?v=r1:v=r2;

        loopi(MAX_FFT_RES)  visuals_ptr[v]->fft_results[i] =  fft_results[i];

        if (j > 0) SDL_BlitSurface(s,0,vis_surface[0],0);

        if (j > 0 && v == V_TUNNEL) udata = (void*)vis_surface[0]->pixels;
        if(v == V_SINTEXT) udata = user_data;

        visuals_ptr[v]->mode = mode;
        visuals_ptr[v]->render(vis_surface[0],udata,a_max);


        SDL_Surface* vs = vis_surface[0];

        if (mode == 2)
        {

            if (look_pos < 511) look_pos++;
            else look_pos = 0;


            rads += 0.1;
            if (c_max > 100)
            {
                rads += ((float)c_max / 100.0);//0.5;
            }

            if (c_max > 115) dir = dir - (dir*1.5);

            zoom += dir;


            thecos = (float)cos(rads);
            thesin = EQ(eq,rads,zoom,thecos) * zoom;
            rotate_surface(vis_surface[0],vis_surface[1],rads,xpath[look_pos],ypath[look_pos],1.0,1);
            vs= vis_surface[1];
           // if (c_max > 100) borders(vis_surface[0],rand()%4,hsl_rgba(c_max,210,c_max));

        }
        SDL_Rect sr = {0,0,DRAW_WIDTH,DRAW_HEIGHT};
        SDL_Rect dr = {0,0,s->w,s->h};

        fade(vis_surface[0],14);

        SDL_SoftStretch(vs,&sr,s,&dr);
    }

    l_max = c_max;
}


void inline visualizer::borders(SDL_Surface*s,const int size, const u32 color)
{
    const int h = s->h;
    const int w = s->w;

    for( int y = 0; y < h; y++ )
    {
        for( int x = 0; x < w; x++ )
        {
            if( x < size || x > (w - size) || y < size || y > (h - size))
            {
                FastSetPixel24(s,x, y, color);
            }
        }
    }
}

