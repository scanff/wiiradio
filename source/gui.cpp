#include "globals.h"
#include "application.h"
#include "genre_list.h"
#include "skins.h"
#include "gui/gui_button.h"
#include "gui/gui_textbox.h"
#include "gui/gui_list.h"
#include "gui/gui_options.h"
#include "gui/gui_info.h"
#include "fonts.h"
#include "textures.h"
#include "skins.h"
#include "lang.h"
#include "options.h"
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
#include "station.h"
#include "gui.h"
#include "script.h"

#include "functions.h"

#define STRING_NOTHING  ("    \0")


gui::gui(app_wiiradio* _theapp) :
    theapp(_theapp),
    visbuffer( NULL ),
    vis_on_screen(0),
    options_screen( NULL )
{

    theapp->ui = this; // As we've not constructed this yet the address is bad
    texture_cache*  tx = theapp->GetTextures();
    skins*          sk = theapp->GetSkins();

    fnts        = theapp->GetFonts();
    vis         = theapp->visuals;
    skin_path   = theapp->GetSettings()->ocurrentskin;

    theapp->GetGenreList()->load_file();
    gui_ents.clear();

#ifdef _WII_
    guibuffer = theapp->screen;//SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH, rmask, gmask, bmask,amask);

#else
    // -- tripple buf ... as HW surface need SW for blitting
    guibuffer = SDL_CreateRGBSurface(SDL_SWSURFACE,SCREEN_WIDTH,SCREEN_HEIGHT,BITDEPTH, rmask, gmask, bmask,amask);
    // SDL_SetColorKey(guibuffer,SDL_SRCCOLORKEY, SDL_MapRGBA(guibuffer->format,0,0,0,255));
#endif

    char s_value1[SMALL_MEM];
    const char* dir = skin_path.c_str();

    if (!theapp->GetSkins()->load_skin(dir)) //skin
        exit(0);

    theapp->load_splash("Skin Loaded...");

    // -- how many listing used by this skin
    max_listings = theapp->GetSkins()->get_value_int("listing_number_of");


    //fontset
    if(sk->get_value_file("fontset",s_value1,dir))
    {
        int fsizes[3];

        fsizes[2] = sk->get_value_int("fontsize_small");
        fsizes[1] = sk->get_value_int("fontsize_medium");
        fsizes[0] = sk->get_value_int("fontsize_large");

        // - deafult sizes
        if (!fsizes[0]) fsizes[0] = 14;
        if (!fsizes[1]) fsizes[1] = 22;
        if (!fsizes[2]) fsizes[2] = 26;

        fnts->reload_fonts(s_value1,fsizes);
    }

    // -- cursor
    if (!sk->get_value_file("cursor",s_value1,dir)) exit(0);
    cursor = tx->texture_lookup(s_value1);

    // -- volume overlay
    if (!sk->get_value_file("volume_bg",s_value1,dir)) exit(0);
    vol_bg = tx->texture_lookup(s_value1);
    if (!sk->get_value_file("volume_over",s_value1,dir)) exit(0);
    vol_ol = tx->texture_lookup(s_value1);

    // --- Mute image
    if (!sk->get_value_file("mute",s_value1,dir)) exit(0);
    mute_img = tx->texture_lookup(s_value1);


    //reorder on zorder
    loopi(gui_ents.size())
    {
        loopj(gui_ents.size())
        {
            if (gui_ents[i]->z_order < gui_ents[j]->z_order)
            {
                gui_object* first = gui_ents[i];
                gui_object* second = gui_ents[j];
                gui_ents[i] = second;
                gui_ents[j] = first;
            }

        }
    }

    // options .. not skinnable
    options_screen = new gui_options(theapp);

    // quit screen
    bQuit[QB_LOADER] = new gui_button(theapp,40,(SCREEN_HEIGHT/2)-20,0,0,0);
    bQuit[QB_LOADER]->set_images("data/imgs/button_out.png","data/imgs/button_over.png",0,0);
    bQuit[QB_LOADER]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_QUIT_LOADER"));
    bQuit[QB_LOADER]->pad_y = 5;
    bQuit[QB_LOADER]->text_color = 0x000000;
    bQuit[QB_LOADER]->text_color_over = 0xff0044;
    bQuit[QB_LOADER]->font_sz = FS_SYSTEM;
    bQuit[QB_LOADER]->center_text = true;
    bQuit[QB_LOADER]->bind_screen = S_QUIT;
    bQuit[QB_LOADER]->parent = 0;
    // --
    bQuit[QB_SYS] = new gui_button(theapp,190,(SCREEN_HEIGHT/2)-20,0,0,0);
    bQuit[QB_SYS]->set_images("data/imgs/button_out.png","data/imgs/button_over.png",0,0);
    bQuit[QB_SYS]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_QUIT_SYS"));
    bQuit[QB_SYS]->pad_y = 5;
    bQuit[QB_SYS]->text_color = 0x000000;
    bQuit[QB_SYS]->text_color_over = 0xff0044;
    bQuit[QB_SYS]->font_sz = FS_SYSTEM;
    bQuit[QB_SYS]->center_text = true;
    bQuit[QB_SYS]->bind_screen = S_QUIT;
    bQuit[QB_SYS]->parent = 0;
        // --
    bQuit[QB_TURNOFF] = new gui_button(theapp,340,(SCREEN_HEIGHT/2)-20,0,0,0);
    bQuit[QB_TURNOFF]->set_images("data/imgs/button_out.png","data/imgs/button_over.png",0,0);
    bQuit[QB_TURNOFF]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_QUIT_TO"));
    bQuit[QB_TURNOFF]->pad_y = 5;
    bQuit[QB_TURNOFF]->text_color = 0x000000;
    bQuit[QB_TURNOFF]->text_color_over = 0xff0044;
    bQuit[QB_TURNOFF]->font_sz = FS_SYSTEM;
    bQuit[QB_TURNOFF]->center_text = true;
    bQuit[QB_TURNOFF]->bind_screen = S_QUIT;
    bQuit[QB_TURNOFF]->parent = 0;
    // --
    bQuit[QB_BACK] = new gui_button(theapp,480,(SCREEN_HEIGHT/2)-20,0,0,0);
    bQuit[QB_BACK]->set_images("data/imgs/button_out.png","data/imgs/button_over.png",0,0);
    bQuit[QB_BACK]->set_text(theapp->GetVariables()->search_var("$LANG_CANCEL"));
    bQuit[QB_BACK]->pad_y = 5;
    bQuit[QB_BACK]->text_color = 0x000000;
    bQuit[QB_BACK]->text_color_over = 0xff0044;
    bQuit[QB_BACK]->font_sz = FS_SYSTEM;
    bQuit[QB_BACK]->center_text = true;
    bQuit[QB_BACK]->bind_screen = S_QUIT;
    bQuit[QB_BACK]->parent = 0;


}

