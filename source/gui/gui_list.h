#ifndef GUI_LIST_H_INCLUDED
#define GUI_LIST_H_INCLUDED
class gui_button;
class app_wiiradio;
class gui_list : public gui_object
{
    public:

    int number;
    int selected;
    int vspacing;

    gui_object*    icons;
    gui_button**   items;
    char**         item_text;
    int*           item_icon_id;
    int            listid; // id of this list

    void (*callback)(app_wiiradio*);

    gui_list(app_wiiradio* _theapp);
    gui_list(app_wiiradio* _theapp, int _num,int _x, int _y, int _w, int _h, int _spacing, void(*cb)(app_wiiradio*));
    ~gui_list();

    int GetSize() { return number; }

    void parse_text(const char* text);
    void create();
    virtual int draw();
    void set_text(char* text);
    virtual int hit_test(const SDL_Event *event);
    void set_on(int i);
    void SetIconID(const int i, const int id);
    void SetItemVisible(const int i, const bool visible);
    void SetHighlight(const int i, const bool h);


    public:

    // Item dims
    int     item_w;
    int     item_h;
    int     item_ox;
    int     item_oy;
    u32     item_bgcolor;
    u32     item_bgcolor_over;
    int     moveable;

    private:

    bool    m_down;
    int     m_start_x;
    int     m_start_y;
};

#endif
