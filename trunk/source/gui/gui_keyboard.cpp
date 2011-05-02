#include "../globals.h"
#include "../application.h"
#include "../fonts.h"
#include "gui_object.h"
#include "../globals.h"
#include "../application.h"
#include "gui_object.h"
#include "gui_textbox.h"
#include "gui_group.h"
#include "gui_button.h"
#include "gui_keyboard.h"
#include "../gui.h"
#include "../script.h"


gui_keyboard::gui_keyboard(app_wiiradio* _theapp) :
    pos_x(0), pos_y(0), caps(0), shift(0)
{
    obj_type = GUI_KEYBOARD;
    theapp = _theapp;
    s_w = SCREEN_WIDTH;
    s_h = SCREEN_HEIGHT;
}

void gui_keyboard::SetSmallKeyOut(const char* img)
{
    strcpy(key_small_out,img);
}

void gui_keyboard::SetSmallKeyOver(const char* img)
{
    strcpy(key_small_over,img);
}

void gui_keyboard::SetMedKeyOut(const char* img)
{
    strcpy(key_med_out,img);
}

void gui_keyboard::SetMedKeyOver(const char* img)
{
    strcpy(key_med_over,img);
}

void gui_keyboard::SetSpaceKeyOut(const char* img)
{
    strcpy(key_space_out,img);
}

void gui_keyboard::SetSpaceKeyOver(const char* img)
{
    strcpy(key_space_over,img);
}

void gui_keyboard::create()
{

    memset(dest_buffer,0,1024);
    caps = 0;
    shift =0;
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
                key_buttons[i][j]->set_images(key_small_out,key_small_over,0,0);
                key_buttons[i][j]->set_text(txt);
                key_buttons[i][j]->pad_y = 5;
                key_buttons[i][j]->text_color = this->text_color;
                key_buttons[i][j]->text_color_over = this->text_color_over;
                key_buttons[i][j]->font_sz = FS_SYSTEM;
                key_buttons[i][j]->center_text = true;
                key_buttons[i][j]->parent = this;

            }
        }
    }


    // space
    key_space = 0;
    key_space = new gui_button(theapp,pos_x+0, pos_y+(4*42+80),0,0,false);
    key_space->set_images(key_space_out,key_space_over,0,0);
    key_space->pad_y = 0;
    key_space->s_x = (SCREEN_WIDTH/2)-(key_space->s_w/2);
    key_space->text_color = this->text_color;
    key_space->text_color_over =this->text_color_over;
    key_space->font_sz = FS_SYSTEM;
    key_space->center_text = true;
    key_space->parent = this;

    key_backspace = 0;
    key_backspace = new gui_button(theapp,pos_x+(10*42+40), pos_y+(0*42+80),0,0,false);
    key_backspace->set_images(key_med_out,key_med_over,0,0);
    key_backspace->pad_y = 5;
    key_backspace->set_text("Back");
    key_backspace->text_color = this->text_color;
    key_backspace->text_color_over = this->text_color_over;
    key_backspace->font_sz = FS_SYSTEM;
    key_backspace->center_text = true;
    key_backspace->parent = this;


    key_caps = 0;
    key_caps = new gui_button(theapp,pos_x+0, pos_y+(2*42+80),0,0,false);
    key_caps->set_images(key_med_out,key_med_over,0,0);
    key_caps->pad_y = 5;
    key_caps->set_text("Caps");
    key_caps->text_color = this->text_color;
    key_caps->text_color_over = this->text_color_over;
    key_caps->font_sz = FS_SYSTEM;
    key_caps->center_text = true;
    key_caps->parent = this;


    key_shift = 0;
    key_shift = new gui_button(theapp,pos_x+21, pos_y+(3*42+80),0,0,false);
    key_shift->set_images(key_med_out,key_med_over,0,0);
    key_shift->pad_y = 5;
    key_shift->set_text("Shift");
    key_shift->text_color = this->text_color;
    key_shift->text_color_over = this->text_color_over;
    key_shift->font_sz = FS_SYSTEM;
    key_shift->center_text = true;
    key_shift->parent = this;

}

gui_keyboard::~gui_keyboard()
{
    delete key_space;
    delete key_backspace;
    delete key_shift;
    delete key_caps;

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

}

void gui_keyboard::update_keys()
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
}

int gui_keyboard::hit_test(const SDL_Event *event)
{
   if(!IsVisible())
        return 0;



    for(int i=0; i<4; i++)
    {
        for(int j=0; j<11; j++)
        {
            if (key_buttons[i][j])
            {
                if(key_buttons[i][j]->hit_test(event)==B_CLICK)
                {
                    char tb[2] = {0};

                    if(shift || caps)  tb[0] = keys[i][j].chShift;
                    else tb[0] = keys[i][j].ch;

                    strcat(dest_buffer,tb);
                    theapp->GetScript()->SetVariableString(gui_object::get_var(),dest_buffer);

                    return 1;
                }
            }
        }
    }

    if (key_space->hit_test(event)==B_CLICK)
    {
        strcat(dest_buffer," ");
        theapp->GetScript()->SetVariableString(gui_object::get_var(),dest_buffer);
        return 1;
    }

    if (key_backspace->hit_test(event)==B_CLICK)
    {
        char* ptr = dest_buffer;

        while(*++ptr != 0);

        if (dest_buffer == ptr) *dest_buffer = 0;
        else *(ptr-1) = 0;

        theapp->GetScript()->SetVariableString(gui_object::get_var(),dest_buffer);
        return 1;
    }

    if(key_shift->hit_test(event)==B_CLICK)
    {
        shift ^= 1;
        update_keys();

        return 1;
    }

    if(key_caps->hit_test(event)==B_CLICK)
    {
        caps ^= 1;
        update_keys();

        return 1;
    }


    return 0;
}

int gui_keyboard::draw()
{
    if (!(parent ? parent->visible : 1))
    {
        visible = false;
        return 0;
    }

    visible = true;

    key_space->draw();
    key_backspace->draw();
    key_caps->draw();
    key_shift->draw();

    for(int i=0; i<4; i++)
        for(int j=0; j<11; j++)
            if (key_buttons[i][j])
                key_buttons[i][j]->draw();


    return 0;
}
