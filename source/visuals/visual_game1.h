/*
 A very simple shooting game to demo the ability of the visuals plugin...

 By Scanff .....
*/

#ifndef VISUAL_GAME1_H_INCLUDED
#define VISUAL_GAME1_H_INCLUDED
SDL_Surface*    spritemap;
int             DRAW_WIDTH;
int             DRAW_HEIGHT;
class vis_game1 {
    public:

    fft*            f;
    bool            loaded;
    bool            game_over;
    bool            game_pause;

    unsigned long   h_score;



    #define MAX_STARS       100
    #define MAX_BADGUY      5
    #define MAX_BG_AMMO     5
    #define MAX_PLY_AMMO    20


    vis_game1(fft* _f) : f(_f), loaded(false),game_over(true),game_pause(false), h_score(0)
    {
        DRAW_HEIGHT = 480;
        DRAW_WIDTH = 640;
        game_pause = false;
        srand(get_tick_count());
        spritemap = 0;
        spritemap = IMG_Load(make_path((char*)"visdata/game1/spritemap.png"));
        if (!spritemap) exit(0);
    };

    ~vis_game1()
    {
        if (spritemap) SDL_FreeSurface(spritemap);
    };

    struct _stars {

        int x;
        int y;
        int speed;
        int color;

        void init(SDL_Surface* s)
        {
            x = (rand() % DRAW_WIDTH);
            y = (rand() % DRAW_HEIGHT);
            speed = (rand() % 10) + 5;

            color = 0x333333ff * speed;

            pixelColor(s,x,y,color);
        };

        void move(SDL_Surface* s)
        {
           // pixelColor(s,x,y,0x0);

            y += speed;
            if (y >= DRAW_HEIGHT) y = 0;

            pixelColor(s,x,y,color);
        };


    } starfield[MAX_STARS];

    struct _bullets
    {
        int x;
        int y;
        bool active;
        int direction;
       // SDL_Surface* spritemap;

        _bullets() : active(false){}

        void move(SDL_Surface* s)
        {
            y += direction;

            SDL_Rect rs = {0,0,0,0};
            SDL_Rect ds = {0,0,0,0};
            rs.x = 48*3;
            rs.y = 0;
            rs.w = 5;
            rs.h = 8;

            ds.x = x;
            ds.y = y;
            ds.w = rs.w;
            ds.h = rs.h;

            SDL_BlitSurface(spritemap,&rs,s,&ds);

            if(y < 0 || y > DRAW_HEIGHT) active = false;
        };

    };

    struct _badguy {

        float x;
        float y;
        int speed;
        bool alive;
        int type;
        int explode;
        int w;
        int h;
        float z;

        _bullets ammo[MAX_BG_AMMO];

        _badguy() : explode(0) {};

        void init()
        {
            explode = 0;
            type = (rand() % 3);
#ifdef _WII_
            speed = (rand() % 2) + 4;
#else
            speed = (rand() % 2) + 1;
#endif
            float a = (rand() % 360);//3.14159);

            x = (rand() % DRAW_WIDTH);
            y = -(rand() % 100);
            z = 0;

            if (type == 1)
            {
                w = 48;
                h = 48;
            }else if (type == 0){
                h = 20;
                w = 48;
                speed += 2; // these move faster
            }else if(type == 2){
                h = 22;
                w = 36;
            }

            if (x < DRAW_WIDTH / 2) z = (float)(speed * sin( a ));
            else z = (float)-(speed * sin( a ));

            alive = true;

        };

        void fire_gun()
        {
            loopi(MAX_BG_AMMO)
            {
                if(!ammo[i].active)
                {
                    ammo[i].x = (int)(x)+(w/2);
                    ammo[i].y = (int)(y)+(h+1);
                    ammo[i].direction = 10;
                    ammo[i].active = true;

                    break;
                }

            }


        };

