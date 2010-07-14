#ifndef GUI_SEARCH_H_INCLUDED
#define GUI_SEARCH_H_INCLUDED

/*
 Search for genre screen

*/
#include "gui.h"
#include "gui_keyboard.h"
#include "gui_button.h"
#include "gui_group.h"

class gui_search : public gui_dlg
{
    public:

    gui*            parent;
    gui_keyboard*   keyboard;
    gui_button*     add_genre;
    gui_group*      search_group;
    int             s_type;

    gui_search(gui* _ui) : parent(_ui), keyboard(0), add_genre(0)
    {
        keyboard = new gui_keyboard(this,_ui->fnts,_ui->guibuffer,50,70,&gui_search_ok,&gui_search_cancel,vars.search_var("$LANG_SEARCH"),vars.search_var("$LANG_CLOSE"));

        // button to add typed text to genre list
        add_genre = new gui_button(_ui->guibuffer,_ui->fnts,280,370,0,0,false);
        add_genre->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        add_genre->set_text(vars.search_var("$LANG_ADDGENRE"));
        add_genre->pad_y = 5;
        add_genre->text_color = 0x000000;
        add_genre->text_color_over = 0xff0000;
        add_genre->font_sz = FS_SYSTEM;
        add_genre->center_text = true;
        add_genre->bind_screen = S_ALL;

        search_group = new gui_group(SEARCH_MAX,140,80,41,26,120,_ui->guibuffer,_ui->fnts);
        search_group->set_on(0);

    };

    ~gui_search()
    {
        delete search_group;
        delete add_genre;
        delete keyboard;
    };

    static void gui_search_ok(void* u,char* buf)
    {
        gui_search* p = (gui_search*)u;

        search_function(buf,p->s_type);

    };

    static void gui_search_cancel()
    {
        SetLastScreenStatus();
    };

    int handle_events(SDL_Event* events)
    {
        keyboard->handle_events(events);
        bloopj(MAX_Z_ORDERS)
        {
            if (SEARCH_GENRE == s_type)
            {
                if(add_genre->hit_test(events,j)==B_CLICK)
                    parent->gl.add_genre(keyboard->dest_buffer);
            }

            s_type = search_group->hit_test(events,j);
        }



        return 0;
    };

    void draw()
    {
        boxRGBA(parent->guibuffer,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,230,230,230,255); // bg
        rectangleRGBA(parent->guibuffer,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,100,100,230,255); // outline

        parent->fnts->change_color(100,100,100);
        parent->fnts->set_size(FS_SMALL);
        //parent->fnts->text(parent->guibuffer,vars.search_var("$LANG_SEARCH_TITLE"),40,40,0);
        parent->fnts->text(parent->guibuffer,"Search/Connect",40,40,0);


        keyboard->draw();
        if (SEARCH_GENRE == s_type) add_genre->draw();
        search_group->draw();

        // text for option group
        parent->fnts->set_size(FS_SMALL);
        parent->fnts->change_color(100,100,100);
        parent->fnts->text(parent->guibuffer,"Genre: ",75,80,0,0);
        parent->fnts->text(parent->guibuffer,"Station: ",230,80,0,0);
        parent->fnts->text(parent->guibuffer,"Address: ",380,80,0,0);

    };
};


#endif // GUI_SEARCH_H_INCLUDED
