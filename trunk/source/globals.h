#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define B_ENDIAN 1
#define L_ENDIAN 0

#include "types.h"

#include <string>
#include <sstream>

using namespace std;

#ifdef _WII_
    #define MAX_PATH 255
    #define ENDIAN B_ENDIAN
    #ifndef EAGAIN
    #define EAGAIN (-11)
    #endif
    #include <wiiuse/wpad.h>
    #include <ogc/lwp_watchdog.h>
    #include <fat.h>
    #include <stdio.h>
    #include <stdlib.h>
    //#include <ogcsys.h>
    #include <gccore.h>
    #include <gctypes.h>
    #include <network.h>
    #include <debug.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <asndlib.h>
    #include <malloc.h>

    #define Sleep(x) usleep(x*1000);

    extern short active_ir;
    extern char localip[16];
    extern char gateway[16];
    extern char netmask[16];
    #define MAX_WPAD 4
#else

    #ifdef _WIN32

        #define ENDIAN L_ENDIAN
        #include <winsock2.h>
        #include <process.h>
        #include <malloc.h>
        #include <unistd.h>

        //#define usleep(x) Sleep(1) // -- min delay is 1ms on windoze

        inline void* memalign(const u32 bits, const u32 size)
        {
            return malloc(size);
        }

        #define O_NONBLOCK 1
    #endif
    #ifdef _LINUX_
        #include <netdb.h>
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
        #include <unistd.h>
        #include <sys/select.h>
        #include <poll.h>
        #include <malloc.h>

        typedef unsigned int u32;
         #define O_NONBLOCK 1
         #define MAX_PATH 1024

        #define Sleep(x) usleep(x*1000);
    #endif

#endif


#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>

typedef void(*func_void)(void*);  // void foo(void*) function pointer

// -- audio source !
enum audio_source
{
    AS_NETWORK = 0,
    AS_LOCAL,
    AS_NULL // Between states

};

enum enum_status
{
  // PLAYBACK STATES
  ALL = 0,
  STOPPED,
  PLAYING,
  PAUSE,
  BUFFERING,
  FAILED,
  CONNECTING,
  SEEKING,
  SEARCHING,
  REDIRECT,
  START_PLAYING_FILE,
  START_SEARCH,
  LOCALFILE_BUFFERING
};

#define MAX_METADATA_SIZE       (4080+100) //16*255 + padding
#define BUFFER_CHUNK            (2048)
#define OUT_BUFFERSIZE			(8192)
#define SYS_SEARCHING           (8)
#define BUFFERING_TIMEOUT       (30*1000) // if we get stuck buffering for > 30sec give up

enum
{
  S_NONE = -1,
  S_MAIN = 0,
  S_BROWSER,
  S_PLAYLISTS,
  S_GENRES,
  S_CANCEL_CON_BUF, // remove
  S_SEARCHING,
  S_LOCALFILES,
  S_START_OVERLAYS = 20,
  S_STREAM_INFO,
  S_OPTIONS,
  S_SEARCHGENRE,
  S_SCREEN_CONNECT,
  S_QUIT,
  S_MAX,
  //special
  S_VISUALS,
  S_LOG
};

enum
{
  V_BARS = 0,
  V_WAVES,
  V_WATER,
  V_OSC,
  V_TUNNEL,
  V_FIRE,
  V_MIST,
  V_CIRCLES,
  V_PLASMA,
  V_SINTEXT,
  V_ROTZOOM,
  V_BOBS,
  V_CIRCULAR,
  V_RAYCASTER,
  V_MATRIX,
  V_STARS,
  V_EXPLODE,
  MAX_VISUALS
};


enum
{
    SERVICE_SHOUTCAST = 0,
    SERVICE_ICECAST,
    SERVICE_UNKNOWN
};

enum
{
    SEARCH_GENRE = 0,
    SEARCH_STATIONS,
    SEARCH_CONNECT,
    SEARCH_MAX
};


extern u8 *        g_nKetStatus;
extern int          g_nGlobalStatus;
extern bool         g_running;


