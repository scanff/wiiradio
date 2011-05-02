#include "globals.h"

#ifdef _WII_
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
//#include <di/di.h>
//#include <ogc/dvd.h>
#endif
#include "variables.h"
#include "fonts.h"
#include "options.h"
#include "station.h"
#include "genre_list.h"
#include "textures.h"
#include "skins.h"
#include "lang.h"
#include "client.h"
#include "browser.h"
#include "playlists.h"
#include "favorites.h"
#include "fft.h"
#include "icy.h"
#include "local.h"
#include "localfiles.h"
#include "visuals.h"
#include "projectm.h"
#include "gui.h"
#include "audio/sound_sdl.h"
#include "script.h"

#include "application.h"


// ---------------------
// Variables:
// ---------------------
int         favs_idx = 0;
int         fullscreen;
int         errors = 0; // Network errors
static _so  search_options;

#ifdef _WII_

short active_ir = 0;
char localip[16];
char gateway[16];
char netmask[16];

void countevs(int chan, const WPADData *data);
void ShutdownCB();

#endif

static int critical_thread (void *arg); // -- main playback and net thread
static SDL_Thread* mainthread = 0;

static int search_thread (void *arg); // -- main playback and net thread
static SDL_Thread* searchthread = 0;


// Mutex
SDL_mutex*          connect_mutex;
SDL_mutex*          script_mutex;

////////////////////////
////////////////////////
////////////////////////

app_wiiradio wrApp;

int main(int argc, char **argv)
{
    wrApp.wrMain(argc, argv);
    SDL_Quit();
    return 0;
    //exit(0);
}
app_wiiradio* GetApp() { return &wrApp; };

////////////////////////
////////////////////////
////////////////////////


app_wiiradio::app_wiiradio() :
#ifdef _WII_
    sd(&__io_wiisd),
    usb(&__io_usbstorage),
//    dvd(&__io_wiidvd),
#endif
    unsaved_volume_change( false ),
    screen_sleeping( false ),
    playing_uuid( 0 ),
    playing_item( -1 ),
    replay_item( 0 ),
    current_screen_status( 0 ),
    last_screen_status( 0 ),
    app_timer( 0 ),
    current_lv( 2 ),
    exit_mode( 0 ),
    sleep_time_start( 0 )
{


    connect_mutex = SDL_CreateMutex();
    script_mutex = SDL_CreateMutex();

    lua_script = new script(this);
    vars = new wiiradio_variables(this);
    settings = new wr_options(this);


}

app_wiiradio::~app_wiiradio()
{
    if(script_mutex) SDL_DestroyMutex(script_mutex);
    if(connect_mutex) SDL_DestroyMutex(connect_mutex);

    delete settings;
    delete vars;
    delete lua_script;
}


inline void app_wiiradio::translate_keys()
{

#ifdef _WII_


    u16 buttonsHeld = 0;
    loopi(MAX_WPAD) buttonsHeld |= WPAD_ButtonsHeld(i);

    if(buttonsHeld & WPAD_BUTTON_HOME)
    {
        keys_buttons.current_keys[KEY_ESCAPE] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_LEFT)
    {
        keys_buttons.current_keys[KEY_LEFT] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_RIGHT)
    {
        keys_buttons.current_keys[KEY_RIGHT] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_DOWN)
    {
        keys_buttons.current_keys[KEY_DOWN] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_UP)
    {
        keys_buttons.current_keys[KEY_UP] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_A)
    {
        //keys_buttons.downtime[KEY_RETURN] = app_timer;
        keys_buttons.current_keys[KEY_RETURN] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_B)
    {
        keys_buttons.current_keys[KEY_b] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_PLUS)
    {
        keys_buttons.current_keys[KEY_PLUS] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_MINUS)
    {
        keys_buttons.current_keys[KEY_MINUS] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_1)
    {
        keys_buttons.current_keys[KEY_1] = 1;
    }

    if(buttonsHeld & WPAD_BUTTON_2)
    {
        keys_buttons.current_keys[KEY_2] = 1;
    }
#endif


}

void app_wiiradio::set_widescreen(const int ws)
{
#ifdef _WII_
    GetSettings()->owidescreen = ws;
	// Will need latest SVN version of SDL as of 3/16/10

    WII_SetWidescreen(GetSettings()->owidescreen);

#endif
}

void app_wiiradio::seek_playback(const int position)
{
    if(GetSystemStatus() != PLAYING || playback_type != AS_LOCAL)
        return;

    SDL_mutexP(connect_mutex);

    const u32 seconds = (((float)GetAudio()->GetSongLen() / 100.0f) * position);
    const u32 seek_pos = seconds * (GetAudio()->GetBitRate()/8);


    GetAudio()->sound_pause(true);

    u32 filesize = localpb->get_size();

    GetAudio()->sound_seek(seek_pos,filesize);

    localpb->seek(seek_pos);

    GetAudio()->sound_pause(false);

    SDL_mutexV(connect_mutex);
}

