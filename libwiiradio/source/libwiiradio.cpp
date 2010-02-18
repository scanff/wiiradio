#include "globals.h"
#include "libwiiradio.h"
#include "client.h"
#include "icy.h"
#include "mp3player.h"

static network* net = 0;
static icy* icy_info = 0;
static int connected = false;
static int LWP_playing = false;
static int errors = 0;
static int LWR_mp3_volume = 255;
#define MAX_SAMPLES (8192/4)
static short sample_data[MAX_SAMPLES] = {0};

#define MAX_NET_BUFFER (10000) // 10k

#ifdef _WII_

    static	lwp_t critical_handle = (lwp_t)NULL;
    static void *critical_thread (void *arg);

#else

    static FMOD_SYSTEM             *fmod_system = 0;
    static FMOD_SOUND              *sound1 = 0 ;
    static FMOD_CHANNEL            *channel1 = 0;
    static FMOD_CREATESOUNDEXINFO  exinfo;

    #ifdef _WIN32

        static HANDLE critical_handle = 0;
        static unsigned critical_thread(void *arg);

    #endif

#endif


static void split_url(char* url,char* _name, int* _port, char* _path)
{
    char url_copy[1024] = {0};
    int part = 0;

    strcpy(url_copy,url);

    char* split = url_copy;

     // ignore http://
    char* search = strstr(url_copy,"http://");
    if(!search) return;

    search += strlen("http://");

    split = strtok(search,":/");
    while(split) {

        if(!part) {
            strcpy(_name,split);
        }else if(part==1) {
            *_port = atoi(split);
            if (*_port == 0) {
                strcat(_path,"/");
                strcat(_path,split);
            }
        }else{
            strcat(_path,"/");
            strcat(_path,split);
        }

        split = strtok(0,":/");
        part++;
    }
};


static int connect(char* name)
{
    char url[1024] = {0};
    char path[1024] = {0};
    int port = 80;
    char* host = 0;
    char request[1024] = {0};

  // -- close current connection
    if (connected)
    {
        connected = 0;
        net->client_close();
    }

    // stop mp3 if currently playing.
    LWR_Stop();

    // parse the url
    split_url(name, url, &port, path);

    // connect to new stream
    int connect_try = net->client_connect(url,port,TCP,false);
	if (!connect_try) connect_try = net->client_connect(url,port,TCP,false); // try again .. weird on wii
    icy_info->icy_reset(); // reset icy state

    if (connect_try) // send stream request to the server
    {
        host = net->get_local_ip();

        //create the server request (to get the stream)
        sprintf(request,
                "GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Accept: */*\r\n"
                "User-Agent: WiiRadio-lib/1.0\r\n"
                "Icy-Metadata: 1\r\n\r\n\r\n",
                !(*path) ? "/" : path,host);

        int len_req = strlen(request);
        len_req = net->client_send(request,len_req);

        connected = connect_try;

    }else return -1;

    return 1; // ok
}


/*
called from libmad ....
return the data from the buffer or a default zero stream if we are having problems keeping up!
*/
static s32 reader_callback(void *usr_data,void *cb,s32 len)
{
    return icy_info->get_buffer(cb,len);
}

#ifndef STD_MAD
// callback called from mod of libmad
static void cb_fft(short* in, int max)
{
	
	if (max < MAX_SAMPLES) memmove(sample_data,in,max);
	else memmove(sample_data,in,MAX_SAMPLES);

}
#endif

// shoutcast playback
#ifdef _WII_
void *critical_thread(void *arg)
#else
unsigned __stdcall critical_thread(void *arg)
#endif
{
    int len = 0;
    char* net_buffer = 0;

    net_buffer = new char[MAX_NET_BUFFER];
    if (!net_buffer) return 0;

    LWP_playing = true;
    while(LWP_playing)
    {
        // stream handler
        if (connected)
        {

            // stream handler
            len = net->client_recv(net_buffer,MAX_NET_BUFFER);

            if(len > 0)
            {
                // reset errors
                errors = 0;

                // metaint handler
                len = icy_info->parse_metaint(net_buffer,len);

                // data handler
                icy_info->buffer_data(net_buffer,len);


            }else if(len < 0){

                /* would block, not really an error in this case */
                if (len != -11) errors++;

                if (errors > 100)
                {   // -- too many errors let's reset
                    LWP_playing = false;
                    break;
                }
            }


            if (!icy_info->bufferring) // only play if we've buffered enough data
            {

#ifdef _WII_
                if(!LWR_MP3Player_IsPlaying())
                {
                    //modified function
                    if(connected)
                    {
#ifdef STD_MAD
                        MP3Player_PlayFile(icy_info->buffer, reader_callback, 0);
#else
                        LWR_MP3Player_PlayFile(icy_info->buffer, reader_callback, 0,cb_fft);
#endif
                    }
                }
#else

                FMOD_BOOL is_playing = false;
                FMOD_Channel_IsPlaying(channel1,&is_playing);
                if (!is_playing)
                {
                    FMOD_System_CreateSound(fmod_system,(const char *)icy_info->buffer,FMOD_SOFTWARE | FMOD_OPENMEMORY_POINT | FMOD_CREATESTREAM | FMOD_CREATECOMPRESSEDSAMPLE,&exinfo, &sound1);
                    FMOD_Sound_SetMode(sound1, FMOD_LOOP_OFF);
                    FMOD_System_PlaySound(fmod_system,FMOD_CHANNEL_FREE, sound1, false, &channel1);
                }
#endif
            }

        }else{

            errors = 0; // no rec errors
            LWP_playing = false; // not connected
        }


        Sleep(15); // minimum delay
    }

    delete [] net_buffer; net_buffer =0;
    if (connected) net->client_close();

#ifdef _WIN32
    _endthreadex( 0 );
#endif

    return 0;
}

