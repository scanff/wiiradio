#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

void click_next(void* arg);
void click_back(void* arg);
void click_add(void* arg);
void click_stop(void* arg);
void click_exit(void* arg);
void click_browser(void* arg);
void click_search(void* arg);
void click_nextskin(void* arg);
void click_localbrowser(void* arg);
void click_dirup(void* arg);
void click_playpause(void* arg);
void click_showvisuals(void* arg);
void click_logo(void* arg);
void click_playing(void* arg);
void click_genres(void* arg);
void click_playlists(void* arg);
void click_item(void* arg);
void click_delete_item(void* arg);
void click_nexttrack(void* arg);
void click_priortrack(void* arg);
void click_replay(void* arg);
void click_cancel_connect(void* arg);
void click_cancel_buffering(void* arg);
void click_lastscreen(void* arg);
void click_showinfo(void* arg);
void click_show(void* arg);
void click_seek(void* arg);
void click_close_wnd(void* arg);
void click_set_local_loc(void* arg);
void click_show_wnd(void* arg);
void click_connect(void* arg);
void click_searchongenre(void* arg);
void click_showconnect(void* arg);
void click_add_genre(void* arg);

struct _smart_functions
{
    const char* name;
    func_void   fun;
};
//extern int total_sm_fun;
extern _smart_functions on_click_fun[];

inline int sf_get_size()
{
    int i = 0;

    _smart_functions* x = &on_click_fun[i];

    while(x->name) x = &on_click_fun[i++];

    return (i-1);
}

#endif
