#include "../globals.h"
#include "../application.h"
#include "mp3.h"
#include "sound_sdl.h"

#define SAMPLES 1024
#define AUDIO_BUFFER_SIZE   (OUT_BUFFERSIZE*220)

static void (*samples)(u8* data,int len,app_wiiradio* _theapp);
static s32 (*reader)(void*,void *,s32,app_wiiradio* _theapp);

static u8 mix_buffer[SAMPLES * 4] __attribute__((aligned(32))); // 16bit * 2 channels * samples
static u16 buffer[OUT_BUFFERSIZE] __attribute__((aligned(32)));

static SDL_mutex* audio_mutex = 0;
static int audio_thread (void *arg);
static SDL_Thread* audiothread = 0;

static int audio_thread(void *arg)
{
    audio_device* dev = (audio_device*)arg;

    while(!dev->done)
    {
        SDL_mutexP(audio_mutex);

        u32 samps = OUT_BUFFERSIZE;

        if( (dev->audio_total_decoded - dev->audio_total_done) < dev->min_buffers)
        {
            if(dev->mp3->decode2(buffer,&samps))
            {
                dev->sound_update(buffer, samps/2);
                dev->audio_total_decoded += samps;
                dev->time_elapsed = dev->mp3->Timer.seconds; // This is not very acurate

            }
        }

        SDL_mutexV(audio_mutex);

        usleep(400);
    }
	
	return 0;
}

static void MixAudio(void *userdata, u8 *stream, int len)
{
    audio_device* dev = (audio_device*)userdata;

    if(!dev->playing)
        return;


    SDL_mutexP(audio_mutex);

    int i;

    // Buffering ?
    if(dev->audio_total_decoded < dev->min_buffers)
    {
        SDL_mutexV(audio_mutex);
        return;
    }

    if(dev->theapp->GetSystemStatus() == LOCALFILE_BUFFERING)
    {
        SDL_mutexP(script_mutex);
        dev->theapp->SetSystemStatus(PLAYING);
        SDL_mutexV(script_mutex);
    }

    for (i = 0; i < len; i++)
    {
        if(dev->audio_rd >= AUDIO_BUFFER_SIZE)
        {
            dev->audio_rd = 0;

            if((dev->audio_rd_num >= dev->audio_wd_num) && (dev->audio_wd < dev->audio_rd))
            {
                dev->done = true;
                break;
            }

            dev->audio_rd_num++;
        }

        if((dev->audio_total_decoded <= dev->audio_total_done))
        {
             dev->done = true;
             break;
        }


        // populate the audio
        stream[i] = 0;
        mix_buffer[i] = dev->audio_buffer[dev->audio_rd];
        dev->audio_buffer[dev->audio_rd] = 0; // clear
        dev->audio_rd++;
        dev->audio_total_done++;
    }


    // Pass the samples back to the app ... note before we adjust the volume !!!
    app_wiiradio* theapp = (app_wiiradio*)((audio_device*)userdata)->theapp;
    samples((u8*)mix_buffer,i,theapp);

    // For Volume control
    SDL_MixAudio(stream,mix_buffer,i,dev->sdl_volume);

    SDL_mutexV(audio_mutex);

}

void audio_device::sound_update(u16 *buffer, u32 num_samples)
{
    for(u32 i = 0; i < num_samples; i++)
    {
        if(audio_wd >= AUDIO_BUFFER_SIZE)
        {
            audio_wd = 0;
            audio_wd_num++;
        }

        u16* buf = (u16*)&audio_buffer[audio_wd];
        *buf = buffer[i];

        audio_wd+=sizeof(u16);

    }

}

audio_device::audio_device(
    app_wiiradio* _theapp,
    s32 (*_r)(void *,void *,s32,app_wiiradio* _theapp),
    void (*_s)(u8* data,int len,app_wiiradio* _theapp))
    : theapp(_theapp), sdl_volume( SDL_MIX_MAXVOLUME )
{
    done = false;
    playing = false;
    samples = _s;   // sample filler callback
    reader = _r;    // data reader callback
    sound_init();
    mp3 = new codec_mp3();
    mp3->init(reader,theapp);
    min_buffers = 5000;

    audio_buffer = (u8*)memalign(32,AUDIO_BUFFER_SIZE);
    audio_wd_num = audio_rd_num = audio_wd = audio_rd = 0;
    audio_total_done = audio_total_decoded = 0;
    time_elapsed = total_samples = 0;
    audio_mutex = SDL_CreateMutex();

}
audio_device::~audio_device()
{
    free(audio_buffer);

    delete mp3;
    mp3 = NULL;

    SDL_DestroyMutex(audio_mutex);
}

