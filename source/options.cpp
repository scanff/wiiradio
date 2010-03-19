#include "globals.h"
#include "options.h"

int g_owidescreen;
int g_oscrolltext;
int g_screensavetime;
char g_currentskin[SMALL_MEM];
char g_currentlang[SMALL_MEM];
int g_servicetype;

void parse_options(char* buf)
{
    sscanf(buf,"ws %d\nst %d\nss %d\nskin %s\nlang %s\nservice %d\n",&g_owidescreen,&g_oscrolltext,&g_screensavetime,g_currentskin,g_currentlang,&g_servicetype);
}

void set_defaults()
{
    g_owidescreen = 0;
    g_oscrolltext = 0;
    g_screensavetime = 0;
    g_servicetype = SERVICE_SHOUTCAST;
}

bool load_options()
{
    set_defaults();

    FILE* f = 0;

    f = fopen(make_path("options.dat"),"r");
    if (!f) return false;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);

    char* options_data = 0;
    options_data = new char[size+1];
    if (!options_data)
    {
        fclose(f);
        return false;
    }

    memset(options_data,0,size+1);


    fread(options_data,size,1,f);

    parse_options(options_data);

    delete[] options_data;
    options_data = 0;

    fclose(f);

    return true;

}

void save_options()
{

    FILE* f = 0;

    f = fopen(make_path("options.dat"),"w");
    if (!f) return;

    fprintf(f,"ws %d\nst %d\nss %d\nskin %s\nlang %s\nservice %d\n",g_owidescreen,g_oscrolltext,g_screensavetime,g_currentskin,g_currentlang,g_servicetype);

    fclose(f);
}