void app_wiiradio::delete_playlist(const int value)
{
    favs->delete_favorite(value);
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

void app_wiiradio::connect_direct(const char* typed)
{
    playing = station(typed);

    connect_to_stream(0,I_DIRECT);
}


void app_wiiradio::connect_to_stream(const int value, connect_info info)
{

    current_info.info = info;
    current_info.value = value;

    SDL_mutexP(script_mutex);
    SetSystemStatus(CONNECTING);
    SDL_mutexV(script_mutex);

}


void app_wiiradio::screen_timeout()
{
    if (GetScreenStatus() == S_VISUALS)
        return;

    // auto burnin reducer if there not viewing a visual

    unsigned int calc_timeout = 0;

    switch (GetSettings()->oscreensavetime)
    {
    case 0:
        return;
    case 1:
        calc_timeout = (1000*60); //1 mins
        break;
    case 2:
        calc_timeout = (1000*300); //5 mins
        break;
    case 3:
        calc_timeout = (1000*600); //10 mins
        break;
    };

    if (app_timer - last_button_time > calc_timeout)
        screen_sleeping = true;

    else if (screen_sleeping)
    {
        screen_sleeping = false;
    }

}


inline void app_wiiradio::check_keys()
{

    // -- keys that always perform the same action go first!!!
    if (keys_buttons.current_keys[KEY_2] && ! keys_buttons.last_keys[KEY_2])
    {
        last_button_time = app_timer;

        if (GetScreenStatus() == S_MAIN)
        {
            if(!settings->ovisual_mode)
            {
                switch_sdl_video_mode(VIDM_GX);
                #ifndef _WII_ //reinit on win/lin when switching to SDL GL mode
                    prjm->init();
                #endif
            }else{
                switch_sdl_video_mode(VIDM_SDL);
            }

            SetScreenStatus(S_VISUALS);
        }
        else
        {
            switch_sdl_video_mode(VIDM_SDL);
            SetScreenStatus(S_MAIN);
        }

    }
#ifndef _WII_
    if (keys_buttons.current_keys[KEY_F10] && !keys_buttons.last_keys[KEY_F10])
    {
        last_button_time = app_timer;

        toggle_fullscreen();
    }
#endif
    if (keys_buttons.current_keys[KEY_MINUS] && !keys_buttons.last_keys[KEY_MINUS])
    {
        last_button_time = app_timer;

        mute ? mute = false : mute = true;

        if (mute)
        {
            audio_dev->sound_volume(0);
        }
        else
        {
            audio_dev->sound_volume(GetSettings()->ovolume);
        }

    }


    if (keys_buttons.current_keys[KEY_DOWN])
    {
        last_button_time = app_timer;

        mute = false;

        GetSettings()->ovolume-=4;
        GetSettings()->ovolume <= 0 ? GetSettings()->ovolume = 0 : 0;

        audio_dev->sound_volume(GetSettings()->ovolume);

        vol_lasttime = app_timer;
        unsaved_volume_change = true;
    }

    if (keys_buttons.current_keys[KEY_UP])
    {
        last_button_time = app_timer;

        mute = false;

        GetSettings()->ovolume+=4;
        GetSettings()->ovolume >= 255 ? GetSettings()->ovolume = 255 : 0;
        audio_dev->sound_volume(GetSettings()->ovolume);

        vol_lasttime = app_timer;
        unsaved_volume_change = true;
    }


    if (keys_buttons.current_keys[KEY_1] && !keys_buttons.last_keys[KEY_1])
    {
        last_button_time = app_timer;

        SDL_mutexP(script_mutex);
        GetScript()->OnKey("1"); // runs lua
        SDL_mutexV(script_mutex);
        return;
      //  current_screen_status = GetScript()->GetVariableInt("SYS_SCREEN");
/*
        if (GetScreenStatus() != S_STREAM_INFO && GetScreenStatus() != S_VISUALS)
        {
            SetScreenStatus(S_STREAM_INFO);
        }
        else if (GetScreenStatus() == S_STREAM_INFO)
        {
            SetLastScreenStatus();
        }
*/
    }

    if (keys_buttons.current_keys[KEY_ESCAPE] && !keys_buttons.last_keys[KEY_ESCAPE] && (GetScreenStatus() != S_VISUALS))
    {
        last_button_time = app_timer;

        if (GetScreenStatus() != S_OPTIONS)
            SetScreenStatus(S_OPTIONS);
        else SetLastScreenStatus();
    }

    if (keys_buttons.current_keys[KEY_b] && !keys_buttons.last_keys[KEY_b])
    {
        last_button_time = app_timer;

        if((GetListView() == S_LOCALFILES))
            localfs->dirup();

    }

    if (keys_buttons.current_keys[KEY_PLUS] && (PLAYING == GetSystemStatus()))
    {
        last_button_time = app_timer;
        request_save_fav(); // save playlist
    }

    if (keys_buttons.current_keys[KEY_RIGHT] && !keys_buttons.last_keys[KEY_RIGHT])
    {
        last_button_time = app_timer;

        if (GetScreenStatus() == S_VISUALS)
        {
            if (visualize_number < MAX_VISUALS)
            {
                visualize_number++;
            }


            return;
        }

        switch(GetListView())
        {
         case S_GENRES:
         {
            gl.MoveNext();
         }break;

        case S_BROWSER:
        {
            scb->MoveNext();
        }break;

        case S_PLAYLISTS:
        {
            favs->MoveNext();
        }break;

        case S_LOCALFILES:
        {

            const int fp = localfs->GetPosition();
            if (fp + max_listings  >= localfs->total_num_files) return;
            else localfs->SetPosition(fp + max_listings);

        }
        break;

        default:
            break;

        }

    }

    if (keys_buttons.current_keys[KEY_LEFT] && !keys_buttons.last_keys[KEY_LEFT])
    {
        last_button_time = app_timer;

        if ((GetScreenStatus() == S_VISUALS))
        {
            if (visualize_number > 0)
                visualize_number--;

            return;
        }

        switch(GetListView())
        {
        case S_BROWSER:
        {
            scb->MoveBack();
        }break;

        case S_PLAYLISTS:
        {
            favs->MoveBack();
        }break;

        case S_GENRES:
        {
            gl.MoveBack();
        }break;

        case S_LOCALFILES:
        {
            const int fp = localfs->GetPosition();

            if (fp - max_listings  < 0) localfs->SetPosition(0);
            else localfs->SetPosition(fp - max_listings);

        }
        break;


        default:
            break;
        }
    }

}


void app_wiiradio::check_sleep_timer()
{
    // Check if sleep timer is enabled
    if (!GetSettings()->osleep_timer_time)
        return;

    u64 seconds;
    switch(GetSettings()->osleep_timer_time)
    {
      case 1: seconds =    5*60; break;
      case 2: seconds =   15*60; break;
      case 3: seconds =   30*60; break;
      case 4: seconds =   60*60; break;
      case 5: seconds = 3*60*60; break;
      default:seconds =      10; break;
    }
    if (app_timer - sleep_time_start > seconds * 1000)
    {
        exit_mode = 1;
        g_running = false;
    }
}

void app_wiiradio::search_function(const char* value, const int search_type)
{
    if(!value || strlen(value) >= SMALL_MEM)
        return;

    //thread
    if (searchthread)
    {
        SDL_WaitThread(searchthread, NULL);
        searchthread = 0;
    }


    strcpy(search_options.buf,value);
    search_options.search_type = search_type;

    searchthread = SDL_CreateThread(search_thread,this);

}


// callback called from mod of libmad
static void cb_fft(unsigned char* in, int max, app_wiiradio* theapp)
{

    if (theapp->reloading_skin || !g_running) return; // -- bad ui is loading

    if ((theapp->GetScreenStatus() == S_VISUALS) || theapp->ui->vis_on_screen) // Only update if viewing
    {
        loopi(max) theapp->audio_data[i] = in[i];
    }
}


void app_wiiradio::get_favorites()
{
    favs->load_favorites();
}

void app_wiiradio::request_save_fav()
{
    favs->save_fav(playing); // save to file then reload
    favs->load_favorites();
}

/*
called from libmad ....
return the data from the buffer or a default zero stream if we are having problems keeping up!
*/
s32 reader_callback(void *usr_data,void *cb,s32 len,app_wiiradio* theapp)
{
    const int st = theapp->GetSystemStatus();

    if ((st != PLAYING) && (st != LOCALFILE_BUFFERING) )//&& (st != BUFFERING))
        return 0;

    switch(playback_type)
    {
        case AS_LOCAL:
            return theapp->GetLocFile()->read((u8*)cb,len);
        break;

        case AS_NETWORK:
            return theapp->icy_info->get_buffer(cb,len);
        break;
		case AS_NULL:
		return 0;

    }
    return 0;
}


int app_wiiradio::network_playback(char* net_buffer)
{
    // stream handler
    const int len = net->client_recv(net_buffer,MAX_NET_BUFFER);

    if(len > 0)
    {
        // reset errors
        errors = 0;

        // data handler
        icy_info->buffer_data(net_buffer,len);

    }
    else if (len < 0)
    {
        /* would block, not really an error in this case */
        if (len != -11) errors++; // -11

        if (errors > 10000)
        {
            return FAILED;
        }
    }

    if (!net->client_connected)
    {
        return FAILED;
    }

    // Too many nested reconnects
    if (icy_info->redirect_detected > 20)
    {
        return FAILED;
    }
    else if (icy_info->redirect_detected)
    {
        return REDIRECT;
    }

    if (!icy_info->bufferring) // only play if we've buffered enough data
    {
        if(net->client_connected && GetSystemStatus() == BUFFERING)
        {
            audio_dev->sound_start(10,0);
            return PLAYING;
        }
    }else{
        if(!buffering_start)
            buffering_start = app_timer;

        if((app_timer - buffering_start) > BUFFERING_TIMEOUT)
        {
            return FAILED;
        }

        //audio_dev->sound_pause(1);
        return BUFFERING;
    }

    return -1;

}

void app_wiiradio::pause_playback(const bool p)
{
    audio_dev->sound_pause(p);
}

int app_wiiradio::GetSongPos()
{
    if (((GetSystemStatus()!=PAUSE) && (GetSystemStatus()!=PLAYING)) || (playback_type != AS_LOCAL))
        return 0;

    const float max = GetAudio()->GetSongLen();
    const float now = GetAudio()->GetSongPlayed();

    return (int)((100.0 / max) * now);

}



int app_wiiradio::new_connection()
{

    audio_dev->sound_stop();

    localpb->m_id3.FreeAPIC();


    // Sort out data for stream to connect to
    switch (current_info.info)
    {
    case I_LOCAL:
        if (localfs->list[current_info.value]->isfolder)
        {
            return -1; // cant play a folder !!!
        }

        playing.local = true;
        playing.name  = localfs->list[current_info.value]->name;
        playing.path  = localfs->list[current_info.value]->path;
        playing_item = current_info.value; // store item playing
        playing.server = localfs->current_path; // used as path for local
        // save the currently playing dir count.... needed when moving to next rack


    break;
    case I_STATION:
    {
        if (current_info.value > GetBrowser()->GetSize())
        {

            return FAILED;
        }

        station_list* csl = GetBrowser()->GetItem(current_info.value);

        if (!csl)
        {
            return FAILED;
        }

        // get playlists
        switch(csl->service_type)
        {
            case SERVICE_SHOUTCAST:
            {
                if(playlst->get_playlist(csl->station_playing) < 0)
                {
                    return FAILED;
                }
            }
            break;

            case SERVICE_ICECAST:
            {
                // no pls for ICEcast
                playlst->split_url(csl->station_id);
            }
            break;
        }

        playing.server = playlst->first_entry.url;
        playing.port = playlst->first_entry.port;
        playing.path = playlst->first_entry.path;

        // Set standard port
        if (!playing.port)
        {
            playing.port = STD_STREAM_PORT;
        }

        //save to the now playing mem
        playing.name = csl->station_name;
        playing.local = false;


    }break;
    case I_PLAYLIST:
    {
        favorite* c = (favorite*)favs->GetItem(current_info.value);//favs->list[value];

        if (!c)
        {
            return FAILED;
        }

        playing.isfolder = c->isfolder;
        playing.port = c->port;
        playing.local = c->local;
        playing.path = c->path;
        playing.name = c->name;
        playing.server = c->server;


        // Set standard port
        if (!playing.port)
        {
            playing.port = STD_STREAM_PORT;
        }

        if (playing.local)
        {

            size_t filesep;
            string p = playing.path;

            filesep = p.find_last_of("/\\");
            playing.server = "";
            playing.server = p.substr(0,filesep) + "/";

            current_info.info = I_LOCAL; // change the type
        }

    }break;
    case I_DIRECT:
        // do nothing
        break;

    case I_HASBEENSET:
        break;
    }

  //  if (current_info.info != I_LOCAL)
  //      favs->save_current(playing);

    const char* server = playing.server.c_str();
    int port = playing.port;
    const char* path = playing.path.c_str();
    bool islocal =  playing.local;

    char* host = 0;
    char request[MED_MEM];

    playback_type = AS_NULL;

    // uuid for highlighting the track/item playing
    playing_path = playing.path;

    if (!islocal)
    {
        // -- close current connection
        net->client_close();


        // connect to new stream
        net->client_connect((char*)server,port,TCP);

        if (net->client_connected) // send stream request to the server
        {
            icy_info->icy_reset(); // reset icy state

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
                net->client_close();
                return FAILED;
            }

            buffering_start = app_timer;
            playback_type = AS_NETWORK; // or icecast .. should rename to AS_NETWORK ?

            return BUFFERING;

        }
        else
        {

            playback_type = AS_NULL;
            return FAILED;
        }

    }
    else
    {
        playback_type = AS_LOCAL;
        return START_PLAYING_FILE;

    }

    return -1;
}

