#ifndef _PROJECTM_H_INCLUDED
#define _PROJECTM_H_INCLUDED


#include <projectM/projectM.hpp>


class projectM;
class app_wiiradio;

class projectm
{
    public:

    projectm(app_wiiradio*   _theapp);
    ~projectm();

    void render();
    void init();

    app_wiiradio* theapp;

    private:

    string      preset_name;
    string      preset_dir;
    bool        init_done;

    projectM*   pm;
    Settings config;


};
#endif
