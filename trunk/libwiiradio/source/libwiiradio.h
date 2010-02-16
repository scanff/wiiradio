#ifndef LIBWIIRADIO_H_INCLUDED
#define LIBWIIRADIO_H_INCLUDED


#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

// lib functions
int LWR_Play(char*);

int LWR_Stop();

// return the current URL
char* LWR_GetUrl();

// return what's playing
char* LWR_GetCurrentTrack();

// return bitrate
int LWR_GetCurrentBitRate();

// return the volume
int LWR_GetVolume();

// set the volume
void LWR_SetVolume(int);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif // LIBWIIRADIO_H_INCLUDED
