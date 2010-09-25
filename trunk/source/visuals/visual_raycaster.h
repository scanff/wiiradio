#ifndef VISUAL_RAYCASTER_H_INCLUDED
#define VISUAL_RAYCASTER_H_INCLUDED

/*

Core Raycaster code (http://www.student.kuleuven.be/~m0216922/CG/raycasting.html):-

Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Heavily modified for WiiRadio and SDL By Scanff

*/

#include "visual_object.h"
#define mapWidth 24
#define mapHeight 24

static const int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,2,1,2,1,0,0,0,0,2,0,2,0,2,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,3,0,0,0,2,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,2,1,2,1,0,0,0,0,3,0,2,0,2,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,2,1,2,1,2,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,2,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,2,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
class vis_raycaster : public visual_object
{
    public:

    #define TDIVIDER    (2)

    double posX, posY;
    double dirX,dirY ;
    double planeX, planeY;
    int direction; // currrent direction
    unsigned long startt;
    unsigned long time_use_tex;

    enum
    {
        RCS_WALL1 = 0,
        RCS_FLOOR,
        RCS_MAX
    };

    SDL_Surface* rc_surfaces[RCS_MAX];
    SDL_Surface* bartexture;

    bool use_texture;

    vis_raycaster(app_wiiradio*   _theapp) :  use_texture(true)
    {
        theapp = _theapp;
        loaded = false;
        f = theapp->fourier;
        layer = 1;

        loopi(RCS_MAX) rc_surfaces[i] = 0;

        DRAW_WIDTH = SCREEN_WIDTH;// / TDIVIDER;
        DRAW_HEIGHT = SCREEN_HEIGHT;// / TDIVIDER;

        srand(get_tick_count());

        posX = 22; posY = 12;  //x and y start position
        dirX = -1; dirY = 0; //initial direction vector
        planeX = 0; planeY = 0.66; //the 2d raycaster version of camera plane


        bartexture = SDL_CreateRGBSurface(SDL_SWSURFACE, 128,128, BITDEPTH, rmask, gmask, bmask, amask);

     };

    ~vis_raycaster()
    {
        loopi(RCS_MAX)
        {
            if (rc_surfaces[i])
            {
                SDL_FreeSurface(rc_surfaces[i]);
                rc_surfaces[i] = 0;
            }
        }

        SDL_FreeSurface(bartexture);
        bartexture = 0;

    };

    void create_bar_texture()
    {
        draw_rect(bartexture,0,0,128,128,0); // clear backbuffer

        int x = 0;
        int y = 128;
        int bar_height = 100;
        int bar_width = int(128 / MAX_FFT_RES);
        double percent = ((double)bar_height / (double)32767);
        int newY;
        int h;

        loopi(MAX_FFT_RES)
        {
            newY = (y - (int)(percent * (double)fft_results[i]));
            h = abs(y - newY);
            draw_rect(bartexture,x+i*bar_width,newY,bar_width-1,h,0x0000c5);
        }

    }


    void load(void* user_data)
    {
        if (!rc_surfaces[RCS_WALL1]) rc_surfaces[RCS_WALL1] = IMG_Load(make_path("visdata/wall.png"));
        if (!rc_surfaces[RCS_FLOOR]) rc_surfaces[RCS_FLOOR] = IMG_Load(make_path("visdata/floor.png"));

        if (loaded) return;
        time_use_tex = startt = get_tick_count();
        loaded = true;
    };