gui::~gui()
{
    if (options_screen)
    {
        delete options_screen;
        options_screen = NULL;
    }

    loopi(gui_ents.size())
    {
        if(gui_ents[i])
        {
            delete gui_ents[i];
            gui_ents[i] = 0;
        }
    }

#ifndef _WII_
    SDL_FreeSurface(guibuffer);
#endif
    if(visbuffer) SDL_FreeSurface(visbuffer);

}

void* gui::new_object(const int type, void* o)
{
    if (type == GUI_UNKNOWN)
        return 0;

    gui_object* new_gui_obj = 0;

    new_gui_obj = (gui_object*)o;
    if (new_gui_obj) gui_ents.push_back(new_gui_obj);

    return new_gui_obj;
}


FORCEINLINE void gui::quit_events(const SDL_Event* events)
{
    if(bQuit[QB_LOADER]->hit_test(events)==B_CLICK) g_running = false;//g_running = false;
    if(bQuit[QB_SYS]->hit_test(events)==B_CLICK)
    {
        theapp->exit_mode = 2;
        g_running = false;
    }
    if (bQuit[QB_TURNOFF]->hit_test(events)==B_CLICK)
    {
        theapp->exit_mode = 1;
        g_running = false;
    }

    if(bQuit[QB_BACK]->hit_test(events)==B_CLICK) theapp->SetScreenStatus(S_MAIN);

}

FORCEINLINE void gui::draw_quit()
{
    const char* title = theapp->GetVariables()->search_var("$LANG_TXT_EXIT");
    boxRGBA(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,220,220,220,255); // bg
    boxRGBA(guibuffer,0,0,SCREEN_WIDTH,50,0,0,0,255); // bg
    boxRGBA(guibuffer,0,SCREEN_HEIGHT-50,SCREEN_WIDTH,SCREEN_HEIGHT,0,0,0,255); // bg
    fnts->change_color(51,51,119);
    fnts->set_size(FS_SYSTEM_LARGE);
    fnts->text(guibuffer,title,20,67,SCREEN_WIDTH,0);
    loopi(QB_MAX) {bQuit[i]->visible = true; bQuit[i]->draw(); }
}

