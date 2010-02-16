
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ogc/lwp_watchdog.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "../include/libwiiradio.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;


char* tests[] = {   "http://205.188.215.225:8002",
                    "http://scfire-dtc-aa07.stream.aol.com:80/stream/1040",
                    "http://208.76.152.74:8000"
				};



int test_lib()
{
    printf("\n\n\n\nTest libWiiRadio, will test three streams!\n");

    for(int i = 0; i < 3;i++)
    {
        printf("Connecting to %s\n", tests[i]);


		LWR_SetVolume(40*(i+1));
        printf("setting volume %d\n", 40*(i+1));
		

        if(LWR_Play(tests[i]) < 0)
        {
            printf("ERROR!\n");
            LWR_Stop();
			continue;
        }

        usleep(2000000); // wait till the info is grabbed

        char *title = LWR_GetCurrentTrack();
        if (title) printf("current track = %s\n", title);

        int br = LWR_GetCurrentBitRate();
        if (br != -1) printf("current bitrate = %d\n", br);
		
		char* url = LWR_GetUrl();
		if (url) printf("current url = %s\n", url);
		
		int vol = LWR_GetVolume();
        if (vol != -1) printf("current volume = %d\n", vol);
		
        
		usleep(30000000); // play for a while
		

		printf("Stop Playback!\n");

        LWR_Stop();
    }

    printf("Test Done!\n");

    return 0;
}


//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();
	

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
	

	test_lib();
	

	return 0;
}