static int search_thread(void *arg)
{
    app_wiiradio* app = (app_wiiradio*)arg;

    SDL_mutexP(connect_mutex);

    SDL_mutexP(script_mutex);
    app->SetWNDStatus( 8 );
    SDL_mutexV(script_mutex);

    if (search_options.search_type == SEARCH_CONNECT)
    {
        // TO do
        app->connect_direct(search_options.buf);
        SDL_mutexP(script_mutex);
        app->SetWNDStatus( 0 );
        SDL_mutexV(script_mutex);
        SDL_mutexV(connect_mutex);
        return 0;
    }

    if (search_options.search_type == SEARCH_STATIONS)
    {
        // can't do this yet with icecast
        app->GetSettings()->oservicetype = SERVICE_SHOUTCAST;
    }

    static char path[MED_MEM];


    switch(search_options.search_type)
    {
    case SEARCH_GENRE:
    {
        // navi-x only seems to want the search with first char upper and the rest lower
        char tmp_gen[SMALL_MEM];
        strcpy(tmp_gen,search_options.buf);

        tmp_gen[0] = toupper(tmp_gen[0]);
        sprintf(path,"/shoutcast/shout.py?genre=%s",tmp_gen);
    }break;

    case SEARCH_STATIONS:
        sprintf(path,"/sbin/newxml.phtml?service=winamp2&no_compress=1&search=%s&limit=1000",search_options.buf);
        break;

    default:
        SDL_mutexV(connect_mutex);
        return 0;
    }

    const int serv_type = app->GetSettings()->oservicetype;

    app->GetBrowser()->connect(path,search_options.buf,serv_type); // try again

    SDL_mutexP(script_mutex);
    app->SetWNDStatus( 0 );
    app->SetListView(S_BROWSER); // show the browser list
    SDL_mutexV(script_mutex);


    SDL_mutexV(connect_mutex);

    return 0;

}

