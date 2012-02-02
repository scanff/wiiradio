#include "globals.h"
#include "functions.h"
#include "gui.h"
#include "gui/gui_object.h"
#include "gui/gui_button.h"
#include "gui/gui_slider.h"
#include "localfiles.h"
#include "browser.h"
#include "favorites.h"
#include "audio/sound_sdl.h"
#include "script.h"

//int total_sm_fun = 20;

// -- lut functions
// Available functions
_smart_functions on_click_fun[] =
{
   {"$FN_NEXT",click_next},
   {"$FN_BACK",click_back},
   {"$FN_SHOWPLAYLISTS",click_playlists},
   {"$FN_SHOWGENRES",click_genres},
   {"$FN_PLAYINGAREA",click_playing},
   {"$FN_SHOWOPTIONS",click_logo},
   {"$FN_ADD",click_add},
   {"$FN_STOP",click_stop},
   {"$FN_EXIT",click_exit},
   {"$FN_SHOWBROWSER",click_browser},
   {"$FN_SHOWVISUALS",click_showvisuals},
   {"$FN_SHOWSEARCH",click_search},
   {"$FN_NEXTSKIN",click_nextskin},
   {"$FN_SHOWLOCALBROWSER",click_localbrowser},
   {"$FN_DIRUP",click_dirup},
   {"$FN_PLAY_PAUSE",click_playpause},
   {"$FN_NEXT_TRACK",click_nexttrack},
   {"$FN_PRIOR_TRACK",click_priortrack},
   {"$FN_REPLAY",click_replay},
   {"$FN_LISTITEM_LCLICK",click_item},
   {"$FN_DELETEITEM",click_delete_item},
   {"$FN_CANCEL_BUFFERING",click_cancel_buffering},
   {"$FN_CANCEL_CONNECTING",click_cancel_connect},
   {"$FN_CLOSE_SCREEN",click_lastscreen},
   {"$FN_SHOW_INFO",click_showinfo},
   {"$FN_SHOW",click_show}, // Show a screen by given arg
   {"$FN_SEEK",click_seek},
   {"$FN_OPTIONS",click_logo},
   {"$FN_CLOSE_WND",click_close_wnd},
   {"$FN_SET_FLOC",click_set_local_loc},
   {"$FN_SHOW_WND",click_show_wnd},
   {"$FN_DIRECT_CONNECT",click_connect},
   {"$FN_SEARCH_ON_GENRE",click_searchongenre},
   {"$FN_SHOWCONNECT",click_showconnect},
   {"$FN_ADDGENRE",click_add_genre},


   {0,0}
};


/* ----------------------
   Button Click Functions
   ---------------------- */

void click_add_genre(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    SDL_mutexP(script_mutex);
    const char* genre_new = theapp->GetScript()->GetVariable("KB_OUTPUT");
    SDL_mutexV(script_mutex);

    theapp->GetGenreList()->add_genre(genre_new);

    // Reload
    theapp->GetGenreList()->reload_file();
}

void click_searchongenre(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    SDL_mutexP(script_mutex);
    const char* typed = theapp->GetScript()->GetVariable("KB_OUTPUT");
    SDL_mutexV(script_mutex);

    theapp->SetScreenStatus(S_MAIN);
    theapp->search_function(typed,SEARCH_GENRE);


}

void click_connect(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    SDL_mutexP(script_mutex);
    const char* typed = theapp->GetScript()->GetVariable("KB_OUTPUT");
    SDL_mutexV(script_mutex);

    theapp->SetScreenStatus(S_MAIN);
    theapp->connect_direct(typed);

}


void click_show_wnd(void* arg)
{

    const gui_object* obj = (gui_object*)arg;

    const int w = atoi(obj->fun_arg);

    obj->theapp->SetWNDStatus(w);

}

void click_set_local_loc(void* arg)
{

    const gui_object* obj = (gui_object*)arg;

    obj->theapp->GetFileDir()->reset_location(obj->fun_arg);
    obj->theapp->SetListView(6);

}
void click_close_wnd(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetWNDStatus(0);

}
void click_lastscreen(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;
    theapp->SetScreenStatus(S_MAIN);
}
void click_show(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);

    gui_object* obj2 = g->FindObjByName_Obj(obj->fun_arg);
    if (!obj2) return;

    for(u32 i = 0;i<g->gui_ents.size();i++)
    {
        if (g->gui_ents[i]->obj_type == GUI_SCREEN) g->gui_ents[i]->visible = false;
    }

    obj2->visible = true;

}

