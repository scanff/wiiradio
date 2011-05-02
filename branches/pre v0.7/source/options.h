#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

extern bool load_options();
extern void save_options();
extern int g_owidescreen;
extern int g_oscrolltext;
extern int g_screensavetime;
extern char g_currentskin[SMALL_MEM];
extern char g_currentlang[SMALL_MEM];
extern int g_servicetype;
extern int g_oripmusic;
extern int g_startfromlast;
extern unsigned int g_sleep_timer_time; // sleep timer time, intentionally not saved
#endif // OPTIONS_H_INCLUDED