        void move(SDL_Surface* s)
        {

            SDL_Rect rs = {0,0,0,0};
            SDL_Rect ds = {0,0,0,0};

            loopi(MAX_BG_AMMO)
            {
                if(ammo[i].active) ammo[i].move(s);
            }

            if (explode)
            {
                rs.x = 48;
                rs.y = 0;
                rs.w = 48;
                rs.h = 48;

                ds.x = (int)(x);
                ds.y = (int)(y);
                ds.w = rs.w;
                ds.h = rs.h;

                SDL_BlitSurface(spritemap,&rs,s,&ds);
                explode--;
            }else{

                if (type == 1){
                    y += speed;
                    x += z;
                    rs.x = 96;
                    rs.y = 0;

                }else if(type == 0){
                    y += speed;
                    x += z;
                    rs.x = 0;
                    rs.y = 48;

                }else if(type == 2) {
                    y += speed;
                    x += z;
                    rs.x = 48;
                    rs.y = 48;
                }

                if (y >= DRAW_HEIGHT || y < -200 ||
                    x < -200 || x > DRAW_WIDTH + 200 ||
                    !alive
                    )
                {
                    init();
                    return;
                }

                if ((rand() % 1000) < 20 && (int)(y) > 0) fire_gun();


                rs.w = w;
                rs.h = h;

                ds.x = (int)(x);
                ds.y = (int)(y);
                ds.w = rs.w;
                ds.h = rs.h;

                SDL_BlitSurface(spritemap,&rs,s,&ds);
            }
        };

    }badguy[MAX_BADGUY];





    struct _player {
        int w;
        int h;
        int x;
        int y;

        bool alive;
        int direction_x;
        int direction_y;
        int explode;

        _bullets ammo[MAX_PLY_AMMO];

        unsigned long score;

        _player() : w(48), h(48), x((DRAW_WIDTH / 2) + (w/2)), y(DRAW_HEIGHT-100), alive(true),
                    direction_x(0), direction_y(0), explode(0), score(0) {};

        void init()
        {
            alive = true;
            x = ((DRAW_WIDTH / 2) + (w/2));
            y = (DRAW_HEIGHT-100);
            loopi(MAX_PLY_AMMO) ammo[i].active = false;
            score = explode = direction_x = direction_y = 0;
        };

        void move(SDL_Surface* s)
        {
            SDL_Rect rs = {0,0,0,0};
            SDL_Rect ds = {0,0,0,0};

            if (explode)
            {
                rs.x = 48;
                rs.y = 0;;
                rs.w = 48;
                rs.h = 48;

                ds.x = x;
                ds.y = y;
                ds.w = rs.w;
                ds.h = rs.h;

                SDL_BlitSurface(spritemap,&rs,s,&ds);
                explode--;
            }else{
#ifdef _WII_
                if (g_real_keys[SDLK_UP]) direction_x = -6;
                if (g_real_keys[SDLK_DOWN]) direction_x = 6;
                if (g_real_keys[SDLK_RIGHT]) direction_y = -6;
                if (g_real_keys[SDLK_LEFT]) direction_y = 6;
#else
                if (g_real_keys[SDLK_LEFT]) direction_x = -6;
                if (g_real_keys[SDLK_RIGHT]) direction_x = 6;
                if (g_real_keys[SDLK_UP]) direction_y = -6;
                if (g_real_keys[SDLK_DOWN]) direction_y = 6;

#endif
                if (alive) {
                    x+=direction_x;
                    y+=direction_y;
                }

                x > DRAW_WIDTH - w ? x=DRAW_WIDTH - w : x < 0 ? x = 0 : 0;
                y > DRAW_HEIGHT - h ? y = DRAW_HEIGHT - h : y < h ? y = h : 0;

                rs.x = 0;
                rs.y = 0;
                rs.w = w;
                rs.h = h;

                ds.x = x;
                ds.y = y;
                ds.w = rs.w;
                ds.h = rs.h;

                SDL_BlitSurface(spritemap,&rs,s,&ds);

                if (alive)
                {

                    if (g_real_keys[SDLK_1] && !g_keys_last_state[SDLK_1])
                        fire_gun();

                    loopi(MAX_PLY_AMMO)
                    {
                        if(ammo[i].active) ammo[i].move(s);
                    }
                }

            }
            direction_y = direction_x = 0;
        };