void click_showinfo(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetScreenStatus(S_STREAM_INFO);
}

void click_cancel_connect(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    SDL_mutexP(connect_mutex);
    SDL_mutexP(script_mutex);
    theapp->SetSystemStatus(STOPPED);
    SDL_mutexV(script_mutex);
    playback_type = AS_NULL;
    SDL_mutexV(connect_mutex);
}

void click_cancel_buffering(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    SDL_mutexP(connect_mutex);
    SDL_mutexP(script_mutex);
    theapp->SetSystemStatus(STOPPED);
    SDL_mutexV(script_mutex);
    playback_type = AS_NULL;
    SDL_mutexV(connect_mutex);
}

void click_next(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);
    app_wiiradio* theapp = obj->theapp;

  //  char* gc;

    switch(theapp->GetListView())
    {
        case S_BROWSER:
        {
            theapp->GetBrowser()->MoveNext();
        }break;

        case S_GENRES:
        {
            theapp->GetGenreList()->MoveNext();
        }break;

        case S_PLAYLISTS:
        {
            theapp->GetFavorites()->MoveNext();
        }break;

        case S_LOCALFILES:
        {
            const int fp = g->theapp->GetFileDir()->GetPosition();

            if (fp + max_listings  >= g->theapp->GetFileDir()->total_num_files)
                return;

            g->theapp->GetFileDir()->SetPosition(fp+max_listings);
            //g->theapp->localfs->current_position += max_listings;
        }
        break;

        default:
        break;
    }

}

void click_back(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    switch(theapp->GetListView())
    {
    case S_BROWSER:
    {
        theapp->GetBrowser()->MoveBack();
    }break;

    case S_PLAYLISTS:
    {
        theapp->GetFavorites()->MoveBack();
    }break;

     case S_GENRES:
     {
         theapp->GetGenreList()->MoveBack();
     }break;

     case S_LOCALFILES:
     {
         const int fp = theapp->GetFileDir()->GetPosition();
         theapp->GetFileDir()->SetPosition(fp - max_listings);
     }
     break;


     default:
     break;


    }
}

void click_add(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);
    app_wiiradio* theapp = obj->theapp;

    //theapp->SetSystemStatus(STOPPED);

    if(theapp->GetSystemStatus()==PLAYING)
        g->theapp->request_save_fav(); // save the pls
}

void click_stop(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);

    SDL_mutexP(connect_mutex);
    g->theapp->GetAudio()->sound_stop();
    playback_type = AS_NULL;
    SDL_mutexP(script_mutex);
    g->theapp->SetSystemStatus(STOPPED);
    SDL_mutexV(script_mutex);
    SDL_mutexV(connect_mutex);

}

void click_seek(void* arg)
{
    gui_slider* obj = (gui_slider*)arg;
    if (!obj)
        return;

    obj->theapp->seek_playback(obj->get_pos());
}

void click_exit(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

#ifndef _WII_
    g_running = false;
    return;
#endif
    theapp->SetScreenStatus(S_QUIT);

}

void click_browser(void* arg)
{

    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetListView(S_BROWSER);
}


void click_showconnect(void* arg)
{

    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetScreenStatus(S_SCREEN_CONNECT);
    theapp->SetWNDStatus(0);

}

void click_search(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetScreenStatus(S_SEARCHGENRE);
    theapp->SetWNDStatus(0);
}

void click_nextskin(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);

    g->theapp->next_skin();
}

void click_localbrowser(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);
    app_wiiradio* theapp = obj->theapp;

    g->theapp->GetFileDir()->directory_list_start(0,0,false);//directory_list(0,0); // Run search
    theapp->SetListView(S_LOCALFILES);
}

void click_dirup(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = (app_wiiradio*)(obj->theapp);

    if(theapp->GetListView() != 6) return;

    theapp->GetFileDir()->dirup();
}

