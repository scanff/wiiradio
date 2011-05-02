#ifndef GUI_KEYBOARD_H_INCLUDED
#define GUI_KEYBOARD_H_INCLUDED
#include "gui_object.h"

class gui_button;
class gui_keyboard : public gui_object
{
    typedef struct _keytype {
        char ch, chShift;
    } Key;

    public:

    char           key_small_out[MAX_PATH];
    char           key_small_over[MAX_PATH];
    char           key_med_out[MAX_PATH];
    char           key_med_over[MAX_PATH];
    char           key_space_out[MAX_PATH];
    char           key_space_over[MAX_PATH];

    void*           userdata;

    Key             keys[4][11];
    gui_button*     key_buttons[4][11];
    gui_button*     key_space;
    gui_button*     key_caps;
    gui_button*     key_shift;
    gui_button*     key_backspace;

    char           dest_buffer[1024];

    int             pos_x;
    int             pos_y;

    int             caps;
    int             shift;

    // -- callbacks
    gui_keyboard(app_wiiradio* _theapp);
    ~gui_keyboard();

    void create();
    void update_keys();

    void SetSmallKeyOut(const char* img);
    void SetSmallKeyOver(const char* img);
    void SetMedKeyOut(const char* img);
    void SetMedKeyOver(const char* img);
    void SetSpaceKeyOut(const char* img);
    void SetSpaceKeyOver(const char* img);

    virtual int draw();
    virtual int hit_test(const SDL_Event *event);

};

#endif // GUI_KEYBOARD_H_INCLUDED