        void fire_gun()
        {
            loopi(MAX_PLY_AMMO)
            {
                if(!ammo[i].active)
                {
                    ammo[i].x = x+(w/2);
                    ammo[i].y = y - 8;
                    ammo[i].direction = -10;
                    ammo[i].active = true;

                    break;
                }

            }

        }
    } p1;


    bool rect_in_rect(SDL_Rect& r1, SDL_Rect& r2)
    {    // -- simple rect collision ... not doing pixel on pixel
         return ! ( r2.x > r1.x+r1.w
                    || r2.x+r2.w < r1.x
                    || r2.y > r1.y+r1.h
                    || r2.y+r2.h < r1.y
                );
    }

    void do_text(SDL_Surface* s, bool paused = false)
    {
        fnts->change_color(150,150,50);
        fnts->set_size(FS_SYSTEM);
        fnts->text(s,(char*)"\"A totally pointless game plugin for WiiRadio\"",
                (DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"\"A totally pointless game plugin for WiiRadio\"",FS_SYSTEM) /2),
                100,0);

        fnts->change_color(150,50,50);
        fnts->text(s,(char*)"Hopefully it provides you some entertainment while listening to good music!",
                (DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"Hopefully it provides you some entertainment while listening to good music!",FS_SYSTEM) /2),
                150,0);

        fnts->change_color(150,150,150);
        fnts->set_size(FS_SYSTEM);
        fnts->text(s,(char*)"D-Pad = Move",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"D-Pad = Move",FS_SYSTEM) /2),240,0);
        fnts->text(s,(char*)"(1) = Fire",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"(1) = Fire",FS_SYSTEM) /2),280,0);

        if (!paused) {
            fnts->text(s,(char*)"Game Over Press (B) To Start",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"Game Over Press (B) To Start",FS_SYSTEM) /2),200,0);



        }else{
            fnts->text(s,(char*)"Game Paused Press (B) To Continue",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"Game Paused Press (B) To Continue",FS_SYSTEM) /2),200,0);
        }
        fnts->set_size(FS_SYSTEM);
        fnts->change_color(50,200,50);
        fnts->text(s,make_string((char*)"High Score: %u",h_score),10,410,0);
        fnts->change_color(150,50,50);
        fnts->text(s,(char*)"Press (D-Right) for next Visual/Plugin",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"Press (D-Right) for next Visual/Plugin",FS_SYSTEM) /2),330,0);
        fnts->text(s,(char*)"Press (D-Left) for previous Visual/Plugin",(DRAW_WIDTH / 2) - (fnts->get_length_px((char*)"Press (D-Left) for previous Visual/Plugin",FS_SYSTEM) /2),350,0);
    };

    bool render(SDL_Surface* s)
    {
        if (!loaded)
        {
            loopi(MAX_STARS) starfield[i].init(s);
            loopi(MAX_BADGUY) badguy[i].init();

            loaded = true;
        }

        // game loop
        loopi(MAX_STARS) starfield[i].move(s);

        if (game_over)
        {
            do_text(s);

            if (g_real_keys[SDLK_b])
            {
                loaded = false;
                game_over = false;

                p1.init();
            }

            return false; // don't steal key map

        }else{

            if (g_real_keys[SDLK_b] && !g_keys_last_state[SDLK_b])
            {
                !game_pause ? game_pause = true : game_pause = false;
            }

            if (game_pause)
            {
                do_text(s,true);

                return false;
            }else{


                // -- game draw, collisions

                if (!p1.alive && !p1.explode) game_over = true;

                loopi(MAX_BADGUY) badguy[i].move(s);
                p1.move(s);

                collide(s);

                  // - score

                fnts->set_size(FS_SYSTEM);
                fnts->change_color(50,200,50);
                fnts->text(s,make_string((char*)"Score: %u",p1.score),10,410,0);

            }//pause
        }
             return true;
        };

        // -- collisions
    void collide(SDL_Surface* s)
    {

        SDL_Rect r1,r2;

        // --player hit by ammo ?
        if (p1.alive)
        {

            loopi(MAX_BADGUY)
            {
                loopj(MAX_BG_AMMO)
                {
                    if (!badguy[i].ammo[j].active) continue;

                    r1.x = badguy[i].ammo[j].x;
                    r1.y = badguy[i].ammo[j].y;
                    r1.w = 6;
                    r1.h = 8;

                    r2.x = p1.x;
                    r2.y = p1.y;
                    r2.w = r2.h = 48;
                    if (rect_in_rect(r1,r2))
                    {
                        badguy[i].ammo[j].active = false;
                        p1.alive = false;
                        p1.explode = 30;
                        break;
                    }
                }
                if (!p1.alive) break;
            }

            // -- bad guy collide w/ player
            loopi(MAX_BADGUY)
            {
                if (!badguy[i].alive) continue;

                r2.x = (int)(badguy[i].x);
                r2.y = (int)(badguy[i].y);
                badguy[i].type ? r2.h = 48 : r2.h = 20;
                r2.w = 48;
                // debuging hit rects
                //rectangleColor(s,r2.x,r2.y,r2.x+r2.w,r2.y+r2.h,0xffffffff);

                r1.x = p1.x;
                r1.y = p1.y;
                r1.w = r1.h = 48;

                // debuging hit rects
                //rectangleColor(s,r1.x,r1.y,r1.x+r1.w,r1.y+r1.h,0xffffffff);

                if (rect_in_rect(r1,r2))
                {
                    badguy[i].alive = false;
                    badguy[i].explode = 10;
                    p1.alive = false;
                    p1.explode = 30;
                    p1.x = (int)(badguy[i].x);
                    p1.y = (int)(badguy[i].y);

                    break;
                }

            }

            // player hit bad guy?
            loopj(MAX_PLY_AMMO)
            {
                if (!p1.ammo[j].active) continue;

                r1.x = p1.ammo[j].x;
                r1.y = p1.ammo[j].y;
                r1.w = 5;
                r1.h = 8;

                loopi(MAX_BADGUY)
                {
                    if (!badguy[i].alive) continue;

                    r2.x = (int)(badguy[i].x);
                    r2.y = (int)(badguy[i].y);
                    r2.h = badguy[i].h;
                    r2.w = badguy[i].w;
                    if (rect_in_rect(r1,r2))
                    {
                        badguy[i].type ? p1.score +=5 : p1.score +=7;
                        if (p1.score > h_score) h_score = p1.score;

                        badguy[i].alive = false;
                        badguy[i].explode = 10;
                        p1.ammo[j].active = false;
                    }
                }

            }
        }

        // collision .... player bullets vrs badguy bullets
        loopi(MAX_BADGUY)
        {
            loopj(MAX_BG_AMMO)
            {
                if (!badguy[i].ammo[j].active) continue;

                r1.x = badguy[i].ammo[j].x;
                r1.y = badguy[i].ammo[j].y;
                r1.w = 5;
                r1.h = 8;

                loopk(MAX_PLY_AMMO)
                {
                    r2.x = p1.ammo[k].x;
                    r2.y = p1.ammo[k].y;
                    r2.w = 5;
                    r2.h = 8;

                    if (rect_in_rect(r1,r2))
                    {
                        badguy[i].ammo[j].active = false;
                        p1.ammo[k].active = false;
                        break;
                    }
                }
            }
        }

    };
};

#endif