void gui::handle_events(const SDL_Event* events)
{
//    const u8 etype = events->type;
    const int cur_screen = theapp->GetScreenStatus();

    if (/*(etype != SDL_MOUSEMOTION &&
        etype != SDL_MOUSEBUTTONDOWN &&
        etype != SDL_MOUSEBUTTONUP) ||*/
        (cur_screen==S_VISUALS))
        return;

    switch(cur_screen)
    {
        case S_OPTIONS:
            options_screen->Events(events);
        return;

        case S_QUIT:
            quit_events(events);
        return;

        default:
        {

            bloopj(gui_ents.size()) gui_ents[j]-> obj_state = B_OUT;

            bloopj(gui_ents.size())
            {
                gui_object* cur_ent = gui_ents[j];

                if(
                   ((cur_ent->IsVisible()) && (cur_ent->hit_test(events))) ||
                   ((cur_ent->GetType() == GUI_POPUP) && (cur_ent->IsVisible()))
                   ) // If this is a popup return, only want hit test for it's children
                    return;
            }
        }
    }
}

void gui::screen_changed(const int cur, const int last)
{
    for(u32 i = 0;i < gui_ents.size();i++)
    {
        if ((gui_ents[i]->obj_type == GUI_SCREEN) && (gui_ents[i]->bind_screen != cur) )
            gui_ents[i]->visible = false;
    }
}

void gui::draw_local_files()
{
    int i = 0;
    u32 p = 0;

    localfiles* lf = theapp->GetFileDir();

    gui_list* obj = (gui_list*)FindObjByName_Obj("local_browser");

    if (!obj)
        return;

    int max = obj->GetSize();

    for(i = 0; i < max; i++) // clear the data first
        theapp->GetVariables()->add_var_array("$LOCAL_LIST_",(char*)STRING_NOTHING,i);

    obj->visible = true;

    i = 0;

    SDL_mutexP(listviews_mutex);

    p = lf->GetPosition();

    for(i = 0; i < max;i++)
    {
        if ( p >= lf->list.size() )
        {
            obj->SetItemVisible(i,false);

        }else{
            if(p < 0) break;

            obj->SetItemVisible(i,true);

            if ((theapp->playing_path == lf->list[p]->path) && (p == theapp->playing_item))
                obj->SetHighlight(i,true);

            theapp->GetVariables()->add_var_array("$LOCAL_LIST_",(char*)lf->list[p]->name,i);
            if (lf->list[p]->isfolder)
            {
                obj->SetIconID(i,1);
            }else{
                obj->SetIconID(i,0);
            }
        }

        p++;
    }

    SDL_mutexV(listviews_mutex);

}

FORCEINLINE void gui::draw_favs()
{
    favorites* favs = theapp->GetFavorites();

    gui_object* obj = 0;
    gui_object* obj_del = 0;

    int i = 0;
    int p = 0;

    loopi(max_listings) theapp->GetVariables()->add_var_array("$FAVS_LIST_",(char*)STRING_NOTHING,i);

    obj = FindObjByName_Obj("favs_browser");
    obj_del = FindObjByName_Obj("favs_del");
    if (!obj || !obj_del) return;

    static_cast<gui_list*>(obj)->visible = true;
    static_cast<gui_list*>(obj_del)->visible = true;

    i=0;
    p = favs->GetPosition();

    for(i = 0; i < max_listings;i++)
    {
        if ( p >= favs->GetSize() )
        {
            static_cast<gui_list*>(obj)->SetItemVisible(i,false);
            static_cast<gui_list*>(obj_del)->SetItemVisible(i,false);

        }else{
            static_cast<gui_list*>(obj_del)->SetItemVisible(i,true);
            static_cast<gui_list*>(obj)->SetItemVisible(i,true);
            theapp->GetVariables()->add_var_array("$FAVS_LIST_",(char*)favs->GetItemText(i),i);
        }


        p++;
    }


}

int gui::find_object_byname(const char* name)
{
    u32 i;
    const u32 hash_in = MAKE_HASH(name);

    for( i = 0; i < gui_ents.size(); i++)
    {
        if (gui_ents[i]->name == hash_in)
        {
            return i;
        }
    }

    return 0;
}

