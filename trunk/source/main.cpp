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
#include "gui/gui.h"
#include "station.h"

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
station             playing; // make now playing as fav struct so we can access the ip/name quickly
visualizer*         visuals;
langs*              lang;
skins*              sk;

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

// local storage of audio data
/*
    Using callbacks to call a class function seem to create unusual problems??? BUG ??? WTF IDK?
*/
unsigned char audio_data[8192];

// search options
struct _so
{
    char*   buf;
    int     search_type;
};
_so search_options;

#ifdef _WII_

// check to see where Wiiradio is, sd or usb
void prob_media(char* p)
{
    memset(g_storage_media,0,8);

    // this is here for wiiload
    if (!p)
    {
        strcpy(g_storage_media,"sd");
        return;
    }

    if (strstr(p,"usb:/"))
    {
        strcpy(g_storage_media,"usb");
        return;
    }

    if (strstr(p,"sd:/"))
    {
        strcpy(g_storage_media,"sd");
        return;
    }

    // older HBC versions !!!
    if (strstr(p,"fat:/"))
    {
        strcpy(g_storage_media,"sd");
        return;
    }

    // last chance .. default
    strcpy(g_storage_media,"sd");
}
#endif

void translate_keys()
{

#ifdef _WII_

    if (GetScreenStatus() == S_SEARCHING)
        return;

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_HOME)
    {
        g_real_keys[SDLK_ESCAPE] = 1;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT)
    {
        g_real_keys[SDLK_LEFT] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT)
    {
        g_real_keys[SDLK_RIGHT] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN)
    {
        g_real_keys[SDLK_DOWN] = 1;
    }
    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP)
    {
        g_real_keys[SDLK_UP] = 1;
    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
    {
        g_real_keys[SDLK_RETURN] = 1;

    }


    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_B)
    {
        g_real_keys[SDLK_b] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS)
    {
        g_real_keys[SDLK_PLUS] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS)
    {
        g_real_keys[SDLK_MINUS] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_1)
    {
        g_real_keys[SDLK_1] = 1;

    }

    if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_2)
    {
        g_real_keys[SDLK_2] = 1;

    }
#endif


}

void draw_ui(char* info)
{

    // drawing stuff
    ui->draw(scb->sl_first,icy_info,favs);
    if (info) ui->draw_info(info);
}


void station_lister(const char* path,char* gen)
{
    // rest gui pointer
    //ui->select_pos = 0;

    // connect and get the web page
    if(!scb->connect(path,gen,g_servicetype))    // parse the html and grab what we need
        scb->connect(path,gen,g_servicetype); // try again

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
    favs->delete_favorite(value);
}


// -- genere search thread
int search_thread(void* arg)
{
    _so* s = (_so*)arg;
    char path[512] = {0};

    // flag as searching so we don't try and draw the current station text ... the pointers will be deleted !!!
    SetScreenStatus(S_SEARCHING);

    Sleep(200); // give the ui time to finish what it's doing before requesting a new list!


    switch(s->search_type)
    {
    case SEARCH_GENRE:
        sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&genre=%s&limit=1000",s->buf);
        break;
    case SEARCH_STATIONS:
        sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&search=%s&limit=1000",s->buf);
        break;
    default:
        return 0;
    }
    // bring down upto 1k of stations per genre.  We will then cache them incase of shoutcast DB problems
    // sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&genre=%s&limit=1000",selected);
    station_lister(path,s->buf);

    SetScreenStatus(S_BROWSER); // default to browser as results will be here

    return 0;
}

// -- Connect thread
int connect_thread(void* arg)
{
    char* server = (char*)(playing.server.c_str());
    int port = playing.port;
    char* path = (char*)(playing.path.c_str());
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

    // connect to new stream
    int connect_try = net->client_connect(server,port,TCP,false);

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
        if (len_req < 0)
        {
            status = FAILED;
            return 0;
        }

        connected = connect_try;

    }
    else
    {
        status = FAILED;
    }

    return 0;
}


