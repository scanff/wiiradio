#ifndef __MP3CPLAYER_H__
#define __MP3CPLAYER_H__

#include <mad.h>
#include "audio.h"
#include "../application.h"

typedef float f32;

typedef struct _eqstate_s
{
	f32 lf;
	f32 f1p0;
	f32 f1p1;
	f32 f1p2;
	f32 f1p3;

	f32 hf;
	f32 f2p0;
	f32 f2p1;
	f32 f2p2;
	f32 f2p3;

	f32 sdm1;
	f32 sdm2;
	f32 sdm3;

	f32 lg;
	f32 mg;
	f32 hg;
} EQState;

typedef union {
	struct {
#ifdef _WII_
		u16 hi;
		u16 lo;
#else // LE
		u16 lo;
		u16 hi;
#endif

	} aword;
	u32 adword;
} dword;

#define VSA  (1.0/4294967295.0)
#define DATABUFFER_SIZE			(32768)

class codec_mp3
{
	public:

        app_wiiradio* theapp;

        s32 (*mp3read)(void*,void *,s32,app_wiiradio*);
        void (*mp3samples)(u8* data,int len);

		struct mad_stream 	Stream;
		struct mad_frame 	Frame;
		struct mad_synth 	Synth;
		mad_timer_t 	    Timer;

		mad_timer_t         length;
		mad_header          header;

		u32                 filesize;

		u8                  InputBuffer[DATABUFFER_SIZE+MAD_BUFFER_GUARD] __attribute__((aligned(32)));
		int                 current_outbuf;
		int                 outbuf_pos;
		bool                playing;

        s32                 ReadSize;
        void*               databuffer;

        EQState eqs[2];

        bool atend;

		codec_mp3();
		int init(s32 (*reader)(void *,void *,s32,app_wiiradio*),app_wiiradio* _theapp);
		void stop();
		void start();

        int decode2(void *block, u32 *size);
        int inline resample2(u16** buf, u32 num);

		u16 inline mad_fixed_to_ushort(mad_fixed_t fixed);
        u16* get_buffer();
        int get_num_samples();
		int get_bitrate();

        void Init3BandState(EQState *es,s32 lowfreq,s32 highfreq,s32 mixfreq);
        s16 Do3Band(EQState *es,s16 sample);


};

#endif
