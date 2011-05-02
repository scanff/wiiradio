/*

    I've used lots of sources to help me with these effects.  To name a few :

    http://www.student.kuleuven.be/~m0216922/CG/ (Lode's Computer Graphics Tutorial)
    and
    http://demo-effects.sourceforge.net/
*/

#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

#define SAM_SCALE ((255.0) / (16384.0*2))
#define PX_REPLACE(a,b) a = b
#define PX_MERGE(a,b) a = (((a * 0.7) + (b * 0.3)))

static const int bpp = BITDEPTH / 8;
static void FORCEINLINE FastSetPixel24(SDL_Surface * dst, const int x, const int y, const unsigned int color)
{
    if(x >= dst->w || x < 0 || y >= dst->h || y < 0)
    return;

    u8 *p = (unsigned char *) dst->pixels + (y * dst->pitch) + (x * bpp);

    p[0] = (color >> 16) & 0xff;
    p[1] = (color >> 8) & 0xff;
    p[2] = color & 0xff;


}

static void bresenham_line(SDL_Surface* s, int x1, int y1, int x2, int y2,unsigned long color)
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

   FastSetPixel24(s,x1,y1,color);

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

           FastSetPixel24(s,x1,y1,color);
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

            FastSetPixel24(s,x1,y1,color);

        }
    }
}


unsigned int inline hsl_rgba(int ia, int ib, int ic)
{
    float r, g, b, h, s, l;
    float temp1, temp2, tempr, tempg, tempb;
    static unsigned int rgb = 0;
    h = ia / 256.0;
    s = ib / 256.0;
    l = ic / 256.0;

    if(s == 0) r = g = b = l;
    else
    {
        //Set the temporary values
        if(l < 0.5) temp2 = l * (1 + s);
        else temp2 = (l + s) - (l * s);
        temp1 = 2 * l - temp2;
        tempr = h + 1.0 / 3.0;
        if(tempr > 1) tempr--;
        tempg = h;
        tempb = h - 1.0 / 3.0;
        if(tempb < 0) tempb++;

        //Red
        if(tempr < 1.0 / 6.0) r = temp1 + (temp2 - temp1) * 6.0 * tempr;
        else if(tempr < 0.5) r = temp2;
        else if(tempr < 2.0 / 3.0) r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
        else r = temp1;

        //Green
        if(tempg < 1.0 / 6.0) g = temp1 + (temp2 - temp1) * 6.0 * tempg;
        else if(tempg < 0.5) g = temp2;
        else if(tempg < 2.0 / 3.0) g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
        else g = temp1;

        //Blue
        if(tempb < 1.0 / 6.0) b = temp1 + (temp2 - temp1) * 6.0 * tempb;
        else if(tempb < 0.5) b = temp2;
        else if(tempb < 2.0 / 3.0) b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
        else b = temp1;
    }
#ifdef _WII_
    rgb = (int(r * 255.0) << 8);
    rgb += (int(g * 255.0) << 16);
    rgb += (int(b * 255.0) << 24);
    rgb += 0xff; // no alpha
#else
    rgb = (int(r * 255.0) << 24);
    rgb += (int(g * 255.0) << 16);
    rgb += (int(b * 255.0) << 8);
    rgb += 0xff; // no alpha
#endif
    return rgb;
};

inline int lmin(int a, int b)
{
  if (a <= b) return a;
  else return b;

  return a;
};

// visual class
class visual_object;
class visualizer
{
public:

    fft*            f;
    int             fft_results[MAX_FFT_RES+1];
    SDL_Surface*    vis_surface[2];
    visual_object*  visuals_ptr[MAX_VISUALS];
    void*           user_data;
    int             r1, r2;
    unsigned long   vt;
    int             mode;
    float           thecos;
    float           thesin;
    float           rads;
    app_wiiradio*   theapp;
    int xpath[512];
    int ypath[512];
    float zoom;
    int l_max;
    float dir;
    int eq;
    int look_pos;

    visualizer(app_wiiradio* _theapp);
    ~visualizer();

    const FORCEINLINE float EQ(int R, float A,int x, int y);

    void FORCEINLINE rotate_surface(SDL_Surface* s, SDL_Surface* d,
                                       const float angle,
                                       const int x_origin,
                                       const int y_origin,
                                       float z,
                                       int preset);

    void draw_visuals(SDL_Surface* s, const int number, const s16* samps);
    void FORCEINLINE borders(SDL_Surface*s,const int size, const u32 color);

};



#endif // VISUALS_H_INCLUDED
