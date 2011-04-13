#ifndef _LOCAL_PLAYER_H_
#define _LOCAL_PLAYER_H_

#include "id3.h"

class local_player : public id3
{
public:


    char            track_title[SMALL_MEM]; // track info
    char            last_track_title[SMALL_MEM]; // track info - last

    // vars
    unsigned long   buffered;
    unsigned long   pre_buffer; // buffer before playback ...
    unsigned long   buffer_size;
    char*           buffer;
    bool            bufferring;

    unsigned long   last_chunk;
    unsigned long   buffers_recvd;
    unsigned long   buffers_sent;

    unsigned long   lastread;
    unsigned long   filepos;
    unsigned long   fileendpos;
    unsigned long   filesentpos;
    bool            at_end;

    #define BUFFER_CHUNK            (5000)
    #define READ_SIZE_FILE          (5000)

    char            localdata[READ_SIZE_FILE];

    string          display_name;

public:

    local_player();
    ~local_player();

    private:

    void inline check_filename(const char* ifilename);
    void inline buffer_data(const char* net_buffer,const int len);
    int check_atend(int len);

    public:


    void reset();
    int local_read(const char* ifilename);
    int get_buffer(void* cbuf,int len);

};

#endif

