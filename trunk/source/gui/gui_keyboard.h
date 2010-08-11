#ifndef GUI_KEYBOARD_H_INCLUDED
#define GUI_KEYBOARD_H_INCLUDED



#include "gui_textbox.h"

class gui_keyboard
{
    typedef struct _keytype {
        char ch, chShift;
    } Key;

    public:

    void*           userdata;

    Key             keys[4][11];
    gui_button*     key_buttons[4][11];
    gui_button*     key_space;
    gui_button*     key_caps;
    gui_button*     key_shift;
    gui_button*     key_backspace;
    gui_textbox*    text_out;
    gui_button*     ok;
    gui_button*     cancel;
    SDL_Surface*    dest;

    char*           dest_buffer;

    int             pos_x;
    int             pos_y;

    int             caps;
    int             shift;

    app_wiiradio*   theapp;

    // -- callbacks
    void (*ok_click)(void*,char*);
    void (*cancel_click)();

    gui_keyboard(app_wiiradio* _theapp, void* _u, int _ix, int _iy,void (*_ok_click)(void*,char*),void (*_cancel_click)(),char* _ok_text, char* _close_text) :
        userdata(_u), pos_x(_ix), pos_y(_iy), caps(0), shift(0)
    {
        theapp = _theapp;

        ok_click = _ok_click;
        cancel_click = _cancel_click;

          Key thekeys[4][11] = {
        {
            {'1','!'},
            {'2','@'},
            {'3','#'},
            {'4','$'},
            {'5','%'},
            {'6','^'},
            {'7','&'},
            {'8','*'},
            {'9','('},
            {'0',')'},
            {'\0','\0'}
        },
        {
            {'q','Q'},
            {'w','W'},
            {'e','E'},
            {'r','R'},
            {'t','T'},
            {'y','Y'},
            {'u','U'},
            {'i','I'},
            {'o','O'},
            {'p','P'},
            {'-','_'}
        },
        {
            {'a','A'},
            {'s','S'},
            {'d','D'},
            {'f','F'},
            {'g','G'},
            {'h','H'},
            {'j','J'},
            {'k','K'},
            {'l','L'},
            {':',';'},
            {'\'','"'}
        },

        {
            {'z','Z'},
            {'x','X'},
            {'c','C'},
            {'v','V'},
            {'b','B'},
            {'n','N'},
            {'m','M'},
            {',','<'},
            {'.','>'},
            {'/','?'},
            {'\0','\0'}
        }
        };

        memcpy(keys, thekeys, sizeof(thekeys));

        // create the keys

        char txt[2] = { 0, 0 };

        for(int i=0; i<4; i++)
        {
            for(int j=0; j<11; j++)
            {
                key_buttons[i][j] = 0;

                if(keys[i][j].ch != '\0')
                {

                    txt[0] = keys[i][j].ch;
                    key_buttons[i][j] = 0;
                    key_buttons[i][j] = new gui_button(theapp,pos_x+(j*42+21*i+40), pos_y+(i*42+80),0,0,false);
                    key_buttons[i][j]->set_images((char*)"imgs/keyboard_key.png",(char*)"imgs/keyboard_key_over.png",0,0);
                    key_buttons[i][j]->set_text(txt);
                    key_buttons[i][j]->pad_y = 5;
                    key_buttons[i][j]->text_color = 0x000000;
                    key_buttons[i][j]->text_color_over = 0xff0044;
                    key_buttons[i][j]->font_sz = FS_SYSTEM;
                    key_buttons[i][j]->center_text = true;
                    key_buttons[i][j]->bind_screen = S_ALL;

                }
            }
        }

        key_space = 0;
        key_space = new gui_button(theapp,pos_x+0, pos_y+(4*42+80),0,0,false);
        key_space->set_images((char*)"imgs/space_out.png",(char*)"imgs/space_out.png",0,0);
        key_space->pad_y = 0;
        key_space->s_x = (SCREEN_WIDTH/2)-(key_space->s_w/2);
        key_space->text_color = 0x000000;
        key_space->text_color_over = 0xff0044;
        key_space->font_sz = FS_SYSTEM;
        key_space->center_text = true;
        key_space->bind_screen = S_ALL;


        key_backspace = 0;
        key_backspace = new gui_button(theapp,pos_x+(10*42+40), pos_y+(0*42+80),0,0,false);
        key_backspace->set_images((char*)"imgs/keyboard_mediumkey_over.png",(char*)"imgs/keyboard_mediumkey.png",0,0);
        key_backspace->pad_y = 5;
        key_backspace->set_text("Back");
        key_backspace->text_color = 0x000000;
        key_backspace->text_color_over = 0xff0044;
        key_backspace->font_sz = FS_SYSTEM;
        key_backspace->center_text = true;
        key_backspace->bind_screen = S_ALL;

        key_caps = 0;
        key_caps = new gui_button(theapp,pos_x+0, pos_y+(2*42+80),0,0,false);
        key_caps->set_images((char*)"imgs/keyboard_mediumkey_over.png",(char*)"imgs/keyboard_mediumkey.png",0,0);
        key_caps->pad_y = 5;
        key_caps->set_text("Caps");
        key_caps->text_color = 0x000000;
        key_caps->text_color_over = 0xff0044;
        key_caps->font_sz = FS_SYSTEM;
        key_caps->center_text = true;
        key_caps->bind_screen = S_ALL;


        key_shift = 0;
        key_shift = new gui_button(theapp,pos_x+21, pos_y+(3*42+80),0,0,false);
        key_shift->set_images((char*)"imgs/keyboard_mediumkey_over.png",(char*)"imgs/keyboard_mediumkey.png",0,0);
        key_shift->pad_y = 5;
        key_shift->set_text("Shift");
        key_shift->text_color = 0x000000;
        key_shift->text_color_over = 0xff0044;
        key_shift->font_sz = FS_SYSTEM;
        key_shift->center_text = true;
        key_shift->bind_screen = S_ALL;



        text_out = 0;
        text_out = new gui_textbox(theapp,pos_x+(0), pos_y+(35),0,0,false);
        text_out->set_images((char*)"imgs/space_out.png",(char*)"imgs/space_out.png",0,0);
        text_out->pad_y = 5;
        text_out->s_x = (SCREEN_WIDTH/2)-(key_space->s_w/2);
        text_out->text_color = 0x000000;
        text_out->text_color_over = 0xff0044;
        text_out->font_sz = FS_SYSTEM;
        //text_out->center_text = true;
        text_out->bind_screen = S_ALL;
       // text_out->limit_text = 200;
        dest_buffer = text_out->text_l1;

        ok = 0;
        ok = new gui_button(theapp,pos_x+(400), pos_y+(300),0,0,false);
        ok->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        ok->set_text((char*)_ok_text);
        ok->pad_y = 5;
        ok->text_color = 0x000000;
        ok->text_color_over = 0xff0044;
        ok->font_sz = FS_SYSTEM;
        ok->center_text = true;
        ok->bind_screen = S_ALL;


        cancel = 0;
        cancel = new gui_button(theapp,pos_x+(70),pos_y+(300),0,0,false);
        cancel->set_images((char*)"imgs/button_out.png",(char*)"imgs/button_out.png",0,0);
        cancel->set_text((char*)_close_text);
        cancel->pad_y = 5;
        cancel->text_color = 0x000000;
        cancel->text_color_over = 0xff0044;
        cancel->font_sz = FS_SYSTEM;
        cancel->center_text = true;
        cancel->bind_screen = S_ALL;

    };
    ~gui_keyboard()
    {
        delete text_out;
        delete key_space;
        delete key_backspace;
        delete key_shift;
        delete key_caps;
        delete ok;
        delete cancel;

        for(int i=0; i<4; i++)
        {
            for(int j=0; j<11; j++)
            {
                if (key_buttons[i][j])
                {
                    delete key_buttons[i][j];
                    key_buttons[i][j] = 0;
                }
            }
        }
    };

