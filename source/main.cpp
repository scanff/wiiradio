#include "globals.h"
#include "options.h"
//#include "fonts.h"
#include "client.h"
#include "browser.h"
#include "playlists.h"
#include "favorites.h"
#include "fft.h"
#include "icy.h"
#include "visuals.h"
#include "gui.h"

// ---------------------
// Variables:
// ---------------------

int         fullscreen;
int         connected = 0;
int         display_idx;
int         favs_idx;
int         genre_display = 0;
int         total_num_playlists = 0;
int         pls_display = 0;

Uint64      last_button_time;
bool        g_critical_running;

texture_cache*      tx;
SDL_Surface*        screen;
fonts*              fnts; //extern this to use everywhere
icy*                icy_info;
shoutcast_browser*  scb;
playlists*          playlst;
network*            net;
gui*                ui;
fft*                fourier;
favorites*          favs;
fav_item*           playing; // make now playing as fav struct so we can access the ip/name quickly
visualizer*         visuals;
langs*              lang;
skins*              sk;
int                 g_value;
bool                g_haveplaylist;

#ifndef _WII_

FMOD_SYSTEM             *fmod_system = 0;
FMOD_SOUND              *sound1 = 0 ;
FMOD_CHANNEL            *channel1 = 0;
FMOD_CREATESOUNDEXINFO  exinfo;

#endif

int critical_thread (void *arg); // -- main playback and net thread
SDL_Thread* mainthread = 0;

int connect_thread (void *arg); // -- thread for connection request
SDL_Thread* connectthread = 0;

int search_thread (void *arg); // -- thread for genre searches
SDL_Thread* searchthread = 0;


void translate_keys()
{

#ifdef _WII_


    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_HOME){
        g_real_keys[SDLK_ESCAPE] = 1;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT){
        g_real_keys[SDLK_LEFT] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT){
        g_real_keys[SDLK_RIGHT] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
         g_real_keys[SDLK_DOWN] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
         g_real_keys[SDLK_UP] = 1;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
         g_real_keys[SDLK_RETURN] = 1;

    }


    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) {
         g_real_keys[SDLK_b] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS) {
         g_real_keys[SDLK_PLUS] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS) {
         g_real_keys[SDLK_MINUS] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_1) {
         g_real_keys[SDLK_1] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_2) {
         g_real_keys[SDLK_2] = 1;

    }
#endif


}

void draw_ui(char* info)
{

    // drawing stuff
    ui->draw(scb->sl_first,icy_info,favs);
    if (info) ui->draw_info(info);
    // flip to main screen buffer
    //SDL_Rect ds = { 100,100,640,480};
    SDL_BlitSurface(ui->guibuffer,0,screen,0);
  /*char out[100]={0};
                fnts->change_color(10,10,10);
                sprintf(out,"x= %u y=%u",event.motion.x,event.motion.y);
                fnts->text(screen,out,30,90,0);
*/
    SDL_Flip(screen);

}


