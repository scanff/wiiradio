
// a playlist can contain several streams.  Store these incase a stream is full
struct station_playlist {
    station_playlist() { reset(); };
    void reset()
    {
        nextnode = 0;
        memset(url,0,1024);
        memset(path,0,SMALL_MEM);
    };
    char url[1024]; // url
    int port;
    char path[SMALL_MEM];
    struct station_playlist *nextnode; // pointer to next
};  // TODO - build list of urls from the playlist .... only grabbing the first right now -----

class playlists
{
    public:

    #define MAX_PLAYLIST_SIZE (5000)


    station_playlist*   first_entry;

    playlists()
    {
        first_entry     = new station_playlist;
    };

    ~playlists()
    {
        delete first_entry;
        first_entry = 0;
    };

   /* int get_head(char* trailing_path)
    {
        network* net = 0;
        char* current_page = 0;
        unsigned long content_size = 0;

        net = new network;
        if (!net) return 0;

        if (net->client_connect((char*)"yp.shoutcast.com",80,TCP))
        {

            // request header to see how big the page is !

            char head[MAX_NET_BUFFER] = {0};

            if(net->send_http_request((char*)"HEAD",(char*)trailing_path,(char*)"yp.shoutcast.com"))
            {
                unsigned long page_size = 0;
                int len = 1;

                unsigned long start_time = get_tick_count();

                //ok ... let real in the head
                while(len > 0) {

                    if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                    len = net->client_recv(head+page_size,MAX_NET_BUFFER);
                    if (len > 0) page_size += len;
#ifdef _WII_
                    if(len==EAGAIN) len =1;//WOULDBLOCK
#else
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
#endif

                }



                if (len < 0) return 0;

                content_size = 0;
                char* start = strstr(head,"Content-Length: ");
                if (start) {
                    start += strlen("Content-Length: ");
                    char* end = strstr(start,"\r\n");
                    if (end) content_size = end - start + strlen(head);

                }

            }

            net->client_close();
        }

        delete net;
        net = 0;

    };
*/
    int get_playlist(const char* path)
    {
        int ret = 0;
        network* net = 0;
        char* current_page = 0;
       // unsigned long content_size = 0;

        net = new network;
        if (!net) {
            return -1000;
        }

        current_page = new char[MAX_PLAYLIST_SIZE];
        if (!current_page)
        {
            delete net;
            net = 0;
            return -1001;
        }

        memset(current_page,0,MAX_PLAYLIST_SIZE);


        char trailing_path[100] = {0};

        sprintf(trailing_path,"/sbin/tunein-station.pls?id=%s",path);

        unsigned long page_size = 0;

     //   get_head(trailing_path); // TODO handle http header ... using Content-Length to allocate the buffer!

        int len;
        if (net->client_connect((char*)"yp.shoutcast.com",80,TCP)) {

            // get the page !
            if(net->send_http_request((char*)"GET",(char*)trailing_path,(char*)"yp.shoutcast.com"))
            {
                len = 1;
                while(len > 0) {
                    len = net->client_recv(current_page+page_size,MAX_NET_BUFFER);
                    if (len > 0) page_size += len;
#ifdef _WII_
                    if(len==EAGAIN) len = 1;//WOULDBLOCK
#endif
#ifdef _WIN32
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
#endif
#ifdef _LINUX_
                    if(net->client_connected) len = 1;
#endif
                }

            }
            else ret = -1004;

            net->client_close();
        }
        else ret = -1003;

        parse_playlist(current_page);

        delete [] current_page;
        current_page = 0;
        if (len < 0) {
            return len;
        }
        return ret;
    };

    void split_url(char* url)
    {
        first_entry->reset();

        int part = 0;
        char* split = url;
         // ignore http://
        char* search = strstr(url,"http://");
        if(!search) return;

        search += strlen("http://");

        split = strtok(search,":/");
        while(split) {

            if(!part) {
                strcpy(first_entry->url,split);
            }else if(part==1) {
                first_entry->port = atoi(split);
                if (first_entry->port == 0) {
                    strcat(first_entry->path,"/");
                    strcat(first_entry->path,split);
                }
            }else{
                strcat(first_entry->path,"/");
                strcat(first_entry->path,split);
            }

            split = strtok(0,":/");
            part++;
        }
    }

    // TODO - build list of urls from the playlist .... only grabbing the first right now
    char* parse_playlist(char* cp)
    {
        char url[1024] = {0};
        char* start = 0;
        char* end = 0;

        start = strstr(cp,"File1=");
        if (start) {
            start += strlen("File1=");
            end = strstr(start,"\n");
            if (end) {
                memcpy(url,start,end-start);
                if(*url) split_url(url);
            }
        }

        return 0;
    };

};