static int critical_thread(void *arg)
{
    app_wiiradio* app = (app_wiiradio*)arg;

    char* net_buffer = 0;

    net_buffer = (char*)memalign(32,MAX_NET_BUFFER);
    if (!net_buffer) exit(0);

    app->critical_running = true;

    while(app->critical_running)
    {
        usleep(450); // -- running too fast will slow down the gui

        SDL_mutexP(connect_mutex);

        const int status = app->GetSystemStatus();
        int result = -1;

        switch(status)
        {
            case CONNECTING:
            {
                errors = 0;

                result = app->new_connection();

            }
            break;

            case BUFFERING:
            case PLAYING:
            {

                switch(playback_type)
                {
                    case AS_NETWORK:
                        if (app->net->client_connected)
                        {
                            result = app->network_playback(net_buffer);
                        }
                    break;
                    default:
                    break;

                }

            }break;
            default:
            break;

        }

        SDL_mutexV(connect_mutex);

        if(result == REDIRECT)
        {

            app->playing = station(GetApp()->icy_info->icy_url);
            app->connect_to_stream(0, I_HASBEENSET);

        }
        else if (result >= 0)
        {
            SDL_mutexP(script_mutex);
            app->SetSystemStatus(result);
            SDL_mutexV(script_mutex);
        }

    }

    if( net_buffer )
    {
        free(net_buffer);
        net_buffer = NULL;
    }

    app->net->client_close();

    return 0;
}