gui_object* gui::FindObjByName_Obj(const char* name)
{
    u32 i;
    const u32 hash_in = MAKE_HASH(name);

    for( i = 0; i < gui_ents.size(); i++)
    {
        if (gui_ents[i]->name == hash_in)
        {
            return gui_ents[i];
        }
    }

    return 0;
}

FORCEINLINE void gui::draw_status()
{
    const local_player* lpb = theapp->GetLocFile();
    const icy* ic = theapp->icy_info;
    const char* nps;

    if (playback_type == AS_LOCAL) nps = lpb->display_name.c_str();
    else nps = ic->track_title;

    if (nps)
    {
        theapp->GetScript()->SetVariableString("TRACK_TITLE",nps);
    }
}

FORCEINLINE void gui::draw_browser()
{
    SDL_mutexP(listviews_mutex);

    const int size = theapp->GetBrowser()->GetSize();

    if (size > 0)
    {

        int i = 0;
        int p = 0;

         for(i = 0; i < max_listings; i++) // clear the data first
            theapp->GetVariables()->add_var_array("$BROWSER_LIST_",(char*)STRING_NOTHING,i);

        gui_object* obj = FindObjByName_Obj("browser_browser");

        if (obj)
        {
            static_cast<gui_list*>(obj)->visible = true;

            i=0;
            p = theapp->GetBrowser()->GetPosition();;

            for(i = 0; i < max_listings;i++)
            {
                if ( p >= theapp->GetBrowser()->GetSize() )
                {
                    static_cast<gui_list*>(obj)->SetItemVisible(i,false);


                }else{

                    static_cast<gui_list*>(obj)->SetItemVisible(i,true);
                    const char* g = theapp->GetBrowser()->get_station(i);
                    if (g) theapp->GetVariables()->add_var_array("$BROWSER_LIST_",g,i);

                }


                p++;
            }

        }
    }
    SDL_mutexV(listviews_mutex);
}

FORCEINLINE void gui::draw_genres()
{

    gui_object* obj = 0;

    int i = 0;
    int p = 0;

    loopi(max_listings)  theapp->GetVariables()->add_var_array("$GENRE_LIST_",(char*)STRING_NOTHING,i);

    obj = FindObjByName_Obj("genre_browser");
    if (!obj) return;

    static_cast<gui_list*>(obj)->visible = true;

    i=0;
    p = theapp->GetGenreList()->GetPosition();
    for(i = 0; i < max_listings;i++)
    {
        if ( p >= theapp->GetGenreList()->GetSize() )
        {
            static_cast<gui_list*>(obj)->SetItemVisible(i,false);


        }else{

            static_cast<gui_list*>(obj)->SetItemVisible(i,true);
            const char* g = theapp->GetGenreList()->get_genre(i);
            if (g) theapp->GetVariables()->add_var_array("$GENRE_LIST_",g,i);
        }


        p++;
    }


}

FORCEINLINE void gui::draw_visuals()
{
    const int vmode = theapp->GetSettings()->ovisual_mode;

    if (!vmode)
    {
#ifndef _WII_
        draw_rect(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0); // clear the buffer
#endif
        theapp->GetPrjM()->render();
        return;
    }

    icy* ic = theapp->icy_info;

    if (theapp->visualize_number < MAX_VISUALS)
    {
        if(playback_type == AS_LOCAL)
        {
            vis->user_data = (char*)theapp->GetLocFile()->m_id3.title.c_str();//  localpb->user_data = ic->track_title;
        }else{
            vis->user_data = ic->track_title;
        }

        vis->draw_visuals(guibuffer,theapp->visualize_number,(s16*)theapp->audio_data);


    }

}

void gui::HideScreens(const int not_sc)
{
    loopi(gui_ents.size())
    {
        if (gui_ents[i]->obj_type == GUI_SCREEN)
        {
//            const gui_object* x = gui_ents[i];
            if (gui_ents[i]->bind_screen == not_sc)
            {
                gui_ents[i]->visible = true;
            }
            else
            {
                gui_ents[i]->visible = false;
            }
        }
    }
}

void gui::HideLists(const int not_lv)
{
    loopi(gui_ents.size())
    {
        if ((gui_ents[i]->obj_type == GUI_LIST) && (static_cast<gui_list*>(gui_ents[i])->listid != not_lv))
        {
            gui_ents[i]->visible = false;
        }
    }
}