    void update_keys()
    {
        char txt[2] = {0};
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<11; j++)
            {
                if (key_buttons[i][j])
                {
                    if (shift || caps) txt[0] = keys[i][j].chShift;
                    else txt[0] = keys[i][j].ch;

                    key_buttons[i][j]->set_text(txt);
                }
            }
        }
    };
    int handle_events(SDL_Event* event)
    {

        for(int z = MAX_Z_ORDERS; z >= 0; z--)
        {
            for(int i=0; i<4; i++)
            {
                for(int j=0; j<11; j++)
                {
                    if (key_buttons[i][j])
                    {
                        if(key_buttons[i][j]->hit_test(event,z)==B_CLICK)
                        {
                            char tb[2] = {0};

                            if(shift || caps)  tb[0] = keys[i][j].chShift;
                            else tb[0] = keys[i][j].ch;

                            strcat(dest_buffer,tb);
                        }
                    }
                }
            }
            if (key_space->hit_test(event,z)==B_CLICK)
            {
                strcat(dest_buffer," ");
            }

            if (key_backspace->hit_test(event,z)==B_CLICK)
            {
                char* ptr = dest_buffer;

                while(*++ptr != 0);

                if (dest_buffer == ptr) *dest_buffer = 0;
                else *(ptr-1) = 0;
            }

            if(key_shift->hit_test(event,z)==B_CLICK)
            {
                shift ^= 1;
                update_keys();
            }

            if(key_caps->hit_test(event,z)==B_CLICK)
            {
                caps ^= 1;
                update_keys();
            }

            if(ok->hit_test(event,z)==B_CLICK) ok_click(userdata,dest_buffer);//( //
            if(cancel->hit_test(event,z)==B_CLICK) cancel_click();
        }


        text_out->set_text(dest_buffer);

        return 0;
    };

    void draw()
    {

        text_out->draw();
        key_space->draw();
        key_backspace->draw();
        key_caps->draw();
        key_shift->draw();
        ok->draw();
        cancel->draw();
        for(int i=0; i<4; i++)
            for(int j=0; j<11; j++)
                if (key_buttons[i][j])
                    key_buttons[i][j]->draw();


    };



};

#endif // GUI_KEYBOARD_H_INCLUDED
