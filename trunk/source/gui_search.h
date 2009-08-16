#ifndef GUI_SEARCH_H_INCLUDED
#define GUI_SEARCH_H_INCLUDED

/*
 Search for genre screen

*/
#include "gui.h"
#include "gui_keyboard.h"
#include "gui_button.h"
class gui_search {
    public:

    gui*            parent;
    gui_keyboard*   keyboard;
    gui_button*     add_genre;

    gui_search(gui* _ui) : parent(_ui), keyboard(0), add_genre(0)
    {
        keyboard = new gui_keyboard(_ui->fnts,_ui->guibuffer,50,50,&gui_search_ok,&gui_search_cancel,vars.search_var("$LANG_SEARCH"),vars.search_var("$LANG_CLOSE"));

        // button to add typed text to genre list
        add_genre = new gui_button(_ui->guibuffer,_ui->fnts,300,350,0,0,false);
        add_genre->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        add_genre->set_text(vars.search_var("$LANG_ADDGENRE"));
        add_genre->pad_y = 5;
        add_genre->text_color = 0x000044;
        add_genre->text_color_over = 0xff0000;
        add_genre->font_sz = FS_SYSTEM;
        add_genre->center_text = true;
        add_genre->bind_screen = S_ALL;
    };

    ~gui_search()
    {
        delete add_genre;
        delete keyboard;
    };

    static void gui_search_ok(char* buf)
    {
        search_genre(buf);
    };

    static void gui_search_cancel()
    {
        g_screen_status = S_BROWSER;
    };

    void handle_events()
    {
        keyboard->handle_events();
        bloopj(MAX_Z_ORDERS)
        {

            if(add_genre->hit_test(event.motion.x,event.motion.y,j)==B_CLICK)
                parent->gl.add_genre(keyboard->dest_buffer);
        }

    };

    void draw()
    {
        boxRGBA(parent->guibuffer,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,230,230,230,255); // bg
        rectangleRGBA(parent->guibuffer,20,20,SCREEN_WIDTH-20,SCREEN_HEIGHT-20,100,100,230,255); // outline

        parent->fnts->change_color(100,100,100);
        parent->fnts->text(parent->guibuffer,vars.search_var("$LANG_SEARCH_TITLE"),40,40,0);

        keyboard->draw();
        add_genre->draw();

    };
};


#endif // GUI_SEARCH_H_INCLUDED
