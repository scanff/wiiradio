#ifndef GUI_IMG_ANIM_H_INCLUDED
#define GUI_IMG_ANIM_H_INCLUDED

#include <vector>

class gui_img;
class app_wiiradio;
class gui_imganim : public gui_object
{
    public:

    unsigned int    frame_time;
    unsigned int    time_l;
    u32             index;

    vector<gui_img*>   items;

    void (*callback)(app_wiiradio*);

    gui_imganim(app_wiiradio* _theapp);
    ~gui_imganim();

    inline int new_img(char* path, char* file);
    void parse_imgstr(char*,char*);
    void set_images(char* path, char* image_str);
    void create();


    virtual int draw();
    void set_text(char* text);
    int hit_test(SDL_Event *event, int current_z);
    void set_on(int i);


};

#endif