void gui::draw()
{
    const int cur_screen        = theapp->GetScreenStatus();
    const int cur_listview      = theapp->GetListView();

    switch(cur_screen)
    {
        case S_OPTIONS:
        {
            options_screen->draw();

        }return;

        case S_QUIT:
        {
            draw_quit();
        }return;

        case S_VISUALS:
        {
            draw_visuals();;
            draw_volume();
        }
        return;

        case S_STREAM_INFO:
            switch(playback_type)
            {
                case AS_LOCAL:
                    draw_mp3_details();
                break;
                case AS_NETWORK:
                    draw_stream_details();
                break;
                default:
                break;
            }

        break;
    }

    switch (cur_listview)
    {
        case S_BROWSER:
            draw_browser();
        break;

        case S_PLAYLISTS:
            draw_favs();
        break;

        case S_LOCALFILES:
            draw_local_files();
        break;

        case S_GENRES:
            draw_genres();
        break;


    }


    loopi(gui_ents.size())
        gui_ents[i]->draw();


    // -- Visuals never get here
    draw_status();

    // volume display ... like an OSD
    draw_volume();



};


FORCEINLINE void gui::draw_stream_details()
{
    icy* ic = theapp->icy_info;
/*    script* sc = theapp->GetScript();

    SDL_mutexP(script_mutex);

    SDL_mutexV(script_mutex);
  */
    theapp->GetVariables()->add_var("$ARTIST",(char*)trim_string(ic->icy_name,50));
    theapp->GetVariables()->add_var("$TITLE",(char*)trim_string(ic->icy_url,50));
    theapp->GetVariables()->add_var("$BITRATE",(char*)make_string((char*)"%dKbps",ic->icy_br));
    theapp->GetVariables()->add_var("$GENRE",(char*)trim_string(ic->icy_genre,50));

}

FORCEINLINE void gui::draw_mp3_details()
{

    local_player*           lp = theapp->GetLocFile();

    theapp->GetVariables()->add_var("$ARTIST",(char*)lp->m_id3.artist.c_str());
    theapp->GetVariables()->add_var("$TITLE",(char*)lp->m_id3.title.c_str());
    theapp->GetVariables()->add_var("$ALBUM",(char*)lp->m_id3.album.c_str());
    theapp->GetVariables()->add_var("$GENRE",(char*)lp->m_id3.genre.c_str());
    theapp->GetVariables()->add_var("$YEAR",(char*)lp->m_id3.year.c_str());

    const int br = theapp->GetAudio()->sound_get_bitrate();
    if(br > 0)
        theapp->GetVariables()->add_var("$BITRATE",(char*)make_string((char*)"%dKbps",theapp->GetAudio()->sound_get_bitrate()));

}


FORCEINLINE void gui::draw_volume()
{
    if (theapp->mute && !theapp->screen_sleeping) // always show if muted
    {
        SDL_Rect d = {20,50,mute_img->w,mute_img->h};
        SDL_BlitSurface(mute_img,0, guibuffer,&d);

        return;
    }

    u32 time = theapp->app_timer;

    if((time - theapp->vol_lasttime) < 2000) // show for 2 secs after change
    {
        SDL_Rect r = { (SCREEN_WIDTH / 2) - (vol_bg->w / 2),(SCREEN_HEIGHT / 2) - (vol_bg->h / 2),vol_bg->w,vol_bg->h };

        // bg
        SDL_BlitSurface( vol_bg,0, guibuffer,&r);

        // actual volume bar
        int pc =  (int)(((float)theapp->GetSettings()->ovolume / 255.0) * (float)vol_ol->w);

        pc > vol_ol->w ? pc = vol_ol->w : pc < 0 ? pc = 0 : 0; // clip

        // overlay
        r.x += 18;
        SDL_Rect s = { 0,0, pc,vol_ol->h };
        SDL_BlitSurface( vol_ol,&s, guibuffer,&r);
    }
}

void gui::draw_cursor(int x,int y, float angle)
{
    /* Cursor rotation */
    SDL_Surface *rot = rotozoomSurface (cursor, angle, 1, 0);
    if(!rot) return; // don't count on rotozoomSurface to always allocate surface
    SDL_Rect r = { x-rot->w/2, y-rot->h/2, rot->w, rot->h };
    SDL_BlitSurface(rot, 0, guibuffer, &r);
    SDL_FreeSurface(rot); // free
}


