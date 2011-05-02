#ifndef SKINS_H_INCLUDED
#define SKINS_H_INCLUDED
class gui_object;
class app_wiiradio;
class skins {
    public:

    #define MAX_SKINS  (100)

    app_wiiradio* theapp;

    char*   skin_list[MAX_SKINS]; // cap @ 100 right now
    int     total_skins;
    int     current_skin;
    char    friendly[SMALL_MEM];
    int     current_objectid;

    // gui object holders
    void*           current_obj;
    void*           last_obj;

    string          skin_path;

    skins(app_wiiradio* _theapp);
    ~skins();

    unsigned long inline make_color(char* value);
    unsigned long get_value_color(const char* search);
    int get_value_int(const char* search);
    char* get_value_string(const char* search,char* buffer);
    char* get_value_file(const char* search,char* buffer, const char* dir);
    void set_oject_id(char* name);
    void inline release_object_id();
    void object_add_var(char* name, char* value);
    void parser_skin_file(char* skin_data,char**,char**);
    const bool load_skin(const char* dir);
    void find_skins();
    char* get_current_skin();
    char* get_current_skin_path();
    void goto_next_skin();
    void set_current_skin(char* path);
    char* string_lower(char* x);
};

#endif // SKINS_H_INCLUDED