void app_wiiradio::next_skin()
{
    sk->goto_next_skin();
    reloading_skin = true;
    g_running = false;
}

void app_wiiradio::next_lang()
{
    lang->goto_next_lang();
    lang->load_lang(lang->current_name);
    strcpy(GetSettings()->ocurrentlang,lang->current_name);
    // sorry have to reload skin right now as all button text needs updating
    reloading_skin = true;
    g_running = false;
}

#ifdef _WII_

static int evctr = 0;
void countevs(int chan, const WPADData *data)
{
    evctr++;
}

//exit function
void ShutdownCB()
{
    GetApp()->exit_mode = 1;
    g_running = false;
}

// check to see where Wiiradio is, sd or usb
void app_wiiradio::prob_media(char* p)
{
    memset(g_storage_media,0,8);

    // mount the deviced
    const char* devices[] = {"sd","usb"};
    const DISC_INTERFACE* disc[] = {sd, usb};

    for(int i = 0; i < DEV_MAX; i++)
    {
        int retries = 20;
        s_devices[i].dev_mounted = false;
        sprintf(s_devices[i].name,"%s:", devices[i]);

        fatUnmount(s_devices[i].name);

        while(retries--)
        {
            if(disc[i]->startup() && fatMountSimple(devices[i], disc[i]))
            {
                s_devices[i].dev_mounted = true;
                break;
            }
            usleep(200);
        }
    }

    if ( s_devices[DEV_SD].dev_mounted && s_devices[DEV_USB].dev_mounted )
    {
        /* where is WiiRadio Running from ? User has both USB and SD mounted */

        if (!p) // wiiload ?
        {
            strcpy(g_storage_media,"sd");
            return;
        }

        if (strstr(p,"sd:/"))
        {
            strcpy(g_storage_media,"sd");
            return;
        }

        if (strstr(p,"usb:/"))
        {
            strcpy(g_storage_media,"usb");
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

    }else{
        if(s_devices[DEV_SD].dev_mounted)
        {
            strcpy(g_storage_media,"sd");
        }
        else if(s_devices[DEV_USB].dev_mounted)
        {
            strcpy(g_storage_media,"usb");

        }else{ // -- hmmm Wiiload with nothing plugged in ???
            exit(0);
        }
    }


}

void app_wiiradio::unmount()
{


        // Unmount
    char name[20];
    const char* devices[] = {"sd:","usb:"};
	const DISC_INTERFACE* disc[] = {sd, usb};

    for(int i = 0; i < DEV_MAX; i++)
    {
        if(s_devices[i].dev_mounted)
        {
            sprintf(name, devices[i]);
            disc[i]->shutdown();

            fatUnmount(name);
        }
    }
}

const bool app_wiiradio::check_media(const char* m)
{

    const DISC_INTERFACE* disc[] = {sd, usb};
    const char* devices[] = {"sd:","usb:"};

    for(int i = 0; i < DEV_MAX; i++)
    {
        if (!strcmp(m,devices[i]))
        {
            if (!s_devices[i].dev_mounted || !disc[i]->isInserted())
            {
                return false;
            }
            return true;
        }
    }
    return false;
                // -- device was never mounted
           /*     if(disc[i]->startup() && disc[i]->isInserted())
                {
                    if(fatMountSimple(devices[i], disc[i])
                    {
                        s_devices[i].dev_mounted = true;
                    }
                }
            }else{
                if(disc[i]->isInserted())
                {
                    // -- been inserted
                    if(disc[i]->startup())
                    {
                        s_devices[i].dev_mounted = true;
                    }
                }
            }
        }

    }*/

}

#endif


// switch SDL video on / off
static _VideoMode sdl_video_mode_last = VIDM_SDL;

#ifndef _WII_
void app_wiiradio::toggle_fullscreen()
{
    fullscreen = !fullscreen;
    video_flags ^= SDL_FULLSCREEN;
    if ( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, BITDEPTH, video_flags) < 0 )
    {
        exit(1); // handle better
    }

    screen = SDL_GetVideoSurface();

    if (!screen)
    {
        printf("SDL_GetVideoSurface() failed\n");
        exit(1);
    }
}
#endif