// lib functions
int LWR_Play(char* name)
{
    if (!name) return -1;

    net = new network;
    icy_info = new icy;

    if (!connect(name)) return -2;

    // set up sound system
#ifdef _WII_
    ASND_Init();
    LWR_MP3Player_Init();
    LWR_MP3Player_Volume(LWR_mp3_volume);
#else
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = icy_info->buffer_size;

    FMOD_System_Create(&fmod_system);
    FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);
    FMOD_Channel_SetVolume(channel1, LWR_mp3_volume/255.);
#endif

#ifdef _WII_
    critical_handle = 0;
	LWP_CreateThread(&critical_handle,critical_thread,NULL, NULL, 50000, 100);
    if (!critical_handle) return -3;
#else
    #ifdef _WIN32
    critical_handle = 0;
    critical_handle = (HANDLE)_beginthreadex( 0, 0, &critical_thread, NULL, 0, 0 );
    if (!critical_handle) return -3;
    #endif
#endif


    return 1;
};


// return the decoded samples
int LWR_GetSamples(short* in,int size)
{
    if(size >= MAX_SAMPLES)
	{
		memmove(in,sample_data,MAX_SAMPLES);
		return MAX_SAMPLES;
	}else{
		memmove(in,sample_data,size);
		return size;
	
	}
	return 0;
};

int LWR_Stop()
{


    if (LWP_playing)
    {


    LWP_playing = false;

#ifdef _WII_
	void *v;
    LWP_JoinThread(critical_handle,&v);
    critical_handle = 0;
#else

#ifdef _WIN32
    WaitForSingleObject( critical_handle, INFINITE );
    CloseHandle( critical_handle );
    critical_handle = 0;
#endif

#endif


#ifdef _WII_
    if(LWR_MP3Player_IsPlaying()) LWR_MP3Player_Stop();
#else
    FMOD_BOOL is_playing = false;
    FMOD_Channel_IsPlaying(channel1,&is_playing);
    if (is_playing) FMOD_Channel_Stop(channel1);

#endif

    delete icy_info;
    icy_info = 0;

    delete net;
    net = 0;

#ifndef _WII_
    FMOD_Sound_Release(sound1);
    FMOD_System_Close(fmod_system);
    FMOD_System_Release(fmod_system);
#endif

    } // playing

    return 1;
};

// set the volume
void LWR_SetVolume(int vol)
{
    LWR_mp3_volume = vol;

    if(!LWP_playing) return;

#ifdef _WII_
    LWR_MP3Player_Volume(LWR_mp3_volume);
#else
    FMOD_Channel_SetVolume(channel1, LWR_mp3_volume/255.);
#endif
};

// return the current volume
int LWR_GetVolume()
{
    return LWR_mp3_volume;
};

// return the current URL
char* LWR_GetUrl()
{
    if(!LWP_playing) return 0;

    return icy_info->icy_url;
};

// return what's playing
char* LWR_GetCurrentTrack()
{
    if(!LWP_playing) return 0;

    return icy_info->track_title;

};

// return bitrate
int LWR_GetCurrentBitRate()
{
    if(!LWP_playing) return -1;

    return icy_info->icy_br;

};

// set up libwiiradio
int LWR_SetBufferSize(unsigned long maxbuffer)
{
	if(!LWP_playing) return 0;

    icy_info->buffer_size = maxbuffer;
	icy_info->pre_buffer = (unsigned long)(icy_info->buffer_size / 10);

	return icy_info->buffer_size;

};