void station_lister(const char* path,char* gen)
{
    // rest gui pointer
    //ui->select_pos = 0;

    // connect and get the web page
    if(!scb->connect(path,gen))    // parse the html and grab what we need
        scb->connect(path,gen); // try again

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


// -- genere search thread
int search_thread(void* arg)
{
    char* selected = (char*)arg;
    char path[512] = {0};

    // flag as searching so we don't try and draw the current station text ... the pointers will be deleted !!!
    g_screen_status = S_SEARCHING;

    Sleep(200); // give the ui time to finish what it's doing before requesting a new list!

    // bring down upto 1k of stations per genre.  We will then cache them incase of shoutcast DB problems
    sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&genre=%s&limit=1000",selected);
    station_lister(path,selected);

    g_screen_status = S_BROWSER;

	return 0;
}

// -- Connect thread
int connect_thread(void* arg)
{
    char* url = playing->station_url;
    int port = playing->port;
    char* path = playing->station_path;
    char* host = 0;
    char request[1024] = {0};


    // -- close current connection
    if (connected)
    {
        connected = 0;
        net->client_close();
    }

    // stop mp3 if currently playing.
    stop_playback();


    if (!g_haveplaylist) {

        g_value += display_idx;
        if (g_value > scb->total_items)
        {
            status = FAILED;
            return 0;
        }

        station_list* csl = scb->sl_first;
        // loop through stations
        int x = 0;
        while(csl){
            if (x==g_value) break;

            csl = csl->nextnode;
            x++;
        }
        if (!csl) {
            status = FAILED;
            return 0;
        }

        // get playlists
        playlst->get_playlist(csl->station_id);
        //playlst->parse_playlist();

        url = playlst->first_entry->url;
        port = playlst->first_entry->port;
        path = playlst->first_entry->path;

        //save to the now playing mem
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
            if (x==g_value) break;

            csl = csl->nextnode;
            x++;
        }
        if (!csl) {
            status = FAILED;
            return 0;
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



    // connect to new stream
    int connect_try = net->client_connect(url,port,TCP,false);

    if (status != CONNECTING) return 0; // cancelled !

    icy_info->icy_reset(); // reset icy state

    if (connect_try) // send stream request to the server
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

        connected = connect_try;

    }else status = FAILED;

    return 0;
}


void connect_to_stream(int,bool); //extern'd
void connect_to_stream(int value,bool haveplaylist)
{
    g_value = value;
    g_haveplaylist = haveplaylist;

    if (connectthread)
    {
        SDL_WaitThread(connectthread, NULL); // wait for it to stop
        connectthread = 0;
    }

    status = CONNECTING; // attempting a new connection

    // start a new connection thread
    connectthread = SDL_CreateThread(connect_thread,0);

}

bool screen_sleeping = false;
void screen_timeout()
{
    if (!visualize)

    // auto burnin reducer if there not viewing a visual
    loopi(MAX_KEYS) {
        if (g_real_keys[i]) last_button_time = get_tick_count();
    }


    unsigned int calc_timeout = 0;

    switch (g_screensavetime)
    {
        case 0:
        calc_timeout = (1000*60); //1 mins
        break;

        case 1:
        calc_timeout = (1000*300); //5 mins
        break;

        case 2:
        calc_timeout = (1000*600); //10 mins
        break;

        case 3:
        return;

    };

    if (get_tick_count() - last_button_time > calc_timeout)
        screen_sleeping = true;
    else if (screen_sleeping) {
        screen_sleeping = false;
        visualize = false;
        visualize_number = 0; //reset
    }


    if (!visualize && screen_sleeping)
    {
        visualize_number = MAX_VISUALS;
        visualize = true;
    }
}

void check_keys()
{
    // -- send the keys to the ui (mainly the visuals) and see if they peform a fifferent action!
    if (!ui->override_keys())
    {
        // -- keys that always perform the same action go first!!!
        if (g_real_keys[SDLK_2] && ! g_keys_last_state[SDLK_2])
            visualize ? visualize = false : visualize = true;


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

            mute = false;

            mp3_volume-=4;
            mp3_volume <= 0 ? mp3_volume = 0 : 0;
#ifdef _WII_
            MP3Player_Volume(mp3_volume);
#endif
            g_vol_lasttime = get_tick_count();
        }

        if (g_real_keys[SDLK_UP]) {

            mute = false;

            mp3_volume+=4;
            mp3_volume >= 255 ? mp3_volume = 255 : 0;
#ifdef _WII_
            MP3Player_Volume(mp3_volume);
#endif

            g_vol_lasttime = get_tick_count();
        }


        if (g_real_keys[SDLK_1] && !g_keys_last_state[SDLK_1])
        {

            if (g_screen_status != S_STREAM_INFO) {
                if (!visualize) g_screen_status = S_STREAM_INFO;
            }else if (g_screen_status == S_STREAM_INFO)  { g_screen_status = S_BROWSER; }


        }

        if (g_real_keys[SDLK_ESCAPE] && !g_keys_last_state[SDLK_ESCAPE] && !visualize)
        {
            if (g_screen_status != S_OPTIONS)
                g_screen_status = S_OPTIONS;
            else g_screen_status = S_BROWSER;
        }

        if (g_real_keys[SDLK_b] && g_screen_status != S_BROWSER) g_screen_status = S_BROWSER;
        if (g_real_keys[SDLK_PLUS] && status == PLAYING)
            request_save_fav(); // save playlist


        if (g_real_keys[SDLK_RIGHT] && !g_keys_last_state[SDLK_RIGHT])
        {
            if (visualize)
            {
                if (visualize_number < MAX_VISUALS)
                    visualize_number++;

                return;
            }

            if (g_screen_status == S_BROWSER)
            {
                if (display_idx > MAX_STATION_CACHE) return;
                else display_idx += max_listings;

                ui->reset_scrollings();

            }

            if (g_screen_status == S_PLAYLISTS)
            {
                 if (pls_display + max_listings  >= total_num_playlists) return;
                 else pls_display += max_listings;

                 ui->reset_scrollings();
            }

            if (g_screen_status == S_GENRES)
            {
                //if (genre_display + max_listings  >= MAX_GENRE) return;
                if (genre_display + max_listings  >= ui->gl.total) return;
                else genre_display += max_listings;

                ui->reset_scrollings();
            }


        }

        if (g_real_keys[SDLK_LEFT] && !g_keys_last_state[SDLK_LEFT])
        {
            if (visualize)
            {
                if (visualize_number > 0)
                    visualize_number--;

                return;
            }

            if (g_screen_status == S_BROWSER)
            {
                if (display_idx < 0) display_idx = 0;
                else display_idx -= max_listings;

                ui->reset_scrollings();
            }

            if (g_screen_status == S_PLAYLISTS)
            {
                pls_display -= max_listings;
                if (pls_display < 0) pls_display = 0;

                ui->reset_scrollings();
            }

            if (g_screen_status == S_GENRES)
            {
                genre_display -= max_listings;
                if (genre_display < 0) genre_display = 0;

                ui->reset_scrollings();
            }
        }
    } // ui over ride keys ?

    screen_timeout();
}


