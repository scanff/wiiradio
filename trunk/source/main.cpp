#include "globals.h"
#include "fonts.h"
#include "client.h"

#include "browser.h"
#include "playlists.h"
#include "favorites.h"

#include "fft.h"
#include "icy.h"
#include "gui.h"



// extern functions
char* make_path(const char*);
Uint64 get_tick_count();
bool refresh_genre_cache;
bool sc_error;
bool g_pause_draw;
// ---------------------
// Variables:
// ---------------------
int			g_nGlobalStatus;
Uint8 *		g_nKetStatus;
Uint8       g_real_keys[MAX_KEYS];
Uint8       g_keys_last_state[MAX_KEYS];
bool        g_running;
int 		g_screen_status;
Uint64      g_vol_lasttime;
int         fullscreen;
int         connected = 0;
int         max_stations = 4; // request position + 4 stations
int         status;
int         display_idx;
int         favs_idx;
void*       p_fft;
int         genre_display = 0;
int         total_num_playlists = 0;
int         pls_display = 0;
int         mp3_volume;
bool        visualize;
int         visualize_number;
bool        mute;

texture_cache*      tx;
SDL_Surface*        screen;
fonts*              fnts;
icy*                icy_info;
shoutcast_browser*  scb;
playlists*          playlst;
network*            net;
gui*                ui;
fft*                fourier;
favorites*          favs;
fav_item*           playing; // make now playing as fav struct so we can access the ip/name quickly

#ifdef _WII_
//    ir_t  ir; //wii mote
#endif


SDL_Event           event;

unsigned long last_action;
#ifdef _WIN32

FMOD_SYSTEM             *fmod_system = 0;
FMOD_SOUND              *sound1 = 0 ;
FMOD_CHANNEL            *channel1 = 0;
FMOD_CREATESOUNDEXINFO  exinfo;

#else
#ifdef _WII_

static	lwp_t critical_handle = (lwp_t)NULL;
void *critical_thread (void *arg);


#endif
#endif

// -- Functions

Uint64 get_tick_count()
{
	return SDL_GetTicks();
}

char* make_path(const char* path_rel)
{
#ifdef _WII_
	static char abs_path[100] = {0};
	sprintf(abs_path,"sd:/apps/radiow/%s",path_rel);

	return abs_path;
#else
	return (char*)path_rel;
#endif

}
void draw_rect(SDL_Surface*,int,int,int,int,unsigned long);
void draw_rect(SDL_Surface* s,int x,int y, int w, int h,unsigned long color)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.h = h;
    r.w = w;
    SDL_FillRect(s, &r, color);
};

void translate_keys()
{

#ifdef _WII_


    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_HOME){
        g_real_keys[SDLK_ESCAPE] = 1;
        return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT){
        g_real_keys[SDLK_LEFT] = 1;
        return;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT){
        g_real_keys[SDLK_RIGHT] = 1;
        return;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
         g_real_keys[SDLK_DOWN] = 1;
         return;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
         g_real_keys[SDLK_UP] = 1;
         return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
         g_real_keys[SDLK_RETURN] = 1;
         return;
    }


    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) {
         g_real_keys[SDLK_b] = 1;
         return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS) {
         g_real_keys[SDLK_PLUS] = 1;
         return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS) {
         g_real_keys[SDLK_MINUS] = 1;
         return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_1) {
         g_real_keys[SDLK_1] = 1;
         return;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_2) {
         g_real_keys[SDLK_2] = 1;
         return;
    }
#endif


}

void draw_ui(char* info)
{

    // drawing stuff
    ui->draw(scb->sl_first,icy_info,favs);
    if (info) ui->draw_info(info);
    // flip to main screen buffer
    SDL_BlitSurface(ui->guibuffer,0,screen,0);

    SDL_Flip(screen);

}


void station_lister(const char* path,char* gen)
{
    // rest gui pointer
    ui->select_pos = 0;

    g_pause_draw = true;

    //force a redraw before doing the costly connect
    draw_ui("Requesting List");


    // connect and get the web page
    if(!scb->connect(path,gen))    // parse the html and grab what we need
        scb->connect(path,gen); // try again

    g_pause_draw = false;
}


void stop_playback()
{
#ifdef _WII_
	if(MP3Player_IsPlaying()) MP3Player_Stop();
#else
    FMOD_BOOL is_playing = false;
    FMOD_Channel_IsPlaying(channel1,&is_playing);
    if (is_playing) FMOD_Channel_Stop(channel1);

#endif
}

void delete_playlist(int);
void delete_playlist(int value)
{

    fav_item* csl = favs->first;
    int x = 0;
    while(csl){
        if (x==value) break;

        csl = csl->nextnode;
        x++;
    }
    if (!csl) return;

    //delete
    //file_name...
    remove(csl->file_name);

    favs->load_favorites(); // reload

}

