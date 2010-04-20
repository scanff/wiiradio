
#include "station_cache.h"

struct station_list {
    station_list() :    station_name(0), station_playing(0), station_id(0),  codec_type(0),
                        service_type(0), nextnode(0) {}
    char* station_name; // name
    char* station_playing; // playing
    char* station_id; // url
    int   codec_type;
    int   service_type; // shoutcast, icecast, etc...


    struct station_list *nextnode; // pointer to next
};

class shoutcast_browser
{
    public:

    /*
        .:: type ::.

        0 SHOUTCAST
        1 ICECAST

    */

    #define SHOUTCAST_DB_URL    ("www.shoutcast.com")
    #define ICECAST_DB_URL      ("dir.xiph.org")
    #define MAX_PAGE_SIZE   (1500000) // 1.5MB .. Icecast is massive

    station_list*   sl_first;
    station_list*   sl;
    network*    net;

    int         total_items;
    int         type; // shoutcast, icecast .. other?

    shoutcast_browser() :  sl_first(0), sl(0), net(0), total_items(0), type(0)
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

    // I don't think SHOUTCAST returns Content-Length in HEAD
    unsigned long get_header(char* connect_url, char* lpath)
    {
        char packet[1024] = {0};
        char header_buffer[2048] = {0};
        int header_pos = 0;

        if (net->client_connect(connect_url,80,TCP))
        {

            if(net->send_http_request((char*)"HEAD",(char*)lpath,connect_url))
            {
                int len = 1;
                unsigned long start_time = get_tick_count();

                while (len > 0)
                {
                    if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                    len = net->client_recv(packet,1024);
                    if (len > 0)
                    {
                        if (header_pos+len >= 2048) break; // too much data
                        memcpy(header_buffer+header_pos,packet,len);
                        header_pos += len;
                    }
#ifdef _WII_
                    if(len==(-EAGAIN)) len = 1;//WOULDBLOCK
#endif
#ifdef _WIN32
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
#endif
#ifdef _LINUX_
                    if(net->client_connected) len = 1;
#endif

                }

            }
            // close the connection
            net->client_close();
        }

        char content_len[10] = {0};
        char* start = strstr(header_buffer,"Content-Length: ");

        if (start)
        {
            start += strlen("Content-Length: ");
            char* end = strstr(start,"\r\n");
            if (end)
            {
                int len = end-start;
                if (len > 0)
                {
                    memcpy(content_len,start,len);
                    return atol(content_len);
                }
            }
        }

        return 0;
    };

