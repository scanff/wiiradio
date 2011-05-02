#include "globals.h"
#include "options.h"
#include "application.h"
#include "script.h"

#include <fstream>
#include <string>
struct _var_hashes
{
    const char* name;
    u32     hash;
};

/* Prob. should hash these for speed */
static _var_hashes option_names[] = {
                {"ws",0},
                {"st",0},
                {"ss",0},
                {"skin",0},
                {"lang",0},
                {"service",0},
                {"ripmusic",0},
                {"startfromlast",0},
                {"volume",0},
                {"visualmode",0},
                {"sleepafter",0},
                {NULL,0}
};

wr_options::wr_options(app_wiiradio* _theapp) : theapp(_theapp)
{
    set_defaults();

}

void wr_options::set_defaults()
{
    owidescreen = 0;
    oscrolltext = 0;
    oscreensavetime = 0;
    oservicetype = SERVICE_SHOUTCAST;
    oripmusic = 0;
    osleep_timer_time = 0;
    ovisual_mode = 0;
    ovolume = 255;
    ocurrentskin[0] = '\0';
    ocurrentlang[0] = '\0';
}

bool wr_options::load_options()
{
    set_defaults();

    ifstream file;

    string line;
    char option[255], value[255];

    file.open(make_path("data/options.dat"));
    file.is_open();
    if (file.fail()) return false;

    while (!file.eof())
    {
        getline(file, line);
        if (sscanf(line.c_str(), "%254s %254s", option, value) == 2)
        {
            if (!strcmp(option, /*"ws"*/option_names[0].name))
            {
                owidescreen = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[0].name,owidescreen);
            }
            else if (!strcmp(option, /*"st"*/option_names[1].name))
            {
                oscrolltext = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[1].name,oscrolltext);
            }
            else if (!strcmp(option, /*"ss"*/option_names[2].name))
            {
                oscreensavetime = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[2].name,oscreensavetime);
            }
            else if (!strcmp(option, /*"skin"*/option_names[3].name))
            {
                sscanf(value, "%254s", ocurrentskin);
                theapp->GetScript()->SetVariableString(option_names[3].name,ocurrentskin);
            }
            else if (!strcmp(option, /*"lang"*/option_names[4].name))
            {
                sscanf(value, "%254s", ocurrentlang);
                theapp->GetScript()->SetVariableString(option_names[4].name,ocurrentlang);
            }
            else if (!strcmp(option, /*"service"*/option_names[5].name))
            {
                oservicetype = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[5].name,oservicetype);
            }
            else if (!strcmp(option, /*"ripmusic"*/option_names[6].name))
            {
                oripmusic = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[6].name,oripmusic);
            }
            else if (!strcmp(option, /*"startfromlast"*/option_names[7].name))
            {
                ostartfromlast = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[7].name,ostartfromlast);
            }
            else if (!strcmp(option, /*"volume"*/option_names[8].name))
            {
                ovolume = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[8].name,ovolume);

            }
            else if (!strcmp(option, /*"visualmode"*/option_names[9].name))
            {
                ovisual_mode = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[9].name,ovisual_mode);
            }
           /* else if (!strcmp(option, "sleepafter"option_names[10].name))
            {
                osleep_timer_time = atoi(value);
                theapp->GetScript()->SetVariableInt(option_names[10].name,osleep_timer_time);
            }*/
        }
    }

    file.close();

    int i = 0;
    while(option_names[i].name)
    {
        option_names[i].hash = MAKE_HASH(option_names[i].name);
        i++;
    }

    return true;
}


const int wr_options::GetOptionInt(const char* name)
{
    int i = 0;
    const u32 hash = MAKE_HASH(name);
    script* sc = (script*)theapp->GetScript();

    while(option_names[i].name)
    {
        if(option_names[i].hash == hash)
        {
            return sc->GetVariableInt(name);
        }
        i++;
    }

    return 0;
}

void wr_options::SetOptionStr(const char* name)
{
}
void wr_options::SetOptionInt(const char* name,const int value)
{
    int i = 0;
    const u32 hash = MAKE_HASH(name);
    script* sc = (script*)theapp->GetScript();

    while(option_names[i].name)
    {
        if(option_names[i].hash == hash)
        {
            sc->SetVariableInt(name,value);
            break;
        }
        ++i;
    }

}

void wr_options::save_options()
{

    FILE* f = 0;

    f = fopen(make_path("data/options.dat"),"wb");
    if (!f) return;

    fprintf(f,"ws %d\nst %d\nss %d\nskin %s\nlang %s\nservice %d\nripmusic %d\nstartfromlast %d\nvolume %d\nvisualmode %d\n"
        ,owidescreen
        ,oscrolltext
        ,oscreensavetime
        ,ocurrentskin
        ,ocurrentlang
        ,oservicetype
        ,oripmusic
        ,ostartfromlast
        ,ovolume
        ,ovisual_mode
        );

    fclose(f);
}

