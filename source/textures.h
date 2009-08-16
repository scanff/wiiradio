#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

#define MAX_TEXTURES 50

class texture_cache {
    public:

    unsigned long texture_id[MAX_TEXTURES];
    SDL_Surface*  texture[MAX_TEXTURES];

    texture_cache()
    {
      loopi(MAX_TEXTURES) {
          texture_id[i] = 0;
          texture[i] = 0;
      }
    };
    ~texture_cache()
    {
        loopi(MAX_TEXTURES) {
            if (texture[i]) {
                SDL_FreeSurface(texture[i]);
            }
        }

    };

    SDL_Surface* texture_lookup(char* name)
    {
        unsigned long hash = 0;
        const char* ptr = name;


        while (*ptr)
            hash = (hash << 5) + hash + *ptr++;

        loopi(MAX_TEXTURES) {
            if (hash == texture_id[i])
                return texture[i]; //already loaded return surface
        }

        //find a spot to load the texture
        loopi(MAX_TEXTURES) {
            if (!texture_id[i]) {
                texture_id[i] = hash;

                texture[i] = IMG_Load(make_path(name));
                return texture[i];
            }
        }

        return 0;


    };


};


#endif // TEXTURES_H_INCLUDED
