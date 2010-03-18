#include "globals.h"
#include "options.h"

// --- anything in this file has global scope !!!!!!

int         X_OFFSET;
int         Y_OFFSET;
int         max_listings;
int         g_reloading_skin; // flag to see if we are loading a new skin!

int         mp3_volume;
bool        visualize;
int         visualize_number;
bool        mute;
int            g_nGlobalStatus;
Uint8 *        g_nKetStatus;
Uint8       g_real_keys[MAX_KEYS];
Uint8       g_keys_last_state[MAX_KEYS];
bool        g_running;
int         g_screen_status;
Uint64      g_vol_lasttime;
enum_status status;

wiiradio_variables vars;

// -- Functions

void next_lang()
{
    lang->goto_next_lang();
    lang->load_lang(lang->current_name);
    strcpy(g_currentlang,lang->current_name);
    // sorry have to reload skin right now as all button text needs updating
    g_reloading_skin = true;
    g_running = false;
}

void next_skin()
{
    sk->goto_next_skin();
    g_reloading_skin = true;
    g_running = false;
}

void fade(SDL_Surface *screen2, Uint32 rgb, Uint8 a)
{
    SDL_Surface *tmp=0;

    tmp=SDL_DisplayFormat(screen2);

    if (!tmp) return;

    SDL_FillRect(tmp,0,rgb);
    SDL_SetAlpha(tmp,SDL_SRCALPHA,a);
    SDL_BlitSurface(tmp,0,screen2,0);
    SDL_FreeSurface(tmp);
};
Uint64 get_tick_count()
{
    return SDL_GetTicks();
}

char g_storage_media[8]; // extern'd
char* make_path(const char* path_rel)
{
#ifdef _WII_
    static char abs_path[SMALL_MEM] = {0};
    sprintf(abs_path,"%s:/apps/radiow/%s",g_storage_media,path_rel);

    return abs_path;
#else
    return (char*)path_rel;
#endif

}
char* trim_string(char*, int);
char* trim_string(char* s, int n_len)
{
    int c_len = strlen(s);
    static char new_string[256] = {0};

    if (n_len > 256) return s;

    if(c_len > n_len)
    {
        memcpy(new_string,s,n_len);
        return new_string;
    }

    return s;
}

char* make_string(char*,...);
char* make_string(char* s ,...)
{
    static char buffer[256] = {0};

    va_list args;
    va_start (args, s);
    vsprintf (buffer,s, args);
    va_end (args);

    return buffer;

}

void draw_rect(SDL_Surface*,int,int,int,int,unsigned long);
void draw_rect(SDL_Surface* s,int x,int y, int w, int h,unsigned long color)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.h = h;
    r.w = w;
    SDL_FillRect(s, &r, color);
};

#ifdef _WII_
// Will need latest SVN version of SDL as of 3/18/10
extern "C" { extern void WII_SetWidescreen(int wide); }
#endif

// Widescreen option
void SetWidescreen();
void SetWidescreen()
{
#ifdef _WII_

	// Will need latest SVN version of SDL as of 3/16/10

    WII_SetWidescreen(g_owidescreen);

#endif
};

// extern functions / vars
char* make_path(const char*);
Uint64 get_tick_count();
bool refresh_genre_cache;
bool sc_error;
bool g_pause_draw;
int  g_stream_source;
