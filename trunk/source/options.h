#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

class app_wiiradio;
class wr_options
{
    public:

    wr_options(app_wiiradio*);

    bool load_options();
    void save_options();

    private:

    app_wiiradio*   theapp;

    void set_defaults();

    public:


    public:

    int             owidescreen;
    int             oscrolltext;
    int             oscreensavetime;
    char            ocurrentskin[SMALL_MEM];
    char            ocurrentlang[SMALL_MEM];
    int             oservicetype;
    int             oripmusic;
    int             ostartfromlast;
    unsigned int    osleep_timer_time; // sleep timer time, intentionally not saved
    int             ovisual_mode;
    int             ovolume;


    public:

    const char* GetOptionStr(const char* name);
    const int GetOptionInt(const char* name);

    void SetOptionStr(const char* name);
    void SetOptionInt(const char* name, const int);
};
#endif // OPTIONS_H_INCLUDED