void audio_device::sound_init()
{
   if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
      return;

    audiofmt.freq       = RATE;
    audiofmt.format     = AUDIO_S16SYS;
    audiofmt.channels   = 2;
    audiofmt.samples    = SAMPLES;
    audiofmt.callback   = MixAudio;
    audiofmt.userdata   = this;
    audiofmt.silence    = 0;

    if (SDL_OpenAudio(&audiofmt, NULL) != 0)
        return;


    audio_running = true;
    SDL_PauseAudio(0);


}

void audio_device::sound_close()
{
    sound_stop();

    SDL_CloseAudio();
    audio_running = false;

}

unsigned int audio_device::sound_get_bitrate()
{
    if (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)
    {
        return (mp3->header.bitrate / 1000);
    }

    return 0;
}
void audio_device::sound_pause(const int pause)
{
    SDL_mutexP(audio_mutex);
    SDL_PauseAudio(pause);
    SDL_mutexV(audio_mutex);
}

void audio_device::sound_stop()
{
    if(playing)
    {
        playing = false;

        SDL_mutexP(audio_mutex);
        done = true;
        SDL_PauseAudio(true);

        mp3->stop();
        audio_wd_num = audio_rd_num = audio_wd = audio_rd = 0;
        audio_total_done = audio_total_decoded = 0;

        SDL_mutexV(audio_mutex);


        if(audiothread) SDL_WaitThread(audiothread, NULL);
    }
}

void audio_device::sound_seek(u32 pos, u32 total)
{
    /*
        This is all very rough and does not work well on VBR's
    */
    audio_wd_num = audio_rd_num = audio_wd = audio_rd = 0;
    audio_total_done = audio_total_decoded = 0;

    double fraction = (double)  pos / total;
    u32 position = (mad_timer_count(mp3->length, MAD_UNITS_MILLISECONDS) * fraction);

	mad_timer_set(&mp3->Timer, position / 1000, position % 1000, 1000);
}

u32 audio_device::get_time_position()
{
    return time_elapsed;
}

const char* audio_device::get_position_fmtd()
{

    int l_time_elapsed = time_elapsed;

    int hours, minutes, seconds;

    hours = l_time_elapsed / 3600;
    l_time_elapsed -= hours * 3600;
    minutes = l_time_elapsed / 60;
    seconds = l_time_elapsed - minutes * 60;

    sprintf(time_fmt,"%02d:%02d",minutes,seconds);

    return time_fmt;
}

u32 audio_device::GetSongLen() { return mp3->length.seconds; }
u32 audio_device::GetSongPlayed() { return time_elapsed; }
u32 audio_device::GetBitRate() { return mp3->header.bitrate; }

void audio_device::sound_start(const int mb, const u32 size)
{
    if ( !playing )
    {
        SDL_mutexP(audio_mutex);

        filesize = size;
        min_buffers = 20000*mb;
        done = false;

        audio_wd_num = audio_rd_num = audio_wd = audio_rd = 0;
        audio_total_done = audio_total_decoded = 0;

        mp3->start();
        mp3->filesize = filesize;
        SDL_PauseAudio(false);

        time_elapsed = total_samples = 0;


        SDL_mutexV(audio_mutex);

        playing = true;
        audiothread = SDL_CreateThread(audio_thread,this);
        if (!audiothread)
        {
            playing = true;
        }

    }
}
void audio_device::sound_volume(int volume)
{
    // our volume max is MAX 255 so need to scale for SDL
    const int c_vol = (int)((SDL_MIX_MAXVOLUME / 255.0)*volume);

    sdl_volume =  c_vol > SDL_MIX_MAXVOLUME ? SDL_MIX_MAXVOLUME : c_vol;
}

int audio_device::sound_status()
{
    return SDL_GetAudioStatus();
}


