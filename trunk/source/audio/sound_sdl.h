#ifndef _SOUND_SDL_H_
#define _SOUND_SDL_H_
class codec_mp3;
class app_wiiradio;
class audio_device
{
    public:

    app_wiiradio*   theapp;

    audio_device(app_wiiradio* _theapp,s32 (*_read)(void*,void *,s32,app_wiiradio*), void (*_samples_cb)(u8* data,int len,app_wiiradio*));
    ~audio_device();

    void sound_close();
    void sound_update(u16 *buffer, u32 num_samples);
    //void sound_update(u16 *buffer, u32 num_samples);
    unsigned int sound_freespace();
    void sound_mute(const int mute);
    void sound_pause(const int pause);
    void sound_volume(int volume);
    int sound_status();
    void sound_stop();
    void sound_start(const int mb, const u32 size);
    unsigned int sound_get_bitrate();
    u32 get_time_position();

    void sound_seek(u32 pos, u32 total);

    bool done;
    bool sound_done() { return done; }

    const char* get_position_fmtd();
    u32 total_samples;
    u32 time_elapsed;

    codec_mp3*      mp3;
    bool            playing;
    SDL_AudioSpec   audiofmt;
    int             sdl_volume;
    u32             min_buffers;


    u8* audio_buffer;
    u32 audio_rd;
    u32 audio_wd;
    u32 audio_rd_num;
    u32 audio_wd_num;
    u64 audio_total_decoded;
    u64 audio_total_done;

    u32     filesize;
    bool    audio_running;

    u32 GetSongLen();
    u32 GetSongPlayed();
    u32 GetBitRate();


    private:

    void sound_init();

    char time_fmt[10];






};
#endif
