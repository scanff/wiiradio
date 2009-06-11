#include "globals.h"
#include "options.h"

int g_owidescreen;
int g_oscrolltext;
int g_screensavetime;

void parse_options(char* buf)
{
    sscanf(buf,"ws %d\nst %d\nss %d",&g_owidescreen,&g_oscrolltext,&g_screensavetime);
}

void set_defaults()
{
    g_owidescreen = 0;
    g_oscrolltext = 0;

}

bool load_options()
{
    set_defaults();

    FILE* f = 0;

    f = fopen(make_path("options.txt"),"r");
    if (!f) return false;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);

    char* options_data = new char[size];
    if (!options_data)
    {
        fclose(f);
        return false;
    }

    memset(options_data,0,size);


    fread(options_data,size,1,f);

    parse_options(options_data);

    delete options_data;
    options_data = 0;

    fclose(f);

    return true;

}

void save_options()
{
    FILE* f = 0;

    f = fopen(make_path("options.txt"),"w");
    if (!f) return;

    fprintf(f,"ws %d\nst %d\nss %d\n",g_owidescreen,g_oscrolltext,g_screensavetime);

    fclose(f);
}