void app_wiiradio::switch_sdl_video_mode(const _VideoMode mode)
{
    if (mode == sdl_video_mode_last) return;

#ifdef _WII_
    GXColor background = { 0, 0, 0, 0xff };
    switch(mode)
    {
        case 0:
            WII_VideoStop();
        break;
        case 1:
            GX_SetCopyClear (background, 0x00ffffff);
            GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
            GX_SetAlphaUpdate(GX_FALSE);
            GX_SetColorUpdate(GX_TRUE);
            WII_VideoStart();
        break;
    }

#endif

    sdl_video_mode_last = mode;
}

void app_wiiradio::SetScreenStatus(u32 s)
{
    if (s == current_screen_status) return;

    ui->HideScreens(s);

    last_screen_status = current_screen_status;
    current_screen_status = s;

    GetScript()->SetVariableInt("SYS_SCREEN",(const int)(s));


    if(last_screen_status != current_screen_status)
    {
        ui->screen_changed(current_screen_status,last_screen_status);
    }
}

int app_wiiradio::GetScreenStatus()
{
    return current_screen_status;
}

void app_wiiradio::SetLastScreenStatus()
{
    SetScreenStatus(last_screen_status);
}
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
int app_wiiradio::GetListView()
{
    return current_lv;
}
void app_wiiradio::SetListView(u32 s)
{
    if (s == current_lv) return;

    // Hide other lists and show this one
    ui->HideLists(s);

    last_lv = current_lv;
    current_lv = s;


    GetScript()->SetVariableInt("SYS_LISTVIEW",(const int)(s));

}
///////////////////////////////////
int app_wiiradio::GetSystemStatus()
{
    return current_ss;
}

void app_wiiradio::SetSystemStatus(u32 s)
{
    if (s == current_ss) return;

    last_ss = current_ss;
    current_ss = s;
    GetScript()->SetVariableInt("SYS_STATUS",(const int)(s));

}

////
const int app_wiiradio::GetWNDStatus()
{
    return current_wnd;
}


void app_wiiradio::SetWNDStatus(const u32 s)
{
    if (s == current_wnd) return;

    last_wnd = current_wnd;
    current_wnd = s;
    GetScript()->SetVariableInt("WND_STATUS",(const int)(s));

}

////
void app_wiiradio::load_splash(const char* text)
{
    draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
    fnts.text(screen,text,30,70,0);
    SDL_Flip(screen);
}

FORCEINLINE void app_wiiradio::do_input()
{
    SDL_Event event;

    loopi(KEY_MAX)
    {
        keys_buttons.last_keys[i] = keys_buttons.current_keys[i];
        keys_buttons.current_keys[i] = 0;
    }


#ifdef _WII_

        WPAD_ScanPads();

        u32 type;
        WPADData *wd[MAX_WPAD];
        WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
        WPAD_Probe(WPAD_CHAN_ALL, &type);

        // Get the data of all wpads
        loopi(MAX_WPAD) wd[i] = WPAD_Data(i);

        // Look for potential new active ir wpad to be used in wiiradio
        if (!wd[active_ir]->ir.valid)
        {
            active_ir = 0;
            while(active_ir < MAX_WPAD-1 &&
                  (!wd[active_ir]->ir.valid ||
                  wd[active_ir]->ir.x-  SCREEN_WIDTH_BUFFER  < 0 ||
                  wd[active_ir]->ir.x+2*SCREEN_WIDTH_BUFFER  > SCREEN_WIDTH ||
                  wd[active_ir]->ir.y-  SCREEN_HEIGHT_BUFFER < 0 ||
                  wd[active_ir]->ir.y+2*SCREEN_HEIGHT_BUFFER > SCREEN_HEIGHT) )
                active_ir++;
        }

        translate_keys();

#else
        SDL_PumpEvents();
        g_nKetStatus = SDL_GetKeyState(NULL);

        loopi(KEY_MAX)
            keys_buttons.current_keys[i] = g_nKetStatus[remap_keys[i]];
#endif

#ifdef _WII_

        // Some valid dummy value
        event.type = SDL_USEREVENT;
        if (wd[active_ir]->ir.valid)
        {
            cursor_angle = -wd[active_ir]->ir.angle;

            event.type = SDL_MOUSEMOTION;
            event.motion.x = wd[active_ir]->ir.x-SCREEN_WIDTH_BUFFER;
            event.motion.y = wd[active_ir]->ir.y-SCREEN_HEIGHT_BUFFER;
            cursor_x = event.motion.x;
            cursor_y = event.motion.y;
            cursor_visible = 1;

            // (A) Mapped to KEY_RETURN
            if(keys_buttons.current_keys[KEY_RETURN] && !keys_buttons.last_keys[KEY_RETURN])
            {
                event.type = SDL_MOUSEBUTTONDOWN;
                event.button.x = event.motion.x;
                event.button.y = event.motion.y;
            }
            if(!keys_buttons.current_keys[KEY_RETURN] && keys_buttons.last_keys[KEY_RETURN])
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
            ui->handle_events(&event);
        }
#endif

        check_keys();


}

