#include "globals.h"
#include "station_cache.h"


void save_cache(char* data,long size,char* genre)
{
    if (!genre) return;

    char cache_path[MED_MEM];

    sprintf(cache_path,"data/cache/%s.tmp",genre);

    FILE* f = 0;
    f = fopen(make_path(cache_path),"w");

    if (!f) return;

    fwrite(data,size,1,f);

    fclose(f);
};


bool load_cache(char* genre,char* buffer)
{
    char cache_path[MED_MEM];

    sprintf(cache_path,"data/cache/%s.tmp",genre);

    FILE* f = 0;
    f = fopen(make_path(cache_path),"r");

    if (!f) return false;

    fseek(f,0,SEEK_END);
    unsigned int size = ftell(f);
    fseek(f,0,SEEK_SET);

    fread(buffer,1,size,f);

    fclose(f);

    return true;

};
