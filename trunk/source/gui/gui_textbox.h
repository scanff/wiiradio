#ifndef GUI_TEXTBOX_H_INCLUDED
#define GUI_TEXTBOX_H_INCLUDED

class gui_textbox : public gui_object{

    public:

    SDL_Surface*    scroll_area;

    int             scroll_x;
    bool            auto_scroll_text;
    int             scroll_speed;
    bool            can_scroll;

    gui_textbox(app_wiiradio* _theapp);
    gui_textbox(app_wiiradio* _theapp,int x,int y,char* t,long tc,bool scroll);

    ~gui_textbox();

    void scroll_reset();

    void scroll_text();
    virtual int draw();

};


#endif // GUI_TEXTBOX_H_INCLUDED
