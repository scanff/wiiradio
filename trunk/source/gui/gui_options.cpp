#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "../gui.h"
#include "gui_button.h"
#include "gui_group.h"
#include "gui_options.h"

static void sleep_timer_callback(app_wiiradio* theapp)
{
    theapp->sleep_time_start = get_tick_count();
}


gui_options::gui_options(app_wiiradio* _theapp) :
    logo(0),
    media_free_space_desc("unknown"),
    last_time_ds(0)
{
    theapp = _theapp;
    fnts = theapp->GetFonts();
    guibuffer =  theapp->ui->guibuffer;

    logo = IMG_Load(make_path("data/imgs/def_logo.png"));

    loopi(O_MAX) b_option_item[i] = 0;
    loopi(OB_MAX) option_buts[i] = 0;

    //quit

    option_buts[OB_QUIT] = new gui_button(theapp, 20,425,0,0,false);
    option_buts[OB_QUIT]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_over.png",0,0);
    option_buts[OB_QUIT]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_EXIT"));
    option_buts[OB_QUIT]->pad_y = 5;
    option_buts[OB_QUIT]->text_color = 0x000000;
    option_buts[OB_QUIT]->text_color_over = 0xff0044;
    option_buts[OB_QUIT]->font_sz = FS_SYSTEM;
    option_buts[OB_QUIT]->center_text = true;
    option_buts[OB_QUIT]->bind_screen = S_OPTIONS;
    option_buts[OB_QUIT]->parent = (gui_object*)this;

    //return
    option_buts[OB_RETURN] = new gui_button(theapp,150,425,0,0,false);
    option_buts[OB_RETURN]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_over.png",0,0);
    option_buts[OB_RETURN]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_OK"));
    option_buts[OB_RETURN]->pad_y = 5;
    option_buts[OB_RETURN]->text_color = 0x000000;
    option_buts[OB_RETURN]->text_color_over = 0xff0044;
    option_buts[OB_RETURN]->font_sz = FS_SYSTEM;
    option_buts[OB_RETURN]->center_text = true;
    option_buts[OB_RETURN]->bind_screen = S_OPTIONS;
    option_buts[OB_RETURN]->parent = (gui_object*)this;

    //more
/*
    option_buts[OB_MORE] = new gui_button(theapp,500,425,0,0,false);
    option_buts[OB_MORE]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_over.png",0,0);
    option_buts[OB_MORE]->set_text("More >>>");//theapp->vars->search_var("$LANG_TXT_OK"));
    option_buts[OB_MORE]->pad_y = 5;
    option_buts[OB_MORE]->text_color = 0x000000;
    option_buts[OB_MORE]->text_color_over = 0xff0044;
    option_buts[OB_MORE]->font_sz = FS_SYSTEM;
    option_buts[OB_MORE]->center_text = true;
    option_buts[OB_MORE]->bind_screen = S_OPTIONS;
    option_buts[OB_MORE]->parent = (gui_object*)this;
*/
    int y = 90;

    service_group = new gui_group(theapp,2,220,y,41,26,143,NULL);
    service_group->set_on(theapp->GetSettings()->oservicetype);


    y += 35;
/*  Removed for now this is kinda annoying to most
    b_option_item[O_SCROLL_TEXT] = new gui_button(theapp,220,y,0,0,0);
    b_option_item[O_SCROLL_TEXT]->set_images((char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_on.png");
    b_option_item[O_SCROLL_TEXT]->bind_screen = S_OPTIONS;
    b_option_item[O_SCROLL_TEXT]->set_image_onout((char*)"data/imgs/toggle_on.png");
    b_option_item[O_SCROLL_TEXT]->parent = (gui_object*)this;
*/
    b_option_item[O_WIDESCREEN] = new gui_button(theapp,403,y,0,0,0);
    b_option_item[O_WIDESCREEN]->set_images((char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_on.png");
    b_option_item[O_WIDESCREEN]->bind_screen = S_OPTIONS;
    b_option_item[O_WIDESCREEN]->parent = (gui_object*)this;
    b_option_item[O_WIDESCREEN]->set_image_onout((char*)"data/imgs/toggle_on.png");
    y += 35;

    b_option_item[O_STARTFROMLAST] = new gui_button(theapp,220,y,0,0,0);
    b_option_item[O_STARTFROMLAST]->set_images((char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_on.png");
    b_option_item[O_STARTFROMLAST]->bind_screen = S_OPTIONS;
    b_option_item[O_STARTFROMLAST]->parent = (gui_object*)this;
    b_option_item[O_STARTFROMLAST]->set_image_onout((char*)"data/imgs/toggle_on.png");

    b_option_item[O_RIPMUSIC] = new gui_button(theapp,403,y,0,0,0);
    b_option_item[O_RIPMUSIC]->set_images((char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_out.png",(char*)"data/imgs/toggle_on.png");
    b_option_item[O_RIPMUSIC]->bind_screen = S_OPTIONS;
    b_option_item[O_RIPMUSIC]->parent = (gui_object*)this;
    b_option_item[O_RIPMUSIC]->set_image_onout((char*)"data/imgs/toggle_on.png");

    y += 50;

    saver_group = new gui_group(theapp,4,220,y,41,26,20,NULL);
    saver_group->set_on(theapp->GetSettings()->oscreensavetime);
    saver_group->bind_screen = S_OPTIONS;

    y += 45;

    sleep_timer_group = new gui_group(theapp,6,220,y,41,26,20,&sleep_timer_callback);
    sleep_timer_group->set_on(theapp->GetSettings()->osleep_timer_time);
    sleep_timer_group->bind_screen = S_OPTIONS;
    sleep_timer_group->parent = (gui_object*)this;

    y += 45;

    // visual type
    visuals_group = new gui_group(theapp,2,220,y,41,26,40,NULL);
    visuals_group->set_on(theapp->GetSettings()->ovisual_mode);
    visuals_group->bind_screen = S_OPTIONS;
    visuals_group->parent = (gui_object*)this;

    y += 35;

    // next skin
    option_buts[OB_NEXT_SKIN] = new gui_button(theapp, 420,y,0,0,false);
    option_buts[OB_NEXT_SKIN]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_out.png",0,0);
    option_buts[OB_NEXT_SKIN]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_NEXT_SKIN"));
    option_buts[OB_NEXT_SKIN]->pad_y = 5;
    option_buts[OB_NEXT_SKIN]->text_color = 0x000000;
    option_buts[OB_NEXT_SKIN]->text_color_over = 0xff0044;
    option_buts[OB_NEXT_SKIN]->font_sz = FS_SYSTEM;
    option_buts[OB_NEXT_SKIN]->center_text = true;
    option_buts[OB_NEXT_SKIN]->bind_screen = S_OPTIONS;
    option_buts[OB_NEXT_SKIN]->parent = (gui_object*)this;

    y += 50;

    // next lang
    option_buts[OB_NEXT_LANG] = new gui_button(theapp,420,y,0,0,false);
    option_buts[OB_NEXT_LANG]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_out.png",0,0);
    option_buts[OB_NEXT_LANG]->set_text(theapp->GetVariables()->search_var("$LANG_TXT_NEXT_LANG"));
    option_buts[OB_NEXT_LANG]->pad_y = 5;
    option_buts[OB_NEXT_LANG]->text_color = 0x000000;
    option_buts[OB_NEXT_LANG]->text_color_over = 0xff0044;
    option_buts[OB_NEXT_LANG]->font_sz = FS_SYSTEM;
    option_buts[OB_NEXT_LANG]->center_text = true;
    option_buts[OB_NEXT_LANG]->bind_screen = S_OPTIONS;
    option_buts[OB_NEXT_LANG]->parent = (gui_object*)this;

//#ifdef _WII_
    // reinir network
    option_buts[OB_NET_REINIT] = new gui_button(theapp,280,425,0,0,false);
    option_buts[OB_NET_REINIT]->set_images((char*)"data/imgs/button_out.png",(char*)"data/imgs/button_out.png",0,0);
    option_buts[OB_NET_REINIT]->set_text(theapp->GetVariables()->search_var("$LANG_REINTNET"));
    option_buts[OB_NET_REINIT]->pad_y = 5;
    option_buts[OB_NET_REINIT]->text_color = 0x000000;
    option_buts[OB_NET_REINIT]->text_color_over = 0xff0044;
    option_buts[OB_NET_REINIT]->font_sz = FS_SYSTEM;
    option_buts[OB_NET_REINIT]->center_text = true;
    option_buts[OB_NET_REINIT]->bind_screen = S_OPTIONS;
    option_buts[OB_NET_REINIT]->parent = (gui_object*)this;
//#endif
    // set options
//    theapp->GetSettings()->oscrolltext ? b_option_item[O_SCROLL_TEXT]->obj_sub_state = B_ON : b_option_item[O_SCROLL_TEXT]->obj_sub_state = B_OFF;
    theapp->GetSettings()->owidescreen ? b_option_item[O_WIDESCREEN]->obj_sub_state = B_ON : b_option_item[O_WIDESCREEN]->obj_sub_state = B_OFF;
    theapp->GetSettings()->oripmusic ? b_option_item[O_RIPMUSIC]->obj_sub_state = B_ON : b_option_item[O_RIPMUSIC]->obj_sub_state = B_OFF;
//    GetSYSVariable(SYS_RIPPING) ? b_option_item[O_RIPMUSIC]->obj_sub_state = B_ON : b_option_item[O_RIPMUSIC]->obj_sub_state = B_OFF;
    theapp->GetSettings()->ostartfromlast ? b_option_item[O_STARTFROMLAST]->obj_sub_state = B_ON : b_option_item[O_STARTFROMLAST]->obj_sub_state = B_OFF;

    // Try calc. ds on construction
    media_free_space_desc = get_media_free_space_desc();
};

gui_options::~gui_options()
{
    delete service_group;
    service_group = 0;

    delete saver_group;
    saver_group = 0;

    delete visuals_group;
    visuals_group = 0;

    delete sleep_timer_group;
    sleep_timer_group = 0;

    loopi(O_MAX)
    {
        delete b_option_item[i];
        b_option_item[i] = 0;
    }

    loopi(OB_MAX)
    {
        delete option_buts[i];
        option_buts[i] = 0;
    }

    SDL_FreeSurface(logo);
}

int gui_options::Events(const SDL_Event* events)
{
    int val;
    wr_options* sc = theapp->GetSettings();

    if(option_buts[OB_QUIT]->hit_test(events)==B_CLICK)
    {
#ifndef _WII_
        g_running = false;
        return 1;
#endif
        theapp->SetScreenStatus(S_QUIT);
        return 1;
    }

    if(option_buts[OB_RETURN]->hit_test(events)==B_CLICK)
    {
        theapp->SetLastScreenStatus();
        return 1;
    }

    if(option_buts[OB_NEXT_SKIN]->hit_test(events)==B_CLICK)
    {
        theapp->next_skin();
        return 1;
    }

    if(option_buts[OB_NEXT_LANG]->hit_test(events)==B_CLICK)
    {
        theapp->next_lang();
        return 1;
    }

    if(option_buts[OB_NET_REINIT]->hit_test(events)==B_CLICK)
    {
        theapp->net_setup();

        return 1;
    }

    //if(option_buts[OB_MORE]->hit_test(events)==B_CLICK) {}

    loopi(O_MAX)
    {
        if(b_option_item[i]->hit_test(events))
        {
            return 1;
        }
    }

    theapp->GetSettings()->oscreensavetime = saver_group->hit_test(events);
    theapp->GetSettings()->osleep_timer_time = sleep_timer_group->hit_test(events);
    theapp->GetSettings()->oservicetype = service_group->hit_test(events);
    theapp->GetSettings()->ovisual_mode = visuals_group->hit_test(events);
    //int x = visuals_group->hit_test(events);
//    b_option_item[O_SCROLL_TEXT]->obj_sub_state == B_OFF ? theapp->GetSettings()->oscrolltext = 0 : theapp->GetSettings()->oscrolltext = 1;


    if (b_option_item[O_WIDESCREEN]->obj_sub_state == B_OFF && theapp->GetSettings()->owidescreen)
    {
        theapp->set_widescreen(0);
        return 1;
    }
    else if (b_option_item[O_WIDESCREEN]->obj_sub_state == B_ON && !theapp->GetSettings()->owidescreen)
    {
        theapp->set_widescreen(1);
        return 1;
    }


    // rip music
    b_option_item[O_RIPMUSIC]->obj_sub_state == B_OFF ?  val = 0: val = 1;
    sc->SetOptionInt("ripmusic", val);
    theapp->GetSettings()->oripmusic = val;

    // start from last
    b_option_item[O_STARTFROMLAST]->obj_sub_state == B_OFF ? val = 0 : val = 1;
    sc->SetOptionInt("startfromlast", val);
    theapp->GetSettings()->ostartfromlast = val;

    return 0;
}

void gui_options::output_options()
{
    this->visible = true;

    int x = 0;
    ostringstream str;

    //logo
    SDL_Rect rc = {50,30,logo->w,logo->h};
    SDL_BlitSurface(logo,0,guibuffer,&rc);

    // Font setup
    fnts->change_color(100,100,100);
    fnts->set_size(FS_SYSTEM);

    // -- about
    str << theapp->GetVariables()->search_var("$LANG_ABOUT")
        << " v" << VERSION_NUMBER << " - " << theapp->GetVariables()->search_var("$LANG_AUTHOR")
        << " Scanff, TiMeBoMb " << theapp->GetVariables()->search_var("$LANG_AND") << " Knarrff";
    fnts->text(guibuffer, str.str().c_str(), 200, 50, 0, 0);

    int y = 90;

    // for service type
    fnts->text(guibuffer,"SHOUTcast :",200,y,0,1);
    fnts->text(guibuffer,"Icecast :",393,y,0,1);

    y += 35;
    // Scrolltext, Widescreen, Rip Music
    //fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_SCROLL_STATIONTEXT"),200,y,0,1);
    fnts->text(guibuffer,"Widescreen :",393,y,0,1); // -- TO DO Variable this

    y += 35;
    fnts->text(guibuffer,"Play last at start :", 200,y,0,1); // -- TO DO Variable this
    fnts->text(guibuffer,"Rip Music :",393,y,0,1); // -- TO DO Variable this

    /*
        Show the user how much free space they have left.
        Only run this every few seconds as it my lag
    */
/*    if((SDL_GetTicks() - last_time_ds > 1000*10) && GetSYSVariable(SYS_RIPPING))
    {
        last_time_ds = SDL_GetTicks();
        media_free_space_desc = get_media_free_space_desc();
    }
*/
    fnts->text(guibuffer,("(Free space: "+media_free_space_desc+")").c_str(),450,y,0,0);

    y += 50;
    // screen save
    fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_SCREEN_SAVE"),200,y,0,1);
    fnts->text(guibuffer,"  Off         1min       5min      10min",220,y-20,0,0);

    y += 45;
    // sleep timer
    fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_SLEEP_TIMER"),200,y,0,1);
    fnts->text(guibuffer,"  Off         5min      15min     30min        1h           3h",220,y-20,0,0);

    y += 45;
    // visual type/mode
    fnts->text(guibuffer,"Visual Mode:",190,y,0,1);
    fnts->text(guibuffer,"ProjectM     Classic(2D)",215,y-20,0,0);

    y += 35;
    // -- skin changer
    fnts->change_color(100,100,100);
    fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_CHANGE_SKIN"),200,y,0,1);
    fnts->change_color(40,40,100);

    fnts->text(guibuffer,theapp->GetVariables()->search_var("skinname"),220,y,0);
    x = fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_AUTHOR"),220,y+20,0);
    x += fnts->text(guibuffer,": ",220+x,y+20,0);
    fnts->text(guibuffer,theapp->GetVariables()->search_var("skinauthor"),220+x,y+20,0);

    y += 50;
    // -- language selection
    fnts->change_color(100,100,100);
    fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_CHANGE_LANG"),200,y,0,1);
    fnts->change_color(40,40,100);
    fnts->text(guibuffer,theapp->GetVariables()->search_var("$LANG_NAME"),220,y,0);

    // -- draw the butons ect...
    loopi(O_MAX) b_option_item[i]->draw();
    loopi(OB_MAX) option_buts[i]->draw();

    saver_group->draw();
    sleep_timer_group->draw();
    visuals_group->draw();
    service_group->draw();
}

int gui_options::draw()
{
    boxRGBA(guibuffer,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,220,220,220,255); // bg
    output_options();

    return 0;
}
