#ifndef GUI_DLG_H_INCLUDE
#define GUI_DLG_H_INCLUDE

class gui_button;
class gui_dlg
{
    public:

    gui_dlg() {};
    virtual ~gui_dlg() {};

    gui*           thegui;

    gui_button*    cancel;
    SDL_Surface*   dest;
    fonts*         fnts;
    char           text[1025];

    virtual void init(){};
    virtual void draw(){};
    virtual void draw(void*){};
    virtual int  handle_events(SDL_Event* e){ return 0; };

};


#endif // GUI_DLG_H_INCLUDE
