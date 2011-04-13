#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define B_ENDIAN 1
#define L_ENDIAN 0


#ifdef _WII_
    #define MAX_PATH 255
    #define ENDIAN B_ENDIAN
    #include <ogc/lwp_watchdog.h>
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
    #include "mp3player.h"

    #define Sleep(x) usleep(x*1000);//1000?
    //#include <errno.h>

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
    #ifdef _LINUX_
        #include <netdb.h>
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
        #include <unistd.h>

        #include "inc/fmod.h"
        #include "inc/fmod_errors.h"

        typedef socklen_t s32;
        typedef unsigned int u32;

        #define Sleep(x) usleep(x*1000);
    #endif

#endif


#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define SMALL_MEM (255)
#define loop(v,m) for(int v = 0; v<int(m); v++)
#define loopi(m) loop(i,m)
#define loopj(m) loop(j,m)
#define loopk(m) loop(k,m)


#endif