FORCEINLINE void app_wiiradio::run_scrips()
{
    SDL_mutexP(script_mutex);

    // this is defined if the skin file has a lua function main
    GetScript()->CallVoidVoidFunction("main");
    if (playback_type == AS_LOCAL) GetScript()->SetVariableString("SONG_POS",GetAudio()->get_position_fmtd());

    const int s1 = GetScript()->GetVariableInt("SYS_SCREEN");
    const int s2 = GetScript()->GetVariableInt("SYS_LISTVIEW");
    const int s3 = GetScript()->GetVariableInt("SYS_STATUS");

    SetScreenStatus(s1);
    SetListView(s2);
    SetSystemStatus(s3);


    SDL_mutexV(script_mutex);
}

FORCEINLINE void app_wiiradio::check_status()
{
    const int stat = GetSystemStatus();

    switch(stat)
    {
        case PLAYING:
        {

            if(playback_type == AS_LOCAL && GetAudio()->sound_done() && GetLocFile()->done)
            {
                GetAudio()->sound_stop();
                if (replay_item)
                {
                    connect_to_stream(playing_item,I_LOCAL);
                }else{
                    // Move to next song in list
                    playing_item++;
                    if (playing_item >= 0 && playing_item < localfs->current_playing_dir_count)
                    connect_to_stream(playing_item,I_LOCAL);
                }

            }

        }break;

        case START_PLAYING_FILE:
        {
            GetAudio()->sound_stop();

            if(GetLocFile()->open(playing.path.c_str()))
            {
                GetAudio()->sound_start(10,GetLocFile()->m_id3.filesize);
                SDL_mutexP(script_mutex);
                SetSystemStatus(LOCALFILE_BUFFERING);
                SDL_mutexV(script_mutex);
                playback_type = AS_LOCAL;
            }
        }
    }

}

void app_wiiradio::non_critical()
{
    screen_timeout();
    check_sleep_timer();

        // Check for unsaved volume changes and save if not changed for some time
        //   to avoid too many write accesses. At the moment this time corresponds
        //   to the time the volume "OSD" disappears from the screen
     //   if (unsaved_volume_change && (get_tick_count() - g_vol_lasttime > 2000))
     //   {
     //       GetSettings()->save_options();
     //       unsaved_volume_change = false;
     //   }


}

int app_wiiradio::net_setup()
{

    net_ok = true;

#ifdef _WII_

    int ret = 0;
    for(int i=0;i<MAX_INIT_RETRIES && (ret=net_init())==-EAGAIN;i++);

    net_ok = false;

          if(ret >= 0)
          {
              if (if_config ( localip, netmask, gateway, TRUE) < 0)
              {
                  return 0;
              }
          }else{
              return 0;
          }

    net_ok = true;
    return 1;
#else

#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 0);
    if(WSAStartup(wVersionRequested, &wsaData) != 0) return 0;

    return 1;
#endif
    return 1;
#endif

    return 0;

}
int app_wiiradio::wrMain(int argc, char **argv)
{

    fullscreen = 0;
    vol_lasttime = 0; // prevent the volume bar from showing on startup
    visualize_number = 0;
    mute = false;
    buffering_start = 0;
    reloading_skin = false;

#ifdef _WII_

    cursor_x = 0;
    cursor_y = 0;
    cursor_angle = 0.;
    cursor_visible = 1;

    fatInitDefault();

    prob_media(argv[0]);

    fullscreen = 1;

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


#endif


    lua_script->init();

    // load options
    GetSettings()->load_options();

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER  ) < 0 )
    {
        exit(1);
    }

    /** Setup some window stuff */
