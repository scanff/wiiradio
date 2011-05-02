#include "globals.h"
#include "application.h"
#include "textures.h"

texture_cache::texture_cache(app_wiiradio* _app) :
        theapp( _app)
{
  loopi(MAX_TEXTURES) {
      texture_id[i] = 0;
      texture[i] = 0;
  }
}
texture_cache::~texture_cache()
{
    loopi(MAX_TEXTURES) {
        if (texture[i]) {
            SDL_FreeSurface(texture[i]);
        }
    }

}
SDL_Surface* texture_cache::texture_lookup(const char* name)
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

          /*  SDL_Surface* tmp = IMG_Load(make_path(name));
            if(!tmp) return NULL;

            texture[i] = SDL_ConvertSurface(tmp,theapp->screen->format, SDL_SWSURFACE);

            SDL_FreeSurface(tmp);

            tmp = texture[i];
*/
            texture[i] = IMG_Load(make_path(name));
            return texture[i];
        }
    }

    return 0;
}

