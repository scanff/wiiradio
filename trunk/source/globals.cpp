#include "globals.h"
#include "options.h"

// --- anything in this file has global scope !!!!!!

int         max_listings;
bool        visualize;
int            g_nGlobalStatus;
Uint8 *        g_nKetStatus;

bool        g_running;

//enum_status status;
audio_source playback_type = AS_NULL;

// -- Functions


// this is slow
void fade(SDL_Surface *s, const int a)
{
    const float fact =  (float)(100.0 - a) / 100.0;
    const int h = s->h;
    const int w = s->w;
    unsigned char* px = (unsigned char*)s->pixels;
    int pos = 0;

    for( int y = 0; y < h; ++y )
    {
        for( int x = 0; x < w; ++x )
        {
            pos = (x * 3) + (y * s->w) * 3;

            px[pos] *= fact;
            px[pos+1] *= fact;
            px[pos+2] *= fact;
        }
    }

}

u64 get_tick_count()
{
    return SDL_GetTicks();
}

char g_storage_media[8]; // extern'd
const char* make_path(const char* path_rel)
{
#ifdef _WII_
    static char abs_path[SMALL_MEM] = {0};
    sprintf(abs_path,"%s:/apps/radiow/%s",g_storage_media,path_rel);

    return abs_path;
#else
    return (char*)path_rel;
#endif

}


void stdstring_trim( string& str)
{
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");

    if(( string::npos == startpos ) || ( string::npos == endpos))
        str = "";
    else
        str = str.substr( startpos, endpos-startpos+1 );
}

const char* trim_string(char* s, int n_len)
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

const char* make_string(char*,...);
const char* make_string(char* s ,...)
{
    static char buffer[MED_MEM];

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

void draw_rect_rgb(SDL_Surface*,const int x,const int y,
                   const int w, const int h,const int r,
                   const int g, const int b);

void draw_rect_rgb(SDL_Surface* s,const int x,const int y,
                   const int w, const int h,const int r,
                   const int g, const int b)
{
    SDL_Rect rc;
    rc.x = x;
    rc.y = y;
    rc.h = h;
    rc.w = w;
    SDL_FillRect(s, &rc, SDL_MapRGB(s->format,r,g,b));
}

// extern functions / vars
const char* make_path(const char*);
u64 get_tick_count();
bool sc_error;
int  g_stream_source;

#ifndef _WIN32

#include <sys/statvfs.h>

/* number of bytes free in current working directory */
u64 get_media_free_space()
{
    u64 free = 0;
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
u64 get_media_free_space()
{
    u64 free = 0;
    u64 FreeBytesAvailable;
    u64 TotalNumberOfBytes;
    u64 TotalNumberOfFreeBytes;

    GetDiskFreeSpaceEx("C:\\",(PULARGE_INTEGER)&FreeBytesAvailable,
                       (PULARGE_INTEGER)&TotalNumberOfBytes, (PULARGE_INTEGER)&TotalNumberOfFreeBytes);

    return TotalNumberOfBytes;
}
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