    int connect(const char* path,char* genre,int itype = 0 /* SHOUTCAST DEFAULT */)
    {
        type = itype;

        clear_list();

        char lpath[255] = {0};
        char lgenre[255] = {0};
        char* connect_url = 0;

        switch(type)
        {
            case SERVICE_SHOUTCAST:
                strcpy(lpath,path);
                strcpy(lgenre,genre);
                connect_url = (char*)SHOUTCAST_DB_URL;

            break;
            case SERVICE_ICECAST:
                strcpy(lpath,"/yp.xml");
                // can't find a way to request genres with the backend XML ? HELP !
                sprintf(lgenre,"icecast_%s",genre);
                connect_url = (char*)ICECAST_DB_URL;
            break;
            default: //???
            break;
        }

        char* current_page = 0;
        current_page = new char[MAX_PAGE_SIZE];
        if(!current_page) return 0;

        memset(current_page,0,MAX_PAGE_SIZE);

        if (!refresh_genre_cache)
        {
            // use the cache data
            if (!load_cache(lgenre,current_page)) refresh_genre_cache = true;
        }

        unsigned long page_size = 0;
        unsigned long size = MAX_PAGE_SIZE;
        if (refresh_genre_cache)
        {
            // don't bother SHOUTcast does not return content-len in HEAD
            if (SERVICE_SHOUTCAST != type)
            {
                if ((size=get_header(connect_url,lpath)) > 0)
                {
                    delete [] current_page;
                    current_page = 0;

                    current_page = new char[size+10]; // guard
                    if(!current_page) return 0;
                }
            }

            if (net->client_connect(connect_url,80,TCP)) {

                if(net->send_http_request((char*)"GET",(char*)lpath,connect_url))
                {
                    int len = 1;
                    unsigned long start_time = get_tick_count();
                    int tmp_size = 3000;
                    char* tmp_buffer = 0;
                    tmp_buffer = new char[tmp_size];
                    if (!tmp_buffer)
                    {
                        delete [] current_page;
                        return 0;
                    }
                    //ok ... lets real in the web page.
                    while (len > 0)
                    {
                        if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                        len = net->client_recv(tmp_buffer,tmp_size);

                        if (len > 0)
                        {
                            if (page_size+len >= size) break;

                            memcpy(current_page+page_size,tmp_buffer,len);
                            page_size += len;

                        }
    #ifdef _WII_
                        if(len==(-EAGAIN)) len = 1;//WOULDBLOCK
    #endif
    #ifdef _WIN32
                        if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
    #endif
    #ifdef _LINUX_
                        if(net->client_connected) len = 1;
    #endif

                    }

                    // temp buf
                    delete [] tmp_buffer;
                }

                // close the connection
                net->client_close();

                // validate that we got good data!!!!!
                if (strstr(current_page,SC_DOWN) || page_size <=0) {
                    // bad !!! .... ShoutCast.com DB is busy or down ... look for a cache if we have one
                    if (!load_cache(lgenre,current_page))
                    {
                        // nothing more we can do ... try again later :(
                        sc_error = true;
                        return 0;
                    }
                }else{
                    // save a cache of the latest
                    save_cache(current_page,page_size,lgenre);
                }

            }else return 0;
        }

        parse_page(current_page,genre);

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

    char* get_element(char* data,const char* element,char* element_buffer,int max)
    {
        char* element_start = 0;
        char* element_end = 0;
        char element_xml_start[100] = {0};
        char element_xml_end[100] = {0};

        sprintf(element_xml_start,"<%s>",element);
        sprintf(element_xml_end,"</%s>",element);


        element_start = strstr(data,element_xml_start);
        if (!element_start) return 0;

        element_start += strlen(element_xml_start);

        element_end = strstr(element_start,element_xml_end);
        if (!element_end) return 0;

        int cpy_len = element_end-element_start;
        if (cpy_len>max)
            memcpy(element_buffer,element_start,max);
        else if (cpy_len==0) strcpy(element_buffer,"Unknown");
        else memcpy(element_buffer,element_start,cpy_len);

        return element_end;

    };



    /*
        genre_filter

        ICEcast does not provide an api that I can find to search for genres.
        We will have to filter the whole list for the genre we've searched for.
    */
    void parse_page(char* data,char* genre_filter)
    {
        char* stream_pos = data;
        char codec_tmp[TINY_MEM] = {0};
        char name_tmp[SMALL_MEM] = {0};

        sc_error = false; // set to false as we have a list of stations


        if (type == SERVICE_SHOUTCAST)
        { // SHOUTcast

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
                            sl_new->service_type = SERVICE_SHOUTCAST; // shoutcast ... TODO enum's
                        }


                        stream_pos = strstr(stream_pos,"</station>");
                    }

                }

                if (!stream_pos) break;

            }
        }
        else if (type == SERVICE_ICECAST)
        { // ICECast

             while(1) { // loop through the xml .... may take time, some return 1K items

                memset(codec_tmp,0,TINY_MEM);
                memset(name_tmp,0,SMALL_MEM);

                stream_pos = strstr(stream_pos,"<entry>"); //class name we are looking for
                if (stream_pos)
                {

                    stream_pos = get_element(stream_pos,"server_name",name_tmp,SMALL_MEM-1); //-1 for null terminator
                    if (stream_pos)
                    {
                        get_element(stream_pos,"server_type",codec_tmp,TINY_MEM-1);

                        if (strstr(codec_tmp,"audio/mpeg") != 0)
                        { // Only add MP3 streams to the browser
                            char genres_tmp[SMALL_MEM]={0};

                            get_element(stream_pos,"genre",genres_tmp,SMALL_MEM-1);

                            string_lower(genres_tmp); // format string from XML file

                            if (strstr(genres_tmp,genre_filter))
                            {
                                // only add a good match to genre

                                station_list* sl_new = new_sl_list();
                                if (!sl_new) return;
                                total_items++; //running count for validity check

                                strcpy(sl_new->station_name,name_tmp);
                                get_element(stream_pos,"listen_url",sl_new->station_id,TINY_MEM-1);//-1 for null terminator
                                get_element(stream_pos,"current_song",sl_new->station_playing,TINY_MEM-1);//-1 for null terminator

                                // just set to zero right now, when more codecs are supported we can do something with this
                                sl_new->codec_type = 0;
                                sl_new->service_type = SERVICE_ICECAST; // icecast ... TODO enum's
                            }
                        }

                        stream_pos = strstr(stream_pos,"</entry>");
                    }

                }

                if (!stream_pos) break;

            }

        }
    };

    void string_lower(char* string)
    {
        int len = strlen(string) + 1;
        char* copy = 0;
        int i = 0;
        char c;

        copy = new char[len];
        if (!copy) return;

        strcpy(copy,string);

        while (copy[i])
        {
            c=copy[i];
            string[i] = tolower(c);
            i++;
        }

        delete [] copy;

    };

    // --- end
};
