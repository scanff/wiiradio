#include "globals.h"
#include "options.h"

#include <fstream>
#include <string>

int g_owidescreen;
int g_oscrolltext;
int g_screensavetime;
char g_currentskin[SMALL_MEM];
char g_currentlang[SMALL_MEM];
int g_servicetype;
int g_oripmusic;
int g_startfromlast;

void set_defaults()
{
    g_owidescreen = 0;
    g_oscrolltext = 0;
    g_screensavetime = 0;
    g_servicetype = SERVICE_SHOUTCAST;
    g_oripmusic = 0;
}

bool load_options()
{
    set_defaults();

    ifstream file;

    string line;
    char option[255], value[255];

    file.open(make_path("options.dat"));
    file.is_open();
    if (file.fail()) return false;

    while (!file.eof())
    {
        getline(file, line);
        if (sscanf(line.c_str(), "%254s %254s", option, value) == 2)
        {
            if (!strcmp(option, "ws"))
                g_owidescreen = atoi(value);
            else if (!strcmp(option, "st"))
                g_oscrolltext = atoi(value);
            else if (!strcmp(option, "ss"))
                g_screensavetime = atoi(value);
            else if (!strcmp(option, "skin"))
                sscanf(value, "%254s", g_currentskin);
            else if (!strcmp(option, "lang"))
                sscanf(value, "%254s", g_currentlang);
            else if (!strcmp(option, "service"))
                g_servicetype = atoi(value);
            else if (!strcmp(option, "ripmusic"))
                g_oripmusic = atoi(value);
            else if (!strcmp(option, "startfromlast"))
                g_startfromlast = atoi(value);

        }
    }

    file.close();
    return true;
}

void save_options()
{

    FILE* f = 0;

    f = fopen(make_path("options.dat"),"w");
    if (!f) return;

    fprintf(f,"ws %d\nst %d\nss %d\nskin %s\nlang %s\nservice %d\nripmusic %d\nstartfromlast %d\n",
        g_owidescreen,
        g_oscrolltext,
        g_screensavetime,
        g_currentskin,
        g_currentlang,
        g_servicetype,
        g_oripmusic,
        g_startfromlast);

    fclose(f);
}