void click_playpause(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    gui* g = (gui*)(obj->theapp->ui);
    app_wiiradio* theapp = obj->theapp;


    if (playback_type != AS_LOCAL)
        return; // N/A - Stream

    u32 status = theapp->GetSystemStatus();

    if (status == PLAYING)
    {
        SDL_mutexP(script_mutex);
        theapp->SetSystemStatus(PAUSE);
        SDL_mutexV(script_mutex);
        g->theapp->pause_playback(true);
    }
    else if (status == PAUSE)
    {
        SDL_mutexP(script_mutex);
        theapp->SetSystemStatus(PLAYING);
        SDL_mutexV(script_mutex);
        g->theapp->pause_playback(false);
    }
}


void click_showvisuals(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = (app_wiiradio*)(obj->theapp);

    if(!theapp->GetSettings()->GetOptionInt("visualmode"))
        theapp->switch_sdl_video_mode(VIDM_GX);
    else
        theapp->switch_sdl_video_mode(VIDM_SDL);

    theapp->SetScreenStatus(S_VISUALS);
}

void click_logo(void* arg)
{

    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

#ifdef LOG_ENABLED
    theapp->SetScreenStatus(S_LOG);
#else
    theapp->SetScreenStatus(S_OPTIONS);
#endif
}

void click_playing(void* arg)
{
//    const gui_object* obj = (gui_object*)arg;
//    gui* g = (gui*)(obj->theapp->ui);

    //g->buttons[BTN_PLAYING]->scroll_reset();
}

void click_genres(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetListView(S_GENRES);
}

void click_playlists(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    theapp->SetListView(S_PLAYLISTS);

}

void click_item(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    if(!obj) return;

    app_wiiradio* theapp = obj->theapp;
    const int lv = theapp->GetListView();

    switch(lv)
    {
        case S_BROWSER:
            theapp->connect_to_stream(obj->object_id,I_STATION);
        break;

        case S_LOCALFILES:
        {
            localfiles* lf = theapp->GetFileDir();

            const u32 connectto = obj->object_id + lf->GetPosition();
            const u32 lsize =  lf->list.size();

            if (connectto >= lsize || connectto < 0)
                return;

            if (lf->list[connectto]->isfolder)
            {
                lf->directory_list_start(lf->list[connectto]->path,0,false);
            }
            else
            {
                theapp->connect_to_stream(connectto,I_LOCAL);
            }
        }
        break;

        case S_PLAYLISTS:
        {

            const u32 connectto = obj->object_id;
            const u32 lsize = theapp->GetFavorites()->GetSize();

            if (connectto >= lsize || connectto < 0)
                return;

            GetApp()->connect_to_stream(connectto,I_PLAYLIST);
        }
        break;

        case S_GENRES:
        {

            const char* gc = theapp->GetGenreList()->get_genre(obj->object_id);
            if (!gc) return;

            theapp->search_function(gc,SEARCH_GENRE); // do the search .. switch to browser
        }
        break;

    }//switch

}


void click_delete_item(void* arg)
{

    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* theapp = obj->theapp;

    int i = obj->object_id;
    int delete_item = -1;

    switch(theapp->GetListView())
    {
        case S_PLAYLISTS:
        {
            delete_item = i + theapp->GetFavorites()->GetPosition();
            if (delete_item >= theapp->GetFavorites()->GetSize())
                return;

            theapp->delete_playlist(delete_item);
        }
        break;

    }

}

void click_nexttrack(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* a = (app_wiiradio*)(obj->theapp);


    if ((playback_type != AS_LOCAL) || (a->GetSystemStatus() != PLAYING) )
        return; // N/A - Stream

    a->playing_item++;
    if (a->playing_item >= 0 && a->playing_item < a->GetFileDir()->current_playing_dir_count)
        a->connect_to_stream(a->playing_item,I_LOCAL);
}

void click_priortrack(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* a = (app_wiiradio*)(obj->theapp);

    if ((playback_type != AS_LOCAL) || (a->GetSystemStatus() != PLAYING) )
        return; // N/A - Stream

    a->playing_item--;
    if (a->playing_item >= 0 && a->playing_item < a->GetFileDir()->current_playing_dir_count)
        a->connect_to_stream(a->playing_item,I_LOCAL);
}

void click_replay(void* arg)
{
    const gui_object* obj = (gui_object*)arg;
    app_wiiradio* a = (app_wiiradio*)(obj->theapp);

    if ((playback_type != AS_LOCAL) || (a->GetSystemStatus() != PLAYING) )
        return; // N/A - Stream

    a->replay_item = !a->replay_item;
}


// -----
