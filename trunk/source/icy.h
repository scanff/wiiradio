#ifndef _ICY_H_
#define _ICY_H_

#ifdef ICY_DEBUG
#define DEB(x) { printf(x); }
#else
#define DEB(x) {}
#endif

// ICY header data fields
#include "application.h"
#include "ripper.h"

class icy
{
public:

    app_wiiradio*   theapp;
    #define ICY_META_MAX (9)

    // icy header/metadata
    char            icy_notice1[SMALL_MEM]; // notice 1
    char            icy_notice2[SMALL_MEM]; // notice 2
    char            icy_name[SMALL_MEM]; // station name
    char            icy_genre[SMALL_MEM]; // station genre
    char            icy_pls_url[SMALL_MEM]; // url in playlist
    char            icy_url[SMALL_MEM]; // url of stream
    char            content_type[SMALL_MEM]; // type
    int             icy_pub; // public
    unsigned int    icy_metaint; // metaint .. very important
    int             icy_br; // bitrate
    char            track_title[SMALL_MEM]; // track info
    char            last_track_title[SMALL_MEM]; // track info - last

    // vars
    unsigned long   metaint_pos;
    bool            looking_for_header;
    int             redirect_detected;
    unsigned long   buffered;
    unsigned long   pre_buffer; // buffer before playback ...
    unsigned long   buffer_size;
    char*           buffer;
    bool            bufferring;
    char            metaint_buffer[MAX_METADATA_SIZE];
    int             metaint_size;
    int             metaint_receive_pos;

    unsigned long   last_chunk;
    unsigned long   buffers_recvd;
    unsigned long   buffers_sent;

    char*           temp_buffer;

    stream_ripper*  ripper;

    const char icy_meta[ICY_META_MAX][20];

    long sent;
public:

    icy(app_wiiradio* _theapp);
    ~icy();
    void clean_icy_data();
    void icy_reset();

    void icy_stream_reset();

private:
    inline void icy_ripdata(char* data, int len);
    void parse_header_item(const char* obj);
    void parse_http_redirect(char *buf);
    int parse_header(char* buf);
    void parse_meta_data(char* data);
    int parse_metaint(char* net_buffer,int len);

public:
    void buffer_data(char* net_buffer,int len);
    const int get_buffer(void* cbuf,int len);

};

#endif //_ICY_H_