void search_genre(char*); //global extern
void search_genre(char* selected)
{
    refresh_genre_cache = true; // everytime we search for a new genre let's refresh the cache
    display_idx = 0;

    if (searchthread)
    {
        SDL_WaitThread(searchthread, NULL); // wait for it to stop
        searchthread = 0;
    }


    // start a new connection thread
    searchthread = SDL_CreateThread(search_thread,(void*)selected);

}

void genre_nex_prev(bool,char*);//extern'd
void genre_nex_prev(bool n,char* genre)
{
    refresh_genre_cache = false; // use the cache

    if(n) display_idx += max_listings;
    else display_idx -= max_listings;

    if (display_idx <= 0 ) display_idx = 0;
}


// callback called from mod of libmad
void cb_fft(short* in, int max)
{
    if (g_reloading_skin || !g_running) return; // -- bad ui is loading

    if (visualize || ui->vis_on_screen) // Only update if viewing
    {
        fourier->setAudioData(in);
        fourier->getFFT(visuals->fft_results);
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

// shoutcast playback
int errors = 0;
int critical_thread(void *arg)
{
    int len = 0;
    char* net_buffer = 0;

    net_buffer = new char[MAX_NET_BUFFER];
    if (!net_buffer) exit(0);

    g_critical_running = true;

    while(g_critical_running)
    {
        // stream handler
        if (connected)
        {
            // stream handler
            len = net->client_recv(net_buffer,MAX_NET_BUFFER);

            if(len > 0)
            {
               /* char out[100]={0};
                fnts->change_color(10,10,10);
                sprintf(out,"mi= %d len= %d buf=%u  - bufsz =%u",icy_info->icy_metaint,len,icy_info->buffered,icy_info->buffer_size);
                fnts->text(screen,out,30,90,0);SDL_Flip(screen);
                */
                // reset errors
                errors = 0;

                // metaint handler
                len = icy_info->parse_metaint(net_buffer,len);

                // data handler
                icy_info->buffer_data(net_buffer,len);
#ifndef _WII_
                cb_fft((short*)net_buffer,len/2);
#endif

            }else if(len < 0){

                /* would block, not really an error in this case */
                if (len != -11) errors++;

                if (errors > 100)
                {   // -- too many errors let's reset
                    status = FAILED;
                }
            }


            if (!icy_info->bufferring) // only play if we've buffered enough data
            {

#ifdef _WII_
                if(!MP3Player_IsPlaying())
                {
                    //modified function
                    if(connected)
                    {
#ifdef STD_MAD
                        MP3Player_PlayFile(icy_info->buffer, reader_callback, 0);
#else
                        MP3Player_PlayFile(icy_info->buffer, reader_callback, 0, cb_fft);
#endif
                        status = PLAYING;
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
                    status = PLAYING;
                }
#endif
            }

            // -- stream connection was cancelled or timed out
            if (status == STOPPED || status == FAILED)
            {
                if (connected) net->client_close();
                stop_playback(); // if playing stop!
                connected = 0;
                errors = 0;
            }

	    }else{

	        errors = 0; // no rec errors
            Sleep(500); // don't hog if not connected

	    }

        Sleep(15); // minimum delay
    }

    delete [] net_buffer; net_buffer =0;
    if (connected) net->client_close();

    return 0;
}

#ifdef _WII_

int evctr = 0;
void countevs(int chan, const WPADData *data) {
	evctr++;
}

//exit function
void ShutdownCB()
{
	g_running = false;
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
	// -- *2 so we don't lagg when hitting the right or bottom of the screen
    WPAD_SetVRes(WPAD_CHAN_ALL, SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
	WPAD_SetIdleTimeout(200);

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	//SYS_SetResetCallback(ResetCB);

#endif

    // load options
    load_options();

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER  ) < 0 ) {
        printf("SDL_Init() failed\n");
        exit(1);
    }

	if ( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, BITDEPTH,
			 (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF ) < 0 ) { //SDL_HWSURFACE
    printf("SDL_SetVideoMode() failed\n");
		exit(1);
	}

    last_button_time = get_tick_count();
	SDL_ShowCursor(0);

	screen = NULL;
	screen = SDL_GetVideoSurface();

	if (!screen) {
    printf("SDL_GetVideoSurface() failed\n");
    exit(1);
  }

    draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
    SDL_Flip(screen);

    // INIT
    fnts            = new fonts;
    fnts->change_color(200,200,200); fnts->set_size(FS_SYSTEM);
    fnts->text(screen,"Loading...",30,70,0);SDL_Flip(screen);
    fnts->text(screen,"Setting Up Network...",30,90,0);SDL_Flip(screen);
    net             = new network;
    scb             = new shoutcast_browser;
    playlst         = new playlists;
    fourier         = new fft;
    visuals         = new visualizer(fourier);
    icy_info        = new icy;
    favs            = new favorites;
    playing         = new fav_item;
    sk              = new skins;
    lang            = new langs;

    get_favorites();


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

 // create a new reader thread
    g_running = true;
    mainthread = SDL_CreateThread(critical_thread,0);
    if (!mainthread) exit(0);

    g_screen_status = S_BROWSER;
    status = STOPPED;

    Uint64 last_time, current_time;
    last_time = current_time = get_tick_count();
 //   int fps = 40; //attempt 60fps




_reload:

    if (g_reloading_skin) { // only if we have a skin open

        char* sp = sk->get_current_skin_path();
        if (sp) strcpy(g_currentskin,sp); // save as option

    }else{
        if (!*g_currentskin)
            strcpy(g_currentskin,"skins/wiiradio/"); // -- load default

        sk->set_current_skin(g_currentskin);
    }
    // -- Clear All Vars
    vars.delete_all();

    // -- language
    if (!*g_currentlang)
    {
        strcpy(g_currentlang,"English"); // default
    }

    lang->load_lang(g_currentlang);

    // --- skin
    fnts->change_color(200,200,200); fnts->set_size(FS_SYSTEM);
    fnts->text(screen,"Loading Skin...",30,110,0);SDL_Flip(screen);

    tx              = new texture_cache;
    ui              = new gui(fnts,visuals,g_currentskin);


    //if (!g_reloading_skin) search_genre((char*)"pop"); // first time so get list ...

	g_running = true;
    g_reloading_skin = false;

    while(g_running)
    {

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
//            cursor_rot = wd_one->orient.roll; // rotation

            event.motion.x = wd_one->ir.x;
            event.motion.y = wd_one->ir.y;


            if(g_real_keys[SDLK_RETURN] && !g_keys_last_state[SDLK_RETURN])  // (A) Mapped to SDLK_RETURN
                event.type = SDL_MOUSEBUTTONDOWN;
            else event.type = SDL_MOUSEBUTTONUP;

            ui->handle_events(&event);
        }else{
            // place the cursor off screen so it does not act laggy, when infact it's just got negitive co-ords
            event.motion.x = 1000;
            event.motion.y = 1000;
        }
#else
       while (SDL_PollEvent( &event )) {
           ui->handle_events(&event);
       }
#endif

        check_keys();

        if (!g_pause_draw) draw_ui(0);


        // frame limit.....
        // never gets hit on Wii
     /*   int delay = 1000 / fps - (current_time-last_time);
        last_time = current_time;
        if(delay > 0)
        {
            SDL_Delay(delay);
            current_time += delay;
        }
     */
        Sleep(5);
    }

    delete ui; ui = 0;
    delete tx; tx = 0;

    // --- skin
    if (g_reloading_skin) goto _reload;

    g_critical_running = false; // -- stop the network / playback thread
    SDL_WaitThread(mainthread, NULL);

    save_options(); // save options

    // clean up
    stop_playback();

#ifndef _WII_
    FMOD_Sound_Release(sound1);
	FMOD_System_Close(fmod_system);
	FMOD_System_Release(fmod_system);
#endif

    delete lang;
    delete sk;
    delete playing;
    delete favs;
    delete visuals;
    delete fourier;
    delete playlst;
    delete scb;
    delete net;
    delete fnts;
    delete icy_info;

    SDL_Quit();

	return 0;

}