    void render(SDL_Surface* s,void* userdata)
    {
        load(userdata);

        if ((get_tick_count() - startt) > 500)
        {
            direction = (rand() % 3);
            startt = get_tick_count();

        }
        if ((get_tick_count() - time_use_tex) > 50000)
        {
            time_use_tex = get_tick_count();
            use_texture = !use_texture;
        }

        //use_texture = true;
        //double percent = ((double)200 / (double)32767);
        int peak = f->getPeak();



    for(int x = 0; x < DRAW_WIDTH; x++)
    {
        // calculate ray position and direction
        double cameraX = 2 * x / double(DRAW_WIDTH) - 1; //x-coordinate in camera space
        double rayPosX = posX;
        double rayPosY = posY;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        //which box of the map we're in

        int mapX = int(rayPosX);
        int mapY = int(rayPosY);

        if(mapX >= mapWidth || mapX < 0 || mapY >= mapHeight || mapY < 0)
            continue;

        //length of ray from current position to next x or y-side
        float sideDistX;
        float sideDistY;

       //length of ray from one x or y-side to next x or y-side
        float deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
        float deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
        float perpWallDist;

        //what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; //was there a wall hit?
        int side; //was a NS or a EW wall hit?

        //calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (rayPosX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (rayPosY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
        }

        //perform DDA
        while (hit == 0)
        {
            //jump to next map square, OR in x-direction, OR in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if(mapX >= mapWidth || mapX < 0 || mapY >= mapHeight || mapY < 0)
                continue;

            //Check if ray has hit a wall
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
        if (side == 0)
        {
            perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
        }
        else
        {
            perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
        }

        // Calculate height of line to draw on screen
        int lineHeight = abs(int(DRAW_HEIGHT / perpWallDist));

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + DRAW_HEIGHT/ 2;
        if(drawStart < 0) drawStart = 0;

        int drawEnd = lineHeight / 2 + DRAW_HEIGHT / 2;
        if(drawEnd >= DRAW_HEIGHT) drawEnd = DRAW_HEIGHT - 1;

        unsigned int color;
        int texture_id = RCS_WALL1;

        unsigned char* texture;
        int map_pos_id = 0;
        if ((map_pos_id = worldMap[mapX % mapWidth][mapY % mapHeight]) != 1)
        {
            texture = (unsigned char*)bartexture->pixels;
            texture_id = RCS_WALL1;
        }
        else
        {
            texture = (unsigned char*)rc_surfaces[RCS_WALL1]->pixels;
            texture_id = RCS_WALL1;
        }


        if (!use_texture)
        {

            switch(worldMap[mapX % mapWidth][mapY % mapHeight])
            {
                case 1:  color = hsl_rgba(100,100,lmin(peak,255));  break;
                case 2:  color = hsl_rgba(25,200,lmin(peak,255));  break;
                case 3:  color = hsl_rgba(80,100,lmin(peak,255));   break;
                case 4:  color = hsl_rgba(200,200,lmin(peak,255));  break;
                default: color = hsl_rgba(200,100,lmin(peak,255)); break;
            }

            //give x and y sides different brightness
            if (side == 1) {color = color / 2;}

            bresenham_line(s, x, drawStart,x,drawEnd,color);

        }
        else
        { // use a texture
            unsigned char* buffer = (unsigned char*)s->pixels;
            unsigned char r,b,g;
            unsigned int surface_pos;
            unsigned int text_pos;

            create_bar_texture();

            float wallX; //where exactly the wall was hit
            if (side == 1) wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
            else           wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
            wallX -= floor((wallX));

            int texX = int(wallX * double(rc_surfaces[texture_id]->w));
            if(side == 0 && rayDirX > 0) texX = rc_surfaces[texture_id]->w - texX - 1;
            if(side == 1 && rayDirY < 0) texX = rc_surfaces[texture_id]->w - texX - 1;

            for(int y = drawStart; y < drawEnd; y++)
            {
                int d = y * 256 - DRAW_HEIGHT * 128 + lineHeight * 128;  //256 and 128 factors to avoid floats
                int texY = ((d * rc_surfaces[texture_id]->h) / lineHeight) / 256;


                surface_pos = ((rc_surfaces[texture_id]->h*3) * texY) + (texX*3);
                if ((signed)surface_pos+2 >= (rc_surfaces[texture_id]->h * rc_surfaces[texture_id]->pitch))
                    continue;

                r = texture[surface_pos+2];
                g = texture[surface_pos+1];
                b = texture[surface_pos];

                if (map_pos_id == 1)
                {
                    r = lmin(255,r+peak);
                    g = lmin(255,g+peak);
                    b = lmin(255,b+peak);
                }

        #ifdef _WII_
                color = r << 16 | g << 8 | b;
        #else
                color = b << 16 | g << 8 | r;
        #endif

                // darker if futher away
                //color *= perpWallDist;

                //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
                if(side == 1) color = (color >> 1) & 8355711;

                if (y < 0 || x < 0 || x >= s->w || y >= s->h)
                    continue;

                text_pos = ((y*(s->w*3))+(x*3));

                SDL_LockSurface (s) ;
                //pixelColor(s,x,y,color);

                buffer[text_pos] = color & 0xff;
                buffer[text_pos+1] = color >> 8;
                buffer[text_pos+2] = color >> 16;

                SDL_UnlockSurface (s);

            }
            // only if textured
            cast_floor_ceiling(s,x,side,mapX,mapY,rayDirX,rayDirY,wallX,perpWallDist,drawEnd);
        }




    }


    double moveSpeed = 2 * 0.5;
    double rotSpeed = 2 * 0.3;
    double oldDirX = dirX;
    double oldPlaneX = planeX;

    switch(direction)
    {
        case 0:
        {
            if(worldMap[int(posX + dirX * moveSpeed) %mapWidth][int(posY) % mapHeight] == false) posX += dirX * moveSpeed;
            if(worldMap[int(posX) % mapWidth][int(posY + dirY * moveSpeed)% mapHeight] == false) posY += dirY * moveSpeed;
        }
        break;
        case 1:
        {
            if(worldMap[int(posX - dirX * moveSpeed)% mapWidth][int(posY)% mapHeight] == false) posX -= dirX * moveSpeed;
            if(worldMap[int(posX)% mapWidth][int(posY - dirY * moveSpeed)% mapHeight] == false) posY -= dirY * moveSpeed;
        }
        break;
        case 2:
        {
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        break;
        case 3:
        {
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
        default:
        break;
    }

    }

    // Ray cast the floor and ceilings
    void cast_floor_ceiling(SDL_Surface*s, int x, int side,int mapx, int mapy,float rayDirX,float rayDirY,float wallX,
                            float perpWallDist, int drawEnd)
    {
        double floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall
        double distWall, distPlayer, currentDist;
        unsigned int color;
        unsigned char* texture = (unsigned char*)rc_surfaces[RCS_FLOOR]->pixels;
        unsigned char* buffer = (unsigned char*)s->pixels;
        unsigned char r,b,g;
        int surface_pos;


        //4 different wall directions possible
        if(side == 0 && rayDirX > 0)
        {
            floorXWall = mapx;
            floorYWall = mapy + wallX;
        }
        else if(side == 0 && rayDirX < 0)
        {
            floorXWall = mapx + 1.0;
            floorYWall = mapy + wallX;
        }
        else if(side == 1 && rayDirY > 0)
        {
            floorXWall = mapx + wallX;
            floorYWall = mapy;
        }
        else
        {
            floorXWall = mapx + wallX;
            floorYWall = mapy + 1.0;
        }

        distWall = perpWallDist;
        distPlayer = 0.0;

        if (drawEnd < 0) drawEnd = DRAW_HEIGHT; //becomes < 0 when the integer overflows

      //draw the floor from drawEnd to the bottom of the screen
      for(int y = drawEnd + 1; y < DRAW_HEIGHT; y++)
      {
        currentDist = DRAW_HEIGHT / (2.0 * y - DRAW_HEIGHT); //you could make a small lookup table for this instead

        double weight = (currentDist - distPlayer) / (distWall - distPlayer);

        double currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
        double currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

        int floorTexX, floorTexY;
        floorTexX = int(currentFloorX * rc_surfaces[RCS_FLOOR]->w) % rc_surfaces[RCS_FLOOR]->w;
        floorTexY = int(currentFloorY * rc_surfaces[RCS_FLOOR]->h) % rc_surfaces[RCS_FLOOR]->h;

        surface_pos = ((rc_surfaces[RCS_FLOOR]->w*3) * floorTexY) + (floorTexX*3);
        //surface_pos = ((eye_surface->w * floorTexY) + (floorTexX*3)) ;

        r = lmin(255,texture[surface_pos+2]);
        g = lmin(255,texture[surface_pos+1]);
        b = lmin(255,texture[surface_pos]);

        #ifdef _WII_
                color = r << 16 | g << 8 | b;
        #else
                color = b << 16 | g << 8 | r;
        #endif

        int text_pos_floor = ((y*(s->w*3))+(x*3));
        //int text_pos_ceiling = (((DRAW_HEIGHT-y)*(s->w*3))+(x*3));

        color = color >> 1 & 8355711; // darken


        //floor
        buffer[text_pos_floor] = color & 0xff;
        buffer[text_pos_floor+1] = color >> 8;
        buffer[text_pos_floor+2] = color >> 16;
        //buffer[text_pos_floor] = color >> 1  & 8355711;
        // ceiling
   //     buffer[text_pos_ceiling] = color & 0xff;
   //     buffer[text_pos_ceiling+1] = color >> 8;
    //    buffer[text_pos_ceiling+2] = color >> 16;//

        //2buffer[x][h - y] = color;//texture[6][texWidth * floorTexY + floorTexX];
      }


    }
};

#endif
