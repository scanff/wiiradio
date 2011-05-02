#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

#define MAX_TEXTURES 500
class app_wiiradio;
class texture_cache {
    public:

    unsigned long texture_id[MAX_TEXTURES];
    SDL_Surface*  texture[MAX_TEXTURES];

    app_wiiradio*   theapp;

    texture_cache(app_wiiradio* _app);
    ~texture_cache();
    SDL_Surface* texture_lookup(const char* name);


};


#endif // TEXTURES_H_INCLUDED