void connect_to_stream(int,bool); //extern'd
void connect_to_stream(int value,bool haveplaylist)
{

    char* url = 0;
    char* path = 0;
    int port;


    g_pause_draw = true;

    //force a redraw before doing the costly connect
    draw_ui("Connecting"); // need a state !!!


    if (connected)
    {
        net->client_close();
        connected = 0;
    }

    // stop mp3 if currently playing.
    stop_playback();

    if (!haveplaylist) {

        value += display_idx;
        if (value > scb->total_items)
        {
            g_pause_draw = false;
            return;
        }

        station_list* csl = scb->sl_first;
        // loop through stations
        int x = 0;
        while(csl){
            if (x==value) break;

            csl = csl->nextnode;
            x++;
        }
        if (!csl) {
            g_pause_draw = false;
            return;
        }

        // ensure we can playback this stream
        if (strstr(csl->codec_type,"audio/mpeg") == 0) {
            status = FAILED;
            g_pause_draw = false;
            return;
        }

        // get playlists
        playlst->get_playlist(csl->station_id);
        playlst->parse_playlist();

        url = playlst->first_entry->url;
        port = playlst->first_entry->port;
        path = playlst->first_entry->path;

        //save to the now playing mem
        //memcpy(playing,csl,sizeof(struct station_list));
        strcpy(playing->station_name,csl->station_name);
        strcpy(playing->station_url,url);
        strcpy(playing->station_path,path);
        playing->port = port;

    }else{
        // play from playlst file
        fav_item* csl = favs->first;
        // loop through stations
        int x = 0;
        while(csl){
            if (x==value) break;

            csl = csl->nextnode;
            x++;
        }
        if (!csl) {
            g_pause_draw = false;
            return;
        }

        url = csl->station_url;
        port = csl->port;
        path = csl->station_path;

        //save to the now playing mem
        strcpy(playing->station_name,csl->station_name);
        strcpy(playing->station_url,url);
        strcpy(playing->station_path,path);
        playing->port = port;


    }


    char* host =  0;
    char request[512] = {0};

    // reset icy state
    icy_info->icy_reset();

    // connect to new stream
    connected = net->client_connect(url,port,TCP);

    if (connected) // send stream request to the server
    {
        host = net->get_local_ip();

        //create the server request (to get the stream)
        sprintf(request,
                "GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Accept: */*\r\n"
                "User-Agent: WiiRadio/1.0\r\n"
                "Icy-Metadata: 1\r\n\r\n\r\n",
                !(*path) ? "/" : path,host);

        int len_req = strlen(request);
        len_req = net->client_send(request,len_req);

    }else status = FAILED;


    g_pause_draw = false;
}

void check_keys()
{
    if (g_real_keys[SDLK_1] && !g_keys_last_state[SDLK_1])
    {
        if (g_screen_status != S_STREAM_INFO)
            g_screen_status = S_STREAM_INFO;
        else g_screen_status = S_BROWSER;
    }

    if (g_real_keys[SDLK_ESCAPE] && !g_keys_last_state[SDLK_ESCAPE])
    {
        if (g_screen_status != S_ABOUT)
            g_screen_status = S_ABOUT;
        else g_screen_status = S_BROWSER;
    }

    if (g_real_keys[SDLK_b] && g_screen_status != S_BROWSER) g_screen_status = S_BROWSER;
    if (g_real_keys[SDLK_PLUS] && status == PLAYING) request_save_fav(); // save playlist


    if (g_real_keys[SDLK_MINUS] && !g_keys_last_state[SDLK_MINUS])
    {
        mute ? mute = false : mute = true;

        if (mute)
        {
#ifdef _WII_
        MP3Player_Volume(0);
#endif
        }else{
#ifdef _WII_
        MP3Player_Volume(mp3_volume);
#endif
        }

    }

    if (g_real_keys[SDLK_DOWN]) {
        mp3_volume-=4;
        mp3_volume <= 0 ? mp3_volume = 0 : 0;
#ifdef _WII_
        MP3Player_Volume(mp3_volume);
#endif
        g_vol_lasttime = get_tick_count();
    }
    if (g_real_keys[SDLK_UP]) {
        mp3_volume+=4;
        mp3_volume >= 255 ? mp3_volume = 255 : 0;
#ifdef _WII_
        MP3Player_Volume(mp3_volume);
#endif
        g_vol_lasttime = get_tick_count();
    }


    if (g_real_keys[SDLK_2] && ! g_keys_last_state[SDLK_2])
        visualize ? visualize = false : visualize = true;

    if (g_real_keys[SDLK_RIGHT] && !g_keys_last_state[SDLK_RIGHT])
    {
        if (visualize)
        {
            if (visualize_number < MAX_VISUALS)
                visualize_number++;
        }

        if (g_screen_status == S_BROWSER)
        {
            if (display_idx > 1000) return;
            else display_idx += max_stations;

            ui->reset_scrollings();

        }

        if (g_screen_status == S_PLAYLISTS)
        {
             if (pls_display + MAX_BUTTONS  >= total_num_playlists) return;
             else pls_display += MAX_BUTTONS;

             ui->reset_scrollings();
        }

        if (g_screen_status == S_GENRES)
        {
            if (genre_display + MAX_BUTTONS  >= MAX_GENRE) return;
            else genre_display += MAX_BUTTONS;

            ui->reset_scrollings();
        }


    }

    if (g_real_keys[SDLK_LEFT] && !g_keys_last_state[SDLK_LEFT])
    {
        if (visualize)
        {
            if (visualize_number > 0)
                visualize_number--;
        }

        if (g_screen_status == S_BROWSER)
        {
            if (display_idx < 0) display_idx = 0;
            else display_idx -= max_stations;

            ui->reset_scrollings();
        }

        if (g_screen_status == S_PLAYLISTS)
        {
            pls_display -= MAX_BUTTONS;
            if (pls_display < 0) pls_display = 0;

            ui->reset_scrollings();
        }

        if (g_screen_status == S_GENRES)
        {
            genre_display -= MAX_BUTTONS;
            if (genre_display < 0) genre_display = 0;

            ui->reset_scrollings();
        }
    }
}