extern SDL_mutex*   listviews_mutex;
extern SDL_mutex*   connect_mutex;
extern SDL_mutex*   script_mutex;

extern char         g_storage_media[8];

extern u64 get_tick_count();
extern const char* make_path(const char*);
extern void draw_rect(SDL_Surface*,int,int,int,int,unsigned long);
extern const char* make_string(char*,...);
extern const char* trim_string(char*, int);
extern void stdstring_trim( string& str);


#ifdef _WII_

extern "C" {
    extern void WII_SetWidescreen(int wide);
    // other SDL secrets
    extern void WII_VideoStart();
    extern void WII_VideoStop();
    extern unsigned int *xfb[2]; // Double buffered
    extern int whichfb; // Switch

}
#endif

enum _VideoMode{
    VIDM_GX = 0,
    VIDM_SDL
};
extern void switch_sdl_video_mode(const _VideoMode mode);

#define ONE_SECOND      (1000)
#define SCREEN_WIDTH    (640)
#define SCREEN_HEIGHT   (480)
#define SCREEN_HEIGHT_D2    (SCREEN_HEIGHT/2)
#define SCREEN_WIDTH_D2     (SCREEN_WIDTH/2)
#define SCREEN_WIDTH_BUFFER  (30)
#define SCREEN_HEIGHT_BUFFER (30)
#define FONT_SIZE       (40)
#define VERSION_NUMBER  (0.7)
#ifdef _WII_
#define BITDEPTH        (24)
#else
#define BITDEPTH        (24)
#endif
#define MAX_TAG_SIZE    (30)
#define APIC_SIZE       (200)

#define TIME_OUT_MS     (8000) // eight second timeout
#define MAX_FFT_RES     (16)
#define SMALL_MEM       (260)
#define MED_MEM         (1024)
#define TINY_MEM        (50)
#define MAX_FFT_SAMPLE  (8192/4)
#define MAX_NET_BUFFER  (MED_MEM*6)
#define STD_STREAM_PORT (80)
#define MAX_STATION_CACHE (1000)
// macros
#define loop(v,m) for(int v = 0; v<int(m); v++)
#define bloop(v,m) for(int v = int(m)-1; v>=0; v--)
#define loopi(m) loop(i,m)
#define loopj(m) loop(j,m)
#define loopk(m) loop(k,m)
#define bloopj(m) bloop(j,m)
#define CLIPA(x,mx)  x > mx ? x = mx : x < 0 ? x = 0 : (x);
#define CLIPB(x,mx)  x > mx ? mx : x < 0 ? 0 : (x);

#define PI              (3.14159265)
#define MAX_Z_ORDERS    (3) // -- z orders per layer
#define ELEMENTS_IN_2D_ARRAY(x)  ((int)((sizeof(x)) / (sizeof(x[0]))))


#define SHRT_MAX 32767

#ifdef _WII_
    #define RATE    48000
#else
    #define RATE    44100
#endif

const inline unsigned long MAKE_HASH(const char* x)
{
    unsigned long h = 0;
    const char* ptr = x;
    while (*ptr) h = (h << 5) + h + *ptr++;

    return h;
}

// SDL BYTE ORDER
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define rmask  0x00ff0000
    #define gmask  0x0000ff00
    #define bmask  0x000000ff
    #define amask  0x00000000
#else
    #define rmask  0x000000ff
    #define gmask  0x0000ff00
    #define bmask  0x00ff0000
    #define amask  0x00000000
#endif

extern int max_listings;

extern void screen_saver();
extern void fade(SDL_Surface*, const int);
extern void draw_rect_rgb(SDL_Surface*,const int x,const int y,
                   const int w, const int h,const int r,
                   const int g, const int b);

extern u64   get_media_free_space();
extern string get_media_free_space_desc();
/*
extern int display_idx; //holds the current displaying browser index
extern int pls_display;
extern int total_num_playlists;
*/
extern SDL_Event event;

extern bool sc_error;
extern audio_source playback_type;

#endif




