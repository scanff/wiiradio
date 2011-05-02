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

};

#endif