void search_genre(char*); //global extern
void search_genre(char* selected)
{
    refresh_genre_cache = true; // everytime we search for a new genre let's refresh the cache
    display_idx = 0;
    char path[200] = {0};

    // bring down upto 1k of stations per genre.  We will then cache them incase of shoutcast DB problems
    sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&genre=%s&limit=1000",selected);
    station_lister(path,selected);

}
void genre_nex_prev(bool,char*);//extern'd
void genre_nex_prev(bool n,char* genre)
{
    refresh_genre_cache = false; // use the cache

    if(n) display_idx += max_stations;
    else display_idx -= max_stations;

    if (display_idx <= 0 ) display_idx = 0;
}

void menu_button_click(int);
void menu_button_click(int id)
{
    g_screen_status = id;
}


// callback called from mod of libmad
void cb_fft(short* in, int max)
{
    if (visualize) // Only update if viewing
    {
        fourier->setAudioData(in);
        fourier->getFFT(ui->fft_results);
    }
}

void get_favorites()
{
    favs->load_favorites();
}

void request_save_fav(); // extern'd
void request_save_fav()
{
    favs->save_current(playing); // save to file then reload
    favs->load_favorites();
}

/*
called from libmad ....
return the data from the buffer or a default zero stream if we are having problems keeping up!
*/
s32 reader_callback(void *usr_data,void *cb,s32 len)
{
    return icy_info->get_buffer(cb,len);

}


#ifdef _WII_
void *critical_thread(void *arg) {
#else
void critical_thread(void *arg) {
#endif
    int len = 0;
    char* net_buffer = new char[2560];
    int errors = 0;

    while(g_running)
    {

        // network handler
        if (connected)
        {

            len = net->client_recv(net_buffer,2560);

            if(len > 0)
            {
                // reset errors
                errors = 0;

                // metaint handler
                len = icy_info->parse_metaint(net_buffer,len);

                // data handler
                icy_info->buffer_data(net_buffer,len);
#ifndef _WII_
                cb_fft((short*)net_buffer,len/2);
#endif

            }

			if (!icy_info->bufferring) // only play if we've buffered enough data
			{
#ifdef _WII_
				if(!MP3Player_IsPlaying())
				{
                    //modified function
					MP3Player_PlayFile(icy_info->buffer, reader_callback, 0, cb_fft);

					status = PLAYING;
				}
#else

                FMOD_BOOL is_playing = false;
                FMOD_Channel_IsPlaying(channel1,&is_playing);
                if (!is_playing)
                {
                    FMOD_System_CreateSound(fmod_system,(const char *)icy_info->buffer,FMOD_SOFTWARE | FMOD_OPENMEMORY_POINT | FMOD_CREATESTREAM | FMOD_CREATECOMPRESSEDSAMPLE,&exinfo, &sound1);
                    FMOD_Sound_SetMode(sound1, FMOD_LOOP_OFF);
                    FMOD_System_PlaySound(fmod_system,FMOD_CHANNEL_FREE, sound1, false, &channel1);
                    status = PLAYING;
                }
#endif
			}else{
			    status = BUFFERING;

                /* would block, not really an error in this case */
                if (len != -11) errors++;

                if (errors > 200) { // too many errors let's reset
                    status = FAILED;
                    if (connected) net->client_close();
                    connected = 0;
                    errors = 0;
                }
			}
	    }else{
	        errors = 0; // no rec errors
            Sleep(500); // don't hog if not connected
	    }

        Sleep(25); // minimum delay
    }

    delete [] net_buffer; net_buffer =0;
}

#ifdef _WII_
int evctr = 0;
void countevs(int chan, const WPADData *data) {
	evctr++;
}
#endif

int main(int argc, char **argv)
{
    display_idx = 0;
    favs_idx = 0;
	fullscreen = 0;
    refresh_genre_cache = true; // default is to refresh cache
    visualize = false;
    g_pause_draw = false;
    mp3_volume = 255; // max volume
    g_vol_lasttime = 0;
    visualize_number = 0;
    mute = false;

#ifdef _WII_
	fullscreen = 1;
	fatInitDefault();

	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, SCREEN_WIDTH, SCREEN_HEIGHT);
	WPAD_SetIdleTimeout(300);

#endif

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER  ) < 0 ) {
        exit(1);
    }

	if ( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, BITDEPTH,
			 (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF ) < 0 ) { //SDL_HWSURFACE
		exit(1);
	}

	SDL_ShowCursor(0);

	screen = NULL;
	screen = SDL_GetVideoSurface();

	if (!screen) exit(1);

    draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
    SDL_Flip(screen);

    // INIT
    tx              = new texture_cache;
    fnts            = new fonts;
    fnts->change_color(200,200,200);
    fnts->size = 8;
    fnts->text(screen,"Loading...",30,70,0);SDL_Flip(screen);
    fnts->text(screen,"Setting Up Network...",30,90,0);SDL_Flip(screen);
    net             = new network;
    scb             = new shoutcast_browser;
    playlst         = new playlists;
    fourier         = new fft;

    fnts->text(screen,"Loading UI...",30,110,0);SDL_Flip(screen);

    ui              = new gui(fnts,fourier);
    icy_info        = new icy;
    favs            = new favorites;
    playing         = new fav_item;

    get_favorites();
	g_running = true;

