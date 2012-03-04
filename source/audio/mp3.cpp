#include "../globals.h"
#include "../application.h"
#include <mad.h>
#include "audio.h"
#include "mp3.h"

#define SAMPLE_DEPTH 16
bool g_Stereo = true;

int codec_mp3::get_bitrate(){ return 0;};


codec_mp3::codec_mp3() :
    current_outbuf(0),
    outbuf_pos(0),
    playing( false ),
    ReadSize(0),
    databuffer(0)
{
}

int codec_mp3::init(s32 (*reader)(void *,void *,s32,app_wiiradio*),app_wiiradio* _theapp)
{
    mp3read = reader;
    theapp = _theapp;

#ifdef _WII_
    Init3BandState(&eqs[0],880,5000,48000);
    Init3BandState(&eqs[1],880,5000,48000);
#else
    Init3BandState(&eqs[0],880,5000,44100);
    Init3BandState(&eqs[1],880,5000,44100);
#endif
    return 1;
}


void codec_mp3::start()
{
    if(playing) return;

    length.seconds = 0;
    samplecount = 0;

    mad_stream_init(&Stream);
    mad_frame_init(&Frame);
    mad_synth_init(&Synth);
    mad_timer_reset(&Timer);

    playing = true;
}

void codec_mp3::stop()
{
    if(!playing) return;

    playing = false;

    mad_synth_finish(&Synth);
    mad_frame_finish(&Frame);
    mad_stream_finish(&Stream);

    samplecount = 0;
}


int codec_mp3::decode2(void *block, u32 *size)
{
	u8 *samples = (u8*)block;
	u32 nsamples;

	nsamples = (*size / (SAMPLE_DEPTH / 8)) >> (g_Stereo ? 1 : 0);
	*size = 0;

	while (nsamples)
	{
		u32 count;

		count = Synth.pcm.length - samplecount;

		if (count > nsamples)
			count = nsamples;

		if (count)
		{
            resample2((u16**)&samples,count);
			samplecount += count;

			nsamples -= count;

			if (nsamples == 0)
				break;
		}

		while (mad_frame_decode(&Frame, &Stream) == -1)
		{
			u8 *ReadStart;
            s32 Remaining;

			if (MAD_RECOVERABLE(Stream.error))
				continue;

            if(Stream.next_frame!=NULL)
            {
                Remaining = Stream.bufend - Stream.next_frame;
                memmove(InputBuffer,Stream.next_frame,Remaining);
                ReadStart = InputBuffer + Remaining;
                ReadSize = DATABUFFER_SIZE - Remaining;
            } else {
                ReadSize = DATABUFFER_SIZE;
                ReadStart = InputBuffer;
                Remaining = 0;
            }

            if((ReadSize = mp3read(0,ReadStart,ReadSize,theapp)) <= 0)
                return 0;


			mad_stream_buffer(&Stream,InputBuffer,(ReadSize + Remaining));
		}


		mad_synth_frame(&Synth, &Frame);

		samplecount = 0;

		mad_timer_add(&Timer, Frame.header.duration);
	}

	*size = samples - (u8 *) block;

   if (!length.seconds)
   {
        header = Frame.header;
        mad_timer_set(&length, 0, 1, header.bitrate / 8);
        mad_timer_multiply(&length, filesize);
   }

	return 1;
}


int inline codec_mp3::resample2(u16** buf, u32 num)
{

    dword pos;
    s32 incr;
    struct mad_pcm *Pcm = &Synth.pcm;
    u32 stereo = (MAD_NCHANNELS(&Frame.header)==2);
    u32 src_samplerate = Frame.header.samplerate;

    pos.adword = 0;

    incr = (u32)(((f32)src_samplerate/RATE)*65536.0F);

    while(pos.aword.hi < num)
    {
        (*buf)[0] = (u16)Do3Band(&eqs[0],mad_fixed_to_ushort(Pcm->samples[0][pos.aword.hi + samplecount]));

        if(stereo)
        {
            (*buf)[1] = (u16)Do3Band(&eqs[1],mad_fixed_to_ushort(Pcm->samples[1][pos.aword.hi + samplecount]));
            (*buf)++;
        }

        (*buf)++;


        pos.adword += incr;

    }

    return pos.aword.hi;
}


u16 inline codec_mp3::mad_fixed_to_ushort(mad_fixed_t fixed)
{
    if(fixed>=MAD_F_ONE)
        return(SHRT_MAX);

    if(fixed<=-MAD_F_ONE)
        return(-SHRT_MAX);

    fixed=fixed>>(MAD_F_FRACBITS-15);
    return((u16)fixed);
}

void codec_mp3::Init3BandState(EQState *es,s32 lowfreq,s32 highfreq,s32 mixfreq)
{
    memset(es,0,sizeof(EQState));

    es->lg = 1.0;
    es->mg = 1.0;
    es->hg = 1.0;

    es->lf = 2.0F*sinf(M_PI*((f32)lowfreq/(f32)mixfreq));
    es->hf = 2.0F*sinf(M_PI*((f32)highfreq/(f32)mixfreq));
}

s16 codec_mp3::Do3Band(EQState *es,s16 sample)
{
    f32 l,m,h;

    es->f1p0 += (es->lf*((f32)sample - es->f1p0))+VSA;
    es->f1p1 += (es->lf*(es->f1p0 - es->f1p1));
    es->f1p2 += (es->lf*(es->f1p1 - es->f1p2));
    es->f1p3 += (es->lf*(es->f1p2 - es->f1p3));
    l = es->f1p3;

    es->f2p0 += (es->hf*((f32)sample - es->f2p0))+VSA;
    es->f2p1 += (es->hf*(es->f2p0 - es->f2p1));
    es->f2p2 += (es->hf*(es->f2p1 - es->f2p2));
    es->f2p3 += (es->hf*(es->f2p2 - es->f2p3));
    h = es->sdm3 - es->f2p3;

    m = es->sdm3 - (h+l);

    l *= es->lg;
    m *= es->mg;
    h *= es->hg;

    es->sdm3 = es->sdm2;
    es->sdm2 = es->sdm1;
    es->sdm1 = (f32)sample;

    return (s16)(l+m+h);
}