void split_url(char* o_url, char* o_path, int* o_port, char* url)
{
    if(!url) return;

    int part = 0;
    char local_url[MED_MEM];
    strcpy(local_url,url);

    char* split = local_url;

    // ignore http://
    char* search = strstr(local_url,"http://");


    // Clear data
    strcpy(o_url,  "");
    strcpy(o_path, "");
    *o_port = 0;

    search += strlen("http://");

    split = strtok(search,":/");
    while(split)
    {

        if(!part)
        {
            strcpy(o_url,split);
        }
        else if(part==1)
        {
            *o_port = atoi(split);
            if (*o_port == 0)
            {
                strcat(o_path,"/");
                strcat(o_path,split);
            }
        }
        else
        {
            strcat(o_path,"/");
            strcat(o_path,split);
        }

        split = strtok(0,":/");
        part++;
    }
}

void connect_direct(char* typed);
void connect_direct(char* typed)
{
    playing = station(typed);

    connect_to_stream(0,I_DIRECT);
};

void connect_to_stream(int,connect_info); //extern'd
void connect_to_stream(int value, connect_info info)
{
    if (connectthread)
    {
        SDL_WaitThread(connectthread, NULL); // wait for it to stop
        connectthread = 0;
    }

    status = CONNECTING; // attempting a new connection

    // Sort out data for stream to connect to
    switch (info)
    {
    case I_STATION:
    {
        value += display_idx;
        if (value > scb->total_items)
        {
            status = FAILED;
            return;
        }

        station_list* csl = scb->sl_first;
        // loop through stations
        int x = 0;
        while(csl)
        {
            if (x==value) break;

            csl = csl->nextnode;
            x++;
        }
        if (!csl)
        {
            status = FAILED;
            return;
        }

        // get playlists
        if (csl->service_type == SERVICE_SHOUTCAST)
        {
            playlst->get_playlist(csl->station_id);
        }
        else if (csl->service_type == SERVICE_ICECAST)
        {
            // no pls for ICEcast
            playlst->split_url(csl->station_id);
        }

        playing.server= playlst->first_entry->url;
        playing.port = playlst->first_entry->port;
        playing.path = playlst->first_entry->path;

        // Set standard port
        if (!playing.port)
        {
            playing.port = STD_STREAM_PORT;
        }

        //save to the now playing mem
        playing.name = csl->station_name;
        break;
    }
    case I_PLAYLIST:
    {
        playing = favs->list[value];

        // Set standard port
        if (!playing.port)
        {
            playing.port = STD_STREAM_PORT;
        }

        break;
    }
    case I_DIRECT:
        // do nothing
        break;

    case I_HASBEENSET:
        break;
    }
    favs->save_current(playing);

    // start a new connection thread
    connectthread = SDL_CreateThread(connect_thread,0);

}