#ifndef _WII_
    SDL_WM_SetCaption( "WiiRadio", NULL );
    video_flags = (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGLBLIT | SDL_OPENGL;
#else
    video_flags = (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF /*| SDL_OPENGLBLIT | SDL_OPENGL*/;
#endif

    // default SDL Video flags .. start window ... but allow fullscreen



    if ( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, BITDEPTH, video_flags) < 0 )
    {
        exit(1);
    }


    set_widescreen(GetSettings()->owidescreen); // adjust the screen depending on the widescreen option

    last_button_time = get_tick_count();
#ifdef _WII_
    SDL_ShowCursor(0);
#else
    SDL_ShowCursor(1);
#endif
    screen = NULL;
    screen = SDL_GetVideoSurface();

    if (!screen)
    {
        exit(1);
    }

    draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear backbuffer
    SDL_Flip(screen);

    // INIT
    fnts.init();
    fnts.change_color(200,200,200);
    fnts.set_size(FS_SYSTEM);

    load_splash("Setting Up Network...");
    if(!net_setup())
    {
        load_splash("Error Setting Up Network !!!");
        usleep(5000);
    }

    load_splash("Loading...");

    net             = new network(this);
    scb             = new browser(this);
    playlst         = new playlists(this);
    fourier         = new fft;
    visuals         = new visualizer(this);
    icy_info        = new icy(this);
    favs            = new favorites(this);
    sk              = new skins(this);
    lang            = new langs(this);
    localpb         = new local_player(this);
    localfs         = new localfiles(this);
    prjm            = new projectm(this);
    audio_dev       = new audio_device(this,reader_callback,cb_fft);

    // Set the current volume to the saved one
    audio_dev->sound_volume(GetSettings()->ovolume);

    SetSystemStatus(STOPPED);
    get_favorites();

    // thread, handles data input

    g_running = true;
    mainthread = SDL_CreateThread(critical_thread,this);
    if (!mainthread) exit(0);


_reload:

    if (reloading_skin)   // only if we have a skin open
    {

        char* sp = sk->get_current_skin_path();
        if (sp) strcpy(GetSettings()->ocurrentskin,sp); // save as option

    }
    else
    {
        if (!*GetSettings()->ocurrentskin)
            strcpy(GetSettings()->ocurrentskin,"data/skins/wrblue/"); // -- load default

        sk->set_current_skin(GetSettings()->ocurrentskin);
    }



    // -- Clear All Vars
    vars->delete_all();

    GetScript()->AddVariable("WR_SCREEN_WIDTH","640");
    GetScript()->AddVariable("WR_SCREEN_HEIGHT","480");

    // -- language
    if (!*settings->ocurrentlang)
    {
        strcpy(settings->ocurrentlang,"English"); // default
        GetScript()->AddVariable("WR_LANGUAGE","English");
    }

    lang->load_lang(settings->ocurrentlang);

    // --- skin
    fnts.change_color(200,200,200);
    fnts.set_size(FS_SYSTEM);

    load_splash("Loading Skin...");

    tx              = new texture_cache(this);
    ui              = new gui(this);


    SetWNDStatus( 0 );

    // -- set the max
    GetGenreList()->SetMax(max_listings);
    GetBrowser()->SetMax(max_listings);
    GetFavorites()->SetMax(max_listings);

    if (!reloading_skin)
    {
        // If there are playlists, show them first
        if (favs->GetSize())
        {
            SetListView(S_PLAYLISTS);
            // If there are no playlists, show genre list
        }
        else
        {
            SetListView(S_GENRES);
        }
        // restart stream if requested
        if (GetSettings()->ostartfromlast != 0)
        {
            FILE * file = fopen(make_path("data/current.pls"),"r");
            if (file) {
                vector<favorite> parsed = favs->parse_items_pls(file,make_path("data/current.pls"));
                if (parsed.size() && parsed[0].server.length())
                {
                    playing = parsed[0];
                    connect_to_stream(0,I_DIRECT);
                }
            }
        }
    }

    g_running = true;
    reloading_skin = false;

    last_time = 0;

    SetWNDStatus( 0 ); // NOTHING

    u64 last_time_nc = get_tick_count();

    // Main loop
    while(g_running)
    {
        app_timer = SDL_GetTicks();

        run_scrips(); // Lua scripting

        do_input(); // Handle mouse/mote/keys



        // do the fft using the local
        if ((GetScreenStatus() == S_VISUALS) || ui->vis_on_screen) // Only update if viewing
        {
            if(GetSystemStatus() == PLAYING)
            {
                fourier->setAudioData((short*)audio_data,8192/2);
                fourier->getFFT(visuals->fft_results);

            }else{
                // zero out the samples
                memset(audio_data,0,8192);
            }
        }

        ui->draw();



#ifdef _WII_

        if (cursor_visible && (GetScreenStatus() != S_VISUALS))
            ui->draw_cursor(cursor_x, cursor_y, cursor_angle);

        if(screen_sleeping)
        {
            draw_rect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0x000000);
        }


        SDL_Flip(screen);
#else

        if ((GetScreenStatus() != S_VISUALS) || GetSettings()->ovisual_mode)
        {
            if(screen_sleeping)
            {
                draw_rect(ui->guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0x000000);
            }
            SDL_BlitSurface(ui->guibuffer,0,screen,0);
            SDL_Flip(screen);
        }

        SDL_GL_SwapBuffers();

#endif


        if((app_timer - last_time_nc) > ONE_SECOND)
        {
            non_critical();
            last_time_nc = app_timer;
        }

        /*
            .... Hummm putting this in the thread locks up everything on wii

            seems that > 1 fopen within the thread locks everything ... can't be arsed to spend anymore
            time right now.
        */

        check_status();


        last_time = app_timer;
        usleep(100);

    }

    delete ui;
    ui = 0;
    delete tx;
    tx = 0;

    // --- skin
    if (reloading_skin) goto _reload;

    critical_running = false; // -- stop the network / playback thread
    SDL_WaitThread(mainthread, NULL);

    GetSettings()->save_options(); // save options

    // clean up
    audio_dev->sound_close();

    delete audio_dev;
    delete prjm;
    delete localfs;
    delete localpb;
    delete lang;
    delete sk;
    delete favs;
    delete visuals;
    delete fourier;
    delete playlst;
    delete scb;
    delete net;
    delete icy_info;

#ifdef _WII_

    unmount();
    net_deinit();

    switch(exit_mode)
    {
        case 1:
            SYS_ResetSystem(SYS_POWEROFF,0,0); // could add poweroff etc..
        break;
        case 2:
            SYS_ResetSystem(SYS_RETURNTOMENU,0,0); // could add poweroff etc..
        break;
    }


#endif
    return 0;
}
