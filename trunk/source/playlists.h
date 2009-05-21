
// a playlist can contain several streams.  Store these incase a stream is full
struct station_playlist {
    station_playlist() { reset(); };
    void reset()
    {
        nextnode = 0;
        memset(url,0,1024);
        memset(path,0,255);
    };
    char url[1024]; // url
    int port;
    char path[255];
    struct station_playlist *nextnode; // pointer to next
};  // TODO - build list of urls from the playlist .... only grabbing the first right now -----

class playlists
{
    public:

    #define MAX_PLAYLIST_SIZE (5000)

    network*            net;
    char*               current_page;
    station_playlist*   first_entry;
    unsigned long       content_size;
    playlists()
    {
        net             = new network;
        first_entry     = new station_playlist;
        current_page    = new char[MAX_PLAYLIST_SIZE];
    };

    ~playlists()
    {
        delete first_entry;
        first_entry = 0;

        delete [] current_page;
        current_page = 0;

        delete net;
        net = 0;
    };

    int get_head(char* trailing_path)
    {
        char request[255] = {0};

        if (net->client_connect("yp.shoutcast.com",80,TCP))
        {

            // request header to see how big the page is !

            sprintf(request,
                    "HEAD %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
                    "Accept: */*\r\n"
                    "Connection: Keep-Alive\r\n\r\n",
                    trailing_path,"yp.shoutcast.com");


            char head[1024] = {0};

            if (net->client_send(request,strlen(request)))
            {
                unsigned long page_size = 0;
                int len = 1;

                unsigned long start_time = get_tick_count();

                //ok ... let real in the head
                while(len > 0) {

                    if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                    len = net->client_recv(head+page_size,1024);
                    if (len > 0) page_size += len;
#ifdef _WII_
                    if(len==-11) len =1;//WOULDBLOCK
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


    };

    void get_playlist(const char* path)
    {
        memset(current_page,0,MAX_PLAYLIST_SIZE);

        char request[255] = {0};
        char trailing_path[100] = {0};

        sprintf(trailing_path,"/sbin/tunein-station.pls?id=%s",path);

        unsigned long page_size = 0;

       // get_head(trailing_path); // TODO handle http header ... using Content-Length to allocate the buffer!

        if (net->client_connect("yp.shoutcast.com",80,TCP)) {

            sprintf(request,
                    "GET %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
                    "Accept: */*\r\n"
                    "Connection: Keep-Alive\r\n\r\n",
                    trailing_path,"yp.shoutcast.com");

            // get the page !
            if (net->client_send(request,strlen(request)))
            {
                int len = 1;
                while(len > 0) {
                    len = net->client_recv(current_page+page_size,1000);
                    if (len > 0) page_size += len;
#ifdef _WII_
                    if(len==-11) len = 1;//WOULDBLOCK
#else
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
#endif
                }

            }

            net->client_close();
        }
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
            }else{
                strcat(first_entry->path,"/");
                strcat(first_entry->path,split);
            }

            split = strtok(0,":/");
            part++;
        }
    }

    // TODO - build list of urls from the playlist .... only grabbing the first right now
    char* parse_playlist()
    {
        char url[1024] = {0};
        char* start = 0;
        char* end = 0;

        start = strstr(current_page,"File1=");
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
