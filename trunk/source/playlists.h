
// a playlist can contain several streams.  Store these incase a stream is full
struct station_playlist
{
    station_playlist() { reset(); }

    void reset()
    {
        port = 0;
        memset(url,0,MED_MEM);
        memset(path,0,SMALL_MEM);
    }

    int port;
    char url[MED_MEM]; // url
    char path[MED_MEM];

};  // TODO - build list of urls from the playlist .... only grabbing the first right now -----

class playlists
{
    public:

    #define MAX_PLAYLIST_SIZE (5000)

    playlists(app_wiiradio* _theapp) : theapp(_theapp)
    {
    }

    ~playlists()
    {
    }

    void GetEntry(station* playing)
    {


    }

    station_playlist    first_entry;
    app_wiiradio*       theapp;

public:

    #define SHOUTCAST_PLS_URL ("http://yp.shoutcast.com")
    #define SHOUTCAST_PLS_URL_SHORT ("yp.shoutcast.com")
    int get_playlist(const char* path)
    {
        int ret = -1;
        network* net = 0;
        char* current_page = 0;

        const char* trailing_path = strstr(path,SHOUTCAST_PLS_URL);
        if(!trailing_path)
             return -1000;

        trailing_path += strlen(SHOUTCAST_PLS_URL);


        net = new network(theapp);
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

        char tmp_buffer[3100];
        u32 page_size = 0;
        u32 size = MAX_PLAYLIST_SIZE - 1;
        int tmp_size = 3050;

        if (net->client_connect(SHOUTCAST_PLS_URL_SHORT,80,TCP)) {

            // get the page !
            if(net->send_http_request("GET",(char*)trailing_path,SHOUTCAST_PLS_URL_SHORT))
            {
                int len = 1;
                u64 start_time = get_tick_count();

                while (len > 0)
                {
                    if ((get_tick_count() - start_time) > TIME_OUT_MS )
                    {
                        break; //timeout
                    }
                    len = net->client_recv(tmp_buffer,tmp_size);

                    if (len > 0)
                    {
                        if (page_size+len >= size)
                            break;

                        memcpy(current_page+page_size,tmp_buffer,len);
                        page_size += len;

                    }
#ifdef _WII_
                    if(len==-11) len = 1; // BLOCK
#endif
#ifdef _WIN32
                    //if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
                    if(len==-11) len = 1; // BLOCK
#endif
#ifdef _LINUX_
                    if(net->client_connected) len = 1;
#endif

                    usleep(800);
                }

            }

            net->client_close();
        }
        else ret = -1003;

        if(page_size > 0)
            ret = parse_playlist(current_page);

        delete [] current_page;
        current_page = 0;

        return ret;
    };


    void split_url(char* url)
    {
        int part = 0;
        char* split = url;
         // ignore http://
        char* search = strstr(url,"http://");
        if(!search) return;

        search += strlen("http://");

        split = strtok(search,":/");
        while(split) {

            if(!part) {
                strcpy(first_entry.url,split);
            }else if(part==1) {
                first_entry.port = atoi(split);
                if (first_entry.port == 0) {
                    strcat(first_entry.path,"/");
                    strcat(first_entry.path,split);
                }
            }else{
                strcat(first_entry.path,"/");
                strcat(first_entry.path,split);
            }

            split = strtok(0,":/");
            part++;
        }
    }

    // TODO - build list of urls from the playlist .... only grabbing the first right now
    int parse_playlist(const char* cp)
    {
        first_entry.reset();

#ifdef SCANFF_DUMP
        FILE *f = fopen("c:\\pls.txt","wb");
        fwrite(cp,strlen(cp),1,f);
        fclose(f);
#endif
        char url[MED_MEM+1];
        char* start = 0;
        char* end = 0;

        start = strstr(cp,"File1=");
        if (start)
        {
            start += strlen("File1=");
            end = strstr(start,"\n");
            if(!end) return -1;

            int c_len = end - start;
            if(c_len > 0 && c_len < MED_MEM)
            {
                strncpy(url,start,c_len);
                url[c_len] = '\0';

                split_url(url);
            }
        }

        return 1;
    }

};
