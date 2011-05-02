#ifndef GUI_SCREEN_H_INCLUDED
#define GUI_SCREEN_H_INCLUDED

#include "../textures.h"
#include "../fonts.h"

class app_wiiradio;

class gui_screen : public gui_object
{

    public:

    gui_screen(app_wiiradio* _theapp);
    ~gui_screen();

    // -- set the images for this gui object
    void set_image_img(char* theimg);
    // -- draw to screen
    virtual int draw();
    bool inline point_in_rect(const int x, const int y);
    //-- hit test
    virtual int hit_test(SDL_Event *event, int current_z);

    // -- test the z-order
    bool test_zorder(const int c);
};


#endif // GUI_OBJECT_H_INCLUDED
