#include "globals.h"
#include "application.h"
#include "projectm.h"
#include <projectM/projectM.hpp>


projectm::projectm(app_wiiradio*   _theapp)
: pm(0)
{
    theapp = _theapp;

    preset_dir = make_path("data/projectm/presets");
    preset_name = "Aderrasi - Agitator.milk";
    init_done = false;

}

projectm::~projectm()
{
    if (pm) delete pm;
}

void projectm::init()
{
    init_done = true;

    config.meshX 				= 30;
    config.meshY 				= 20;
    config.textureSize 			= 512;
    config.windowWidth 			= 640;
    config.windowHeight 		= 480;
    config.smoothPresetDuration = 5;
    config.presetDuration 		= 15;
    config.beatSensitivity 		= 20.0f;
    config.aspectCorrection 	= true;
    config.shuffleEnabled 		= true;
    config.presetURL 			= preset_name;
    config.presetDIR 			= preset_dir.c_str();

    if (pm) delete pm;

    pm = new projectM(config);
    if (!pm)
        exit(0);


}

void projectm::render()
{
    if(!init_done) init();

#ifdef _WII_
    GXColor bgclr = {0,0,0,0xFF};

    // Render the next frame in the visualisation/preset
    short *data = (short*)theapp->audio_data;

    pm->pcm()->addPCM16Data(data, 512);
    pm->renderFrame();

    // handle the FB
    GX_SetCopyClear(bgclr,0);
    GX_SetAlphaUpdate(GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    whichfb ^= 1;
    GX_CopyDisp(xfb[whichfb],GX_TRUE);
    GX_DrawDone();

    VIDEO_SetNextFramebuffer(xfb[whichfb]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
#else

        short *data = (short*)theapp->audio_data;

        pm->pcm()->addPCM16Data(data, 512);
        pm->renderFrame();

#endif


}