#ifdef _WII_
	LWP_CreateThread(&critical_handle,	critical_thread,NULL, NULL, 2*2560, 100);
#else
    _beginthread(critical_thread,0,0);
#endif

#ifdef _WII_
    ASND_Init();
    MP3Player_Init();
    SND_ChangeVolumeVoice(0,mp3_volume,mp3_volume);
#else

    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = icy_info->buffer_size;

    FMOD_System_Create(&fmod_system);
	FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);

#endif


    search_genre("pop"); // get list ...
    status = STOPPED;
    g_screen_status = S_BROWSER;
    DWORD last_time, current_time;
    last_time = current_time = get_tick_count();
    int fps = 40; //attempt 40fps


    while(g_running)
    {
        draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer

        loopi(MAX_KEYS) {
            g_keys_last_state[i] = g_real_keys[i];
            g_real_keys[i] = 0;
        }


#ifdef _WII_
		WPAD_ScanPads();

        u32 type;
		WPADData *wd_one;
		WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
		WPAD_Probe(WPAD_CHAN_ALL, &type);

		wd_one = WPAD_Data(0);

        translate_keys();
#else
		SDL_PumpEvents();
        g_nKetStatus = SDL_GetKeyState(NULL);
        loopi(MAX_KEYS) g_real_keys[i] = g_nKetStatus[i];
#endif

#ifdef _WII_

        if (wd_one->ir.valid)
        {
            event.motion.x = wd_one->ir.x;
            event.motion.y = wd_one->ir.y;

            if(g_real_keys[SDLK_RETURN] && !g_keys_last_state[SDLK_RETURN])  // (A) Mapped to SDLK_RETURN
                event.type = SDL_MOUSEBUTTONDOWN;
            else event.type = SDL_MOUSEBUTTONUP;

            ui->handle_events(&event);
        }
#else
       if (SDL_PollEvent( &event )) {
           ui->handle_events(&event);
       }
#endif

        check_keys();

        if (!g_pause_draw)
            draw_ui(0);


        // frame limit.....
        // never gets hit on Wii
        int delay = 1000 / fps - (current_time-last_time);
        last_time = current_time;
        if(delay > 0)
        {
            SDL_Delay(delay);
            current_time += delay;
        }



    }

    Sleep(2000); // YUK ! wait for thread to end
    // clean up
    stop_playback();

#ifdef _WIN32
    FMOD_Sound_Release(sound1);
	FMOD_System_Close(fmod_system);
	FMOD_System_Release(fmod_system);
#endif

    delete playing; playing = 0;
    delete favs; favs = 0;
    delete ui; ui = 0;
    delete fourier; fourier =0;
    delete playlst; playlst = 0;
    delete scb; scb=0;
    delete net; net =0;
    delete fnts; fnts = 0;
    delete icy_info; icy_info = 0;
    delete tx; tx = 0;

    SDL_Quit();

	return 0;

}
