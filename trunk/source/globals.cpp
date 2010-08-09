#include "globals.h"
#include "options.h"

// --- anything in this file has global scope !!!!!!

int         max_listings;
int         g_reloading_skin; // flag to see if we are loading a new skin!

int         volume;
bool        visualize;
int         visualize_number;
bool        mute;
int            g_nGlobalStatus;
Uint8 *        g_nKetStatus;
Uint8       g_real_keys[MAX_KEYS];
Uint8       g_keys_last_state[MAX_KEYS];
bool        g_running;
Uint64      g_vol_lasttime;
enum_status status;
audio_source playback_type = AS_SHOUTCAST;
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

/* Screen status access */
int current_screen_status = 0;
int last_screen_status = 0;
void SetScreenStatus(int);
void SetScreenStatus(int s)
{
    last_screen_status = current_screen_status;
    current_screen_status = s;
};

int GetScreenStatus();
int GetScreenStatus()
{
    return current_screen_status;
};

void SetLastScreenStatus();
void SetLastScreenStatus()
{
    current_screen_status = last_screen_status;
};

// extern functions / vars
char* make_path(const char*);
Uint64 get_tick_count();
bool refresh_genre_cache;
bool sc_error;
int  g_stream_source;

#ifndef _WIN32

#include <sys/statvfs.h>

/* number of bytes free in current working directory */
long get_media_free_space()
{
    long free = 0;
    /* Thanks teknecal for this info */
    struct statvfs fiData;

    if (statvfs(".", &fiData) < 0) {
        return -1;
    }
    else {
        free = (fiData.f_bfree * fiData.f_bsize);
    }

    return free;
}
#else
long get_media_free_space() { return -1; }
#endif

/* Human-readable desription of free space in working directory */
string get_media_free_space_desc()
{
    char tmp[SMALL_MEM];
    string unit;
    ostringstream ret;
    double bytes = get_media_free_space();
    short expon = 0;
    if (bytes < 0)
        return "unknown";
    while (bytes > 1024 && expon <= 4) {
        bytes /= 1024;
        expon++;
    }
    switch (expon) {
      case 0: unit =   "B"; break;
      case 1: unit = "kiB"; break;
      case 2: unit = "MiB"; break;
      case 3: unit = "GiB"; break;
      case 4: unit = "TiB"; break;
      default: return "unknown";
    }
    if (bytes < 10)
        snprintf(tmp, SMALL_MEM-1, "%.1f ", bytes);
    else
        snprintf(tmp, SMALL_MEM-1, "%.0f ", bytes);
    ret << tmp << unit;
    return ret.str();
}

