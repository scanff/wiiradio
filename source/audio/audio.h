#ifndef _AUDIO_H_
#define _AUDIO_H_

#define CODEC_MP3 (0)
class audio_codec
{
    public:

    bool                playing;
    int                 volume;
    int                 pause;
    int                 bitrate;

    public:

    int m_Type;


    audio_codec() :
        playing(false),
        volume(100),
        pause(0),
        bitrate(0)
        {};

   // virtual void stop(){ playing = false; };
    virtual void set_volume(int v) { volume = v; };
    virtual int get_isplaying() { return playing; };
    virtual void set_isplaying(const int p) { playing = p; };
   // virtual int get_bitrate() { return bitrate; };
   // virtual void set_bitrate(int b) { bitrate = b; };

//	virtual int get_buffer(unsigned char* buf, int request_len) {};
//	virtual void seek(int seconds) {};

/*	virtual int getLength();
	virtual uint32_t getFrequency();
	virtual void close();
	virtual void readAhead();

	virtual void getNowPlaying(char *output);
	virtual int supportsFormat(const char *ext);
	virtual bool hasTime();

		void getFormat(char *fmt);*/

};

#endif
