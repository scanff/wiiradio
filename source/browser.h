
#include "station_cache.h"

struct station_list {
    station_list() :    station_name(0), station_playing(0), station_id(0),  codec_type(0),
                        nextnode(0) {}
    char* station_name; // name
    char* station_playing; // playing
    char* station_id; // url
    int   codec_type;


    struct station_list *nextnode; // pointer to next
};

class shoutcast_browser
{
    public:

    #define MAX_PAGE_SIZE   (200000)

    station_list*   sl_first;
    station_list*   sl;
    network*    net;

    int         total_items;

    shoutcast_browser() :  sl_first(0), sl(0), net(0), total_items(0)
    {
        net = new network;
    };
    ~shoutcast_browser()
    {

        delete net;
        net = 0;

        clear_list();
    };

    void clean_small_mem(void* p,int sz)
    {
        if (p) memset(p,0,sz);
    };

    struct station_list* new_sl_list()
    {
        station_list* sl_current = 0;

        if (!sl_first)
        {
            sl_first = new station_list;
            sl_current = sl_first;

        }else{

            station_list* sl_prior = sl;

            if(!sl) {
                sl = new station_list;
                sl_first->nextnode = sl;
            }else{
                sl = new station_list;
                sl_prior->nextnode = sl;
            }
            sl_current = sl;
        }
        if (sl_current) {
            sl_current->station_name = new char[SMALL_MEM];
            clean_small_mem(sl_current->station_name,SMALL_MEM);
            sl_current->station_playing = new char[SMALL_MEM];
            clean_small_mem(sl_current->station_playing,SMALL_MEM);
            sl_current->station_id = new char[TINY_MEM];
            clean_small_mem(sl_current->station_id,TINY_MEM);
        }

        return sl_current;
    };

    void delete_members(station_list* sl_current)
    {
        if (sl_current) {
            delete [] sl_current->station_name;
            sl_current->station_name = 0;

            delete [] sl_current->station_playing;
            sl_current->station_playing = 0;

            delete [] sl_current->station_id;
            sl_current->station_id = 0;

        }
    };

    void clear_list()
    {
        struct station_list* fsl = sl_first;
        struct station_list* nsl = 0;
        while(fsl) {

            nsl = fsl->nextnode;

            if (fsl) {
                delete_members(fsl);

                delete fsl;
                fsl = 0;
            }

            fsl = nsl;
        }

        sl_first = 0;
        sl = 0;
        total_items = 0;
    };

    int connect(const char* path,char* genre)
    {
        clear_list();

        char* current_page = 0;
        current_page = new char[MAX_PAGE_SIZE];
        if(!current_page) return 0;

        memset(current_page,0,MAX_PAGE_SIZE);

        if (!refresh_genre_cache)
        {
            // use the cache data
            if (!load_cache(genre,current_page)) refresh_genre_cache = true;
        }

        unsigned long page_size = 0;

        if (refresh_genre_cache)
        {

            if (net->client_connect((char*)"www.shoutcast.com",80,TCP)) {

                if(net->send_http_request((char*)"GET",(char*)path,(char*)"www.shoutcast.com"))
                {
                    int len = 1;
                    unsigned long start_time = get_tick_count();
                    //ok ... lets real in the web page.
                    while (len > 0)
                    {
                        if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                        len = net->client_recv(current_page+page_size,MAX_NET_BUFFER);
                        if (len > 0) page_size += len;
    #ifdef _WII_
                        if(len==-11) len = 1;//WOULDBLOCK
    #endif
    #ifdef _WIN32
                        if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
    #endif

                    }


                }
                // close the connection
                net->client_close();

                // validate that we got good data!!!!!
                if (strstr(current_page,SC_DOWN) || page_size <=0) {
                    // bad !!! .... ShoutCast.com DB is busy or down ... look for a cache if we have one
                    if (!load_cache(genre,current_page))
                    {
                        // nothing more we can do ... try again later :(
                        sc_error = true;
                        return 0;
                    }
                }else{
                    // save a cache of the latest
                    save_cache(current_page,page_size,genre);
                }

            }else return 0;
        }

        parse_page(current_page);

        delete [] current_page;
        current_page = 0;

        return 1;

    }


    char* get_attrib(char* data,const char* attrib,char* attib_buffer,int max)
    {
        char* attrib_start = 0;
        char* attrib_end = 0;

        attrib_start = strstr(data,attrib);
        if (!attrib_start) return 0;

        attrib_start += strlen(attrib);

        attrib_end = strstr(attrib_start,"\"");
        if (!attrib_end) return 0;

        int cpy_len = attrib_end-attrib_start;
        if (cpy_len>max)
            memcpy(attib_buffer,attrib_start,max);
        else if (cpy_len==0) strcpy(attib_buffer,"Unknown");
        else memcpy(attib_buffer,attrib_start,cpy_len);

        return attrib_end;

    };

    int get_attrib_int(char* data,const char* attrib)
    {
        char* attrib_start = 0;
        char* attrib_end = 0;
        int max = 8;
        char tmp_int[10] = {0};

        attrib_start = strstr(data,attrib);
        if (!attrib_start) return 0;

        attrib_start += strlen(attrib);

        attrib_end = strstr(attrib_start,"\"");
        if (!attrib_end) return 0;

        int cpy_len = attrib_end-attrib_start;
        if (cpy_len>max)
            memcpy(tmp_int,attrib_start,max);
        else if (cpy_len==0) strcpy(tmp_int,"0");
        else memcpy(tmp_int,attrib_start,cpy_len);

        return atoi(tmp_int);

    };

    void parse_page(char* data)
    {
        char* stream_pos = data;
        char codec_tmp[TINY_MEM] = {0};
        char name_tmp[SMALL_MEM] = {0};

        sc_error = false; // set to false as we have a list of stations


        while(1) { // loop through the xml .... may take time, some return 1K items

            memset(codec_tmp,0,TINY_MEM);
            memset(name_tmp,0,SMALL_MEM);

            stream_pos = strstr(stream_pos,"<station name=\""); //class name we are looking for
            if (stream_pos)
            {

                stream_pos = get_attrib(stream_pos,"<station name=\"",name_tmp,SMALL_MEM-1); //-1 for null terminator
                if (stream_pos) {

                    get_attrib(stream_pos,"mt=\"",codec_tmp,TINY_MEM-1);//-1 for null terminator
                    // check it's a playable stream

                    if (strstr(codec_tmp,"audio/mpeg") != 0)
                    { // Only add MP3 streams to the browser
                        station_list* sl_new = new_sl_list();
                        if (!sl_new) return;
                        total_items++; //running count for validity check

                        strcpy(sl_new->station_name,name_tmp);
                        get_attrib(stream_pos,"id=\"",sl_new->station_id,TINY_MEM-1);//-1 for null terminator
                        get_attrib(stream_pos,"ct=\"",sl_new->station_playing,TINY_MEM-1);//-1 for null terminator

                        // just set to zero right now, when more codecs are supported we can do something with this
                        sl_new->codec_type = 0;

                    }


                    stream_pos = strstr(stream_pos,"</station>");
                }

            }

            if (!stream_pos) break;

        }

    };

};
