#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#define B_ENDIAN 1
#define L_ENDIAN 0


#ifndef _WIN32
#define ENDIAN B_ENDIAN
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ogcsys.h>
#include <gccore.h>
#include <gctypes.h>
#include <network.h>
#include <debug.h>
#include <unistd.h>
#include <fcntl.h>
#include <asndlib.h>
#include <mp3player.h>

#define Sleep(x) usleep(x*1000);//1000?
//#include <errno.h>
#define _WII_

#else

#ifdef _WIN32
//#define WIN32
#define ENDIAN L_ENDIAN
#include <winsock2.h>
#include <process.h>

#include "fmod/inc/fmod.h"
#include "fmod/inc/fmod_errors.h"


typedef SOCKET s32;
typedef int u32;
#define O_NONBLOCK 1
#endif

#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>

#define MAX_KEYS 300

enum _status{
  STOPPED = 0,
  PLAYING,
  BUFFERING,
  FAILED
};
enum _screens{
  S_ALL = 0,
  S_BROWSER,
  S_PLAYLISTS,
  S_GENRES,
  S_OPTIONS,
  S_ABOUT,
  S_STREAM_INFO,
  S_MAX

};
extern Uint8 *		g_nKetStatus;
extern Uint8        g_real_keys[MAX_KEYS];
extern Uint8        g_keys_last_state[MAX_KEYS];
extern int			g_FPS;
extern int 			g_nGlobalStatus;
extern int 			g_screen_status;
extern bool         g_running;
extern int          status;
extern bool         visualize;
extern int          visualize_number;
extern bool         g_pause_draw;
extern int          mp3_volume;
extern Uint64       g_vol_lasttime;
extern bool         mute;

typedef unsigned long DWORD;
typedef unsigned char BYTE;

extern Uint64 get_tick_count();
extern char* make_path(const char*);
extern SDL_Surface *screen;
extern void draw_rect(SDL_Surface*,int,int,int,int,unsigned long);
extern void menu_button_click(int);
extern void delete_playlist(int);

#define K_UP_1      SDLK_UP
#define K_DOWN_1    SDLK_DOWN
#define K_LEFT_1    SDLK_LEFT
#define K_RIGHT_1   SDLK_RIGHT
#define K_FIRE_1    SDLK_SPACE


#define K_UP_2      SDLK_w
#define K_DOWN_2    SDLK_s
#define K_LEFT_2    SDLK_a
#define K_RIGHT_2   SDLK_d
#define K_FIRE_2    SDLK_r

#define SCREEN_WIDTH    (640)
#define SCREEN_HEIGHT   (480)
#define FONT_SIZE       (32)//(40)
#define VERSION_NUMBER  (0.3)
#define VERSION_NAME    ("Version")
#define BITDEPTH        (24)
#define TIME_OUT_MS     (5000)
#define SC_DOWN         ("503 Service Temporarily Unavailable")
#define MAX_VISUALS     (2)
#define MAX_BUTTONS     (4)
#define MAX_FFT_RES     (16)
#define loop(v,m) for(int v = 0; v<int(m); v++)
#define bloop(v,m) for(int v = int(m)-1; v>=0; v--)

#define loopi(m) loop(i,m)
#define loopj(m) loop(j,m)
#define bloopj(m) bloop(j,m)
#define CLIP(mx,x)  x>mx ? (mx) : x < 0 ? (0) : (x);

extern void connect_to_stream(int,bool);
extern void search_genre(char*);
extern void genre_nex_prev(bool,char*);
extern void request_save_fav();
extern void screen_saver();

extern int display_idx; //holds the current displaying browser index
extern int genre_display; //holds the genre list start display value

extern int pls_display;
extern int total_num_playlists;
extern SDL_Event event;
extern bool refresh_genre_cache;
extern bool sc_error;
#include "textures.h"
extern texture_cache* tx;
#endif


