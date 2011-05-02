#ifndef VISUAL_WAVES_H_INCLUDED
#define VISUAL_WAVES_H_INCLUDED

#include "visual_object.h"

class vis_waves : public visual_object
{
    public:

    int fulaselect;
    float xr, yr, xr2, yr2, ipdr, expan, freq, oscrate, radius, zoom, roll, pitch, yaw;
    float yawi, pitchi, rolli; // rotation speed
    float dim_color;
    int  vel, npoints, ipd, cx, cy, cz;
    bool changednpoints, eyemode, clear, frozenland, pressed, kpts, osc;
    float x3[8000];
    float y3[8000];
    float z3[8000];
    float vpd, y2, z1, x, y, z;
    float landx, landz;
    int n1, n2;
    float ix, iz, csyaw, snyaw, csyaw2, snyaw2, csroll, snroll, cspitch, snpitch, eye;
    float pi, twopi, hp;
    unsigned long startt;

    vis_waves(app_wiiradio* _theapp) : startt(0)
    {
        noclearbg = true;
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 1;


        pi = 3.14159265;
        twopi = 2 * pi;
        hp = pi * 0.5;

        fulaselect = 0;
    }

    ~vis_waves()
    {
    }

    void load(void* user_data)
    {


        if(loaded) return;

        // Some defaults

        eye = 200;
        cx = DRAW_WIDTH / 2;
        cy = DRAW_HEIGHT / 2;
        ipd = 92;
        zoom = 50;
        rolli = 0;
        yawi = 0.023;
        pitchi = 0;
        ipdr = 8 / 13;
        npoints = 6000;
        expan = 1;
        freq = 0.1;
        vel = 100;

        loaded = true;
    };


    void render(SDL_Surface* s,void* userdata, const int max)
    {
        load(userdata);

        double percent = ((double)1 / (double)32767);
        int peak = (int)(percent * (double)fft_results[2]);

        float vfactor = (rand() % 10) * 0.001;
        int n = 0;
        freq = 0.07f;
        if ((theapp->app_timer - startt) > 10000)
        {
            fulaselect = (rand() % 7);
            startt = theapp->app_timer;
            vfactor = (rand() % 10) * 0.001;
            expan = 1.20f;
           // freq = 0.07f;//0.031015;
            zoom = 160 / 2;
            frozenland = false;
            radius = zoom + 5;
            npoints = 6000;
            vel = 100;//(3000.0 / f->getPeak()) * 100;//100;
            yaw = 0.0f;
            pitch = 0.0f;
            roll = 0.0f;
            // Modify for rotation speed
            rolli =  peak;
            yawi = 0.023 + peak;
            pitchi = - peak;
        }

        for(float ix = -expan; ix < expan;ix+=freq * expan)
        {

            for(float iz = -expan; iz < expan; iz+= freq * expan)
            {

                x3[n] = ix / expan;
                z3[n] = iz / expan;

                //'These are some example formulas
//fulaselect=0;
                switch(fulaselect)
                {
                    case 0: y3[n] = sin(ix + landx + 4.0f * sin(cos(landx + iz))); break;
                    case 1: y3[n] = 0.15f * sin(3.0f * pow(ix,2.0f) + 3.0f * pow(iz,2.0f) + 0.014f + landz); break;
                    case 2: y3[n] = 0.23f * sin(ix * 3.0f + landz) + 0.23 * sin(iz * 3.0f + landx); break;
                    case 3: y3[n] = cos(sqrt(pow(ix,2.0f) + pow(iz,2.0f)) + landx) * (-(pow(ix,2.0f) + pow(iz,2.0f)) + 1.0f); break;
                    case 4: y3[n] = sin(pow(ix,2.0f) + landx) / (0.134f + iz) + sin(iz + (iz * sin(landx))); break;
                    case 5: y3[n] = 0.4f * cos((ix + cos(landx + 2.0f * iz + 2.0f))) + 0.4f * sin(2.0f * iz * ix + 1.1f + landx); break;
                    case 6: y3[n] = vfactor / sin(ix * (vfactor*100.0f) + landz) + vfactor * 1.0f / cos(iz * (vfactor*100.0f) + landx + vfactor); break;
                    case 7: y3[n] = vfactor / sin(ix * (vfactor*100.0f) + landz);

                                      // case 7: y3[n] = 1 / (cos(3 * pow(ix,2) + landx) + sin(pow(iz,3) + 1)); break;
                }


                y3[n] *= radius;
                x3[n] *= radius;
                z3[n] *= radius;

                if (n >= npoints) break;
                n++;
            }
            if (n >= npoints) break;
        }
        npoints = n - 1;


        // freeze
        if(!frozenland)
        {

            landx = landx + 0.13 * vel * 0.01;
            landz = landz + 0.32 * vel * 0.01;

            if(landx > twopi) landx = landx - twopi;
            if(landz > twopi) landz = landz - twopi;
        }


        ipd = zoom / ipdr;
        snyaw = sin(yaw);
        csyaw = cos(yaw);
        snpitch = sin(pitch);
        cspitch = cos(pitch);
        snroll = sin(roll);
        csroll = cos(roll);

        // render it
        int lx, ly;
        lx = cx;
        ly = cy;
        float distance = 0.5 * (radius + eye);
        unsigned int color = 0;

        for(int n = 0; n < npoints; n++)
        {
            x = x3[n] * cspitch + y3[n] * snpitch;
            y = -x3[n] * snpitch + y3[n] * cspitch;
            y *= csroll + z3[n] * snroll;
            z1 = -y * snroll + z3[n] * csroll;
            z = -x * snyaw + z1 * csyaw;

            //'only draw poins within given distance to and from eye
            if(z < 100 && z > -100)
            {

                dim_color = (-z + 100) / distance;
                vpd = eye / (eye - z);//  'vanishing-point distortion
                x = vpd * (x * csyaw + z1 * snyaw);
                y = vpd * y;


                // screen co-ords
                x = x + cx;
                y = y + cy;

                color = hsl_rgba(dim_color,200,200); // get the color

                FastSetPixel24(s,x,y,color);


            }

            lx = x;
            ly = y;
        }


        // Rotation Speed
        yaw += yawi;
        pitch += pitchi;
        roll += rolli;

        // Yaw Speed
        if(yaw > 2 * pi) yaw = yaw - 2 * pi;
        if(yaw < 0) yaw = yaw + 2 * pi;

        // Roll Speed
        if(roll > 2 * pi) roll = roll - 2 * pi;
        if (roll < 0) roll = roll + 2 * pi;

        // Pitch Speed
        if(pitch > 2 * pi) pitch = pitch - 2 * pi;
        if(pitch < 0) pitch = pitch + 2 * pi;


    };


};

#endif //
