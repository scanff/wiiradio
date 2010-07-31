#ifndef DISKSPACE_H_INCLUDED
#define DISKSPACE_H_INCLUDED

#ifndef _WIN32

#include <sys/statvfs.h>

int get_mediasize_mb()
{
    char media[20];
    int size_mb = 0;

    sprintf(media,"%s:/",g_storage_media);

    /*
        Thanks teknecal for this info
    */

    struct statvfs fiData;
    if (statvfs(media,&fiData) < 0) {
        return 0;
    }
    else{
        //sd_used = fiData.f_blocks*fiData.f_bsize/1000000;
        size_mb = fiData.f_bfree;
        size_mb = size_mb / 1000;
        size_mb = round(size_mb * fiData.f_bsize);
        size_mb = size_mb / 1000;
    }


    return size_mb;
}
#else
int get_mediasize_mb() { return 0;}
#endif

#endif // DISKSPACE_H_INCLUDED