bool screen_sleeping = false;
void screen_timeout()
{
    if (!visualize)

        // auto burnin reducer if there not viewing a visual
        loopi(MAX_KEYS)
    {
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
    else if (screen_sleeping)
    {
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

    // -- keys that always perform the same action go first!!!
    if (g_real_keys[SDLK_2] && ! g_keys_last_state[SDLK_2])
        visualize = !visualize;

    if (g_real_keys[SDLK_MINUS] && !g_keys_last_state[SDLK_MINUS])
    {
        mute ? mute = false : mute = true;

        if (mute)
        {
#ifdef _WII_
            MP3Player_Volume(0);
#else
            FMOD_Channel_SetVolume(channel1, 0.);
#endif
        }
        else
        {
#ifdef _WII_
            MP3Player_Volume(volume);
#else
            FMOD_Channel_SetVolume(channel1, volume/255.);
#endif
        }

    }


    if (g_real_keys[SDLK_DOWN])
    {

        mute = false;

        volume-=4;
        volume <= 0 ? volume = 0 : 0;
#ifdef _WII_
        MP3Player_Volume(volume);
#else
        FMOD_Channel_SetVolume(channel1, volume/255.);
#endif
        g_vol_lasttime = get_tick_count();
    }

    if (g_real_keys[SDLK_UP])
    {

        mute = false;

        volume+=4;
        volume >= 255 ? volume = 255 : 0;
#ifdef _WII_
        MP3Player_Volume(volume);
#else
        FMOD_Channel_SetVolume(channel1, volume/255.);
#endif

        g_vol_lasttime = get_tick_count();
    }


    if (g_real_keys[SDLK_1] && !g_keys_last_state[SDLK_1])
    {

        if (GetScreenStatus() != S_STREAM_INFO)
        {
            if (!visualize) SetScreenStatus(S_STREAM_INFO);
        }
        else if (GetScreenStatus() == S_STREAM_INFO)
        {
            SetLastScreenStatus();
        }

    }

    if (g_real_keys[SDLK_ESCAPE] && !g_keys_last_state[SDLK_ESCAPE] && !visualize)
    {
        if (GetScreenStatus() != S_OPTIONS)
            SetScreenStatus(S_OPTIONS);
        else SetLastScreenStatus();
    }

    if (g_real_keys[SDLK_b] && GetScreenStatus() != S_BROWSER) SetScreenStatus(S_BROWSER);

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

        switch(GetScreenStatus())
        {
        case S_BROWSER:

            if (display_idx > MAX_STATION_CACHE) return;
            else display_idx += max_listings;

            ui->reset_scrollings();

            break;

        case S_PLAYLISTS:

            if (pls_display + max_listings  >= total_num_playlists) return;
            else pls_display += max_listings;

            ui->reset_scrollings();
            break;

        case S_GENRES:

            //if (genre_display + max_listings  >= MAX_GENRE) return;
            if (genre_display + max_listings  >= ui->gl.total) return;
            else genre_display += max_listings;

            ui->reset_scrollings();

            break;

        default:
            break;

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

        switch(GetScreenStatus())
        {
        case S_BROWSER:

            if (display_idx < 0) display_idx = 0;
            else display_idx -= max_listings;

            ui->reset_scrollings();
            break;

        case S_PLAYLISTS:

            pls_display -= max_listings;
            if (pls_display < 0) pls_display = 0;

            ui->reset_scrollings();
            break;

        case S_GENRES:

            genre_display -= max_listings;
            if (genre_display < 0) genre_display = 0;

            ui->reset_scrollings();
            break;

        default:
            break;
        }
    }


    screen_timeout();
}


void search_function(char*,int); //global extern
void search_function(char* value,int search_type)
{
    refresh_genre_cache = true; // everytime we search for a new genre let's refresh the cache
    display_idx = 0;

    search_options.buf = value;
    search_options.search_type = search_type;

    if (search_type == SEARCH_CONNECT)
    {
        // TO do
        connect_direct(value);
        return;
    }

    if (search_type == SEARCH_STATIONS)
    {
        // can't do this yet with icecast
        g_servicetype = SERVICE_SHOUTCAST;
    }

    if (searchthread)
    {
        SDL_WaitThread(searchthread, NULL); // wait for it to stop
        searchthread = 0;
    }


    // start a new connection thread
    searchthread = SDL_CreateThread(search_thread,(void*)&search_options);

}

void genre_nex_prev(bool,char*);//extern'd
void genre_nex_prev(bool n,char* genre)
{
    refresh_genre_cache = false; // use the cache

    if(n) display_idx += max_listings;
    else display_idx -= max_listings;

    if (display_idx <= 0 ) display_idx = 0;
}

#ifndef STD_MAD
// callback called from mod of libmad
static void cb_fft(unsigned char* in, int max)
{

    if (g_reloading_skin || !g_running) return; // -- bad ui is loading

    if (visualize || ui->vis_on_screen) // Only update if viewing
    {
        loopi(max) audio_data[i] = in[i];
        //fourier->setAudioData(in,max);
        //fourier->getFFT(visuals->fft_results);
    }
}
#endif

void get_favorites()
{
    favs->load_favorites();
}

void request_save_fav(); // extern'd
void request_save_fav()
{
    favs->save_fav(playing); // save to file then reload
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

    net_buffer = (char*)memalign(32,MAX_NET_BUFFER);
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
                // reset errors
                errors = 0;

                // data handler
                icy_info->buffer_data(net_buffer,len);

#ifndef _WII_
                // just emulate the audio data on windows for now
                loopi(8192) audio_data[i] = rand() % (i + 8000);
#endif
            }
            else if (len < 0)
            {
                /* would block, not really an error in this case */
                if (len != -EAGAIN) errors++; // -11

                if (errors > 300)
                {
                    // -- too many errors let's reset
                    status = FAILED;
                    continue;
                }
            }
            if (!net->client_connected)
            {
                status = FAILED;
            }
            // Too many nested reconnects
            if (icy_info->redirect_detected > 20)
            {
                status = FAILED;
            }
            else if (icy_info->redirect_detected)
            {
                playing = station(icy_info->icy_url);
                connect_to_stream(0, I_HASBEENSET);
            }

            if (!icy_info->bufferring) // only play if we've buffered enough data
            {
#ifdef _WII_
                if(!MP3Player_IsPlaying())
                {
                    if(connected)
                    {
#ifdef STD_MAD
                        MP3Player_PlayFile(icy_info->buffer, reader_callback, 0);
#else
                        //modified function
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

        }
        else
        {

            errors = 0; // no rec errors
            Sleep(500); // don't hog if not connected

        }

        Sleep(15); // minimum delay
    }

    free(net_buffer);
    net_buffer =0;
    if (connected) net->client_close();

    return 0;
}

#ifdef _WII_

int evctr = 0;
void countevs(int chan, const WPADData *data)
{
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
    volume = 255; // max volume
    g_vol_lasttime = 0;
    visualize_number = 0;
    mute = false;
    SDL_Event event;
    int cursor_x = 0;
    int cursor_y = 0;
    float cursor_angle = 0.;
    int cursor_visible = 1;

#ifdef _WII_
    fullscreen = 1;
    fatInitDefault();

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    // -- *2 so we don't lagg when hitting the right or bottom of the screen
    WPAD_SetVRes(WPAD_CHAN_ALL, SCREEN_WIDTH +2*SCREEN_WIDTH_BUFFER,
                 SCREEN_HEIGHT+2*SCREEN_HEIGHT_BUFFER);
    WPAD_SetIdleTimeout(200);

    // Wii Power/Reset buttons
    WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
    SYS_SetPowerCallback(ShutdownCB);
    //SYS_SetResetCallback(ResetCB);


    // where is wiiradio ?
    prob_media(argv[0]);


#endif

    // load options
    load_options();

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER  ) < 0 )
    {
        printf("SDL_Init() failed\n");
        exit(1);
    }

    if ( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, BITDEPTH,
                           (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF ) < 0 )   //SDL_HWSURFACE
    {
        printf("SDL_SetVideoMode() failed\n");
        exit(1);
    }

    SetWidescreen(); // adjust the screen depending on the widescreen option

    last_button_time = get_tick_count();
    SDL_ShowCursor(0);

    screen = NULL;
    screen = SDL_GetVideoSurface();

    if (!screen)
    {
        printf("SDL_GetVideoSurface() failed\n");
        exit(1);
    }

    draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
    SDL_Flip(screen);

    // INIT
    fnts            = new fonts;
    fnts->change_color(200,200,200);
    fnts->set_size(FS_SYSTEM);
    fnts->text(screen,"Loading...",30,70,0);
    SDL_Flip(screen);
    fnts->text(screen,"Setting Up Network...",30,90,0);
    SDL_Flip(screen);
    net             = new network;
    scb             = new shoutcast_browser;
    playlst         = new playlists;
    fourier         = new fft;
    visuals         = new visualizer(fourier);
    icy_info        = new icy;
    favs            = new favorites;
    sk              = new skins;
    lang            = new langs;

    get_favorites();


#ifdef _WII_
    ASND_Init();
    MP3Player_Init();
    SND_ChangeVolumeVoice(0,volume,volume);
#else


    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = icy_info->buffer_size;

    FMOD_System_Create(&fmod_system);
#ifdef _LINUX_
    FMOD_System_SetOutput(fmod_system, FMOD_OUTPUTTYPE_ALSA);
#endif
    FMOD_System_Init(fmod_system, 32, FMOD_INIT_NORMAL, NULL);

#endif

// create a new reader thread
    g_running = true;
    mainthread = SDL_CreateThread(critical_thread,0);
    if (!mainthread) exit(0);

    SetScreenStatus(S_BROWSER);
    status = STOPPED;

    Uint64 last_time, current_time;
    last_time = current_time = get_tick_count();

// restart stream if requested
    if (g_startfromlast != 0)
    {
        FILE * file = fopen(make_path("current.pls"),"r");
        vector<favorite> parsed = favs->parse_items_pls(file,make_path("current.pls"));
        if (parsed.size() && parsed[0].server.length())
        {
            playing = parsed[0];
            connect_to_stream(0,I_DIRECT);
        }
    }
_reload:

    if (g_reloading_skin)   // only if we have a skin open
    {

        char* sp = sk->get_current_skin_path();
        if (sp) strcpy(g_currentskin,sp); // save as option

    }
    else
    {
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
    fnts->change_color(200,200,200);
    fnts->set_size(FS_SYSTEM);
    fnts->text(screen,"Loading Skin...",30,110,0);
    SDL_Flip(screen);

    tx              = new texture_cache;
    ui              = new gui(fnts,visuals,g_currentskin);


    if (!g_reloading_skin)
    {
        // If there are playlists, show them first
        if (total_num_playlists)
        {
            SetScreenStatus(S_PLAYLISTS);
            // If there are no playlists, show genre list
        }
        else
        {
            // first time so get list ...
            //search_function((char*)"dance",SEARCH_GENRE);
            SetScreenStatus(S_GENRES);
        }
    }

    g_running = true;
    g_reloading_skin = false;

    while(g_running)
    {

        loopi(MAX_KEYS)
        {
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

        // Some valid dummy value
        event.type = SDL_USEREVENT;
        if (wd_one->ir.valid)
        {
            cursor_angle = -wd_one->ir.angle;

            event.type = SDL_MOUSEMOTION;
            event.motion.x = wd_one->ir.x-SCREEN_WIDTH_BUFFER;
            event.motion.y = wd_one->ir.y-SCREEN_HEIGHT_BUFFER;
            cursor_x = event.motion.x;
            cursor_y = event.motion.y;
            cursor_visible = 1;

            // (A) Mapped to SDLK_RETURN
            if(g_real_keys[SDLK_RETURN] && !g_keys_last_state[SDLK_RETURN])
            {
                event.type = SDL_MOUSEBUTTONDOWN;
                event.button.x = event.motion.x;
                event.button.y = event.motion.y;
            }
            if(!g_real_keys[SDLK_RETURN] && g_keys_last_state[SDLK_RETURN])
            {
                event.type = SDL_MOUSEBUTTONUP;
                event.button.x = event.motion.x;
                event.button.y = event.motion.y;
            }

            ui->handle_events(&event);
        }
        else
        {
            cursor_visible = 0;
        }
#else
        while (SDL_PollEvent( &event ))
        {
            if (event.type == SDL_MOUSEMOTION)
            {
                cursor_x = event.motion.x;
                cursor_y = event.motion.y;
            }

            ui->handle_events(&event);
        }
#endif

        check_keys();

        draw_ui(0);
        if (cursor_visible && !visualize)
        {
#ifdef _WII_
            ui->draw_cursor(cursor_x, cursor_y, cursor_angle);
#else
            ui->draw_cursor(cursor_x - sk->get_value_int("cursor_x_off"),
                            cursor_y - sk->get_value_int("cursor_y_off"),
                            cursor_angle);
#endif
        }
        // flip to main screen buffer
        // SDL_Rect ds = { 100,100,640,480};
        SDL_BlitSurface(ui->guibuffer,0,screen,0);
        SDL_Flip(screen);

        // do the fft using the local
        if (visualize || ui->vis_on_screen) // Only update if viewing
        {
            if(status == PLAYING)
            {
                fourier->setAudioData((short*)audio_data,8192);
                fourier->getFFT(visuals->fft_results);
            }
        }
#ifdef _WII_
        Sleep(5);
#else
        Sleep(20);
#endif
    }

    delete ui;
    ui = 0;
    delete tx;
    tx = 0;

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
    delete favs;
    delete visuals;
    delete fourier;
    delete playlst;
    delete scb;
    delete net;
    delete fnts;
    delete icy_info;

    SDL_Quit();

    exit(0);

}
