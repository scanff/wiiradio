#ifndef COVERS_H_INCLUDES
#define COVERS_H_INCLUDES


class covers {
    public:

    #define DB_URL  ("www.freecovers.net")

    network*        net;
    char            current_img_url[MED_MEM];
    SDL_Surface*    cover_surf;
    SDL_Thread      *searchthread;
    char*           search_string;
    covers() : cover_surf(0), searchthread(0),search_string(0)
    {
      net = new network;
      memset(current_img_url,0,MED_MEM);
    };

    ~covers()
    {
        if (cover_surf) SDL_FreeSurface(cover_surf);

        if (searchthread) SDL_WaitThread(searchthread, 0);

        delete net;
        net = 0;
    };


    static int cv_thread(void *arg)
    {
        covers* x = (covers*)arg;
        x->run();

        return 0;
    };

    void start_cover_search(char* ss)
    {
        search_string = ss;
        searchthread = 0;
        searchthread = SDL_CreateThread(cv_thread,(void*)this);
    };

    void run()
    {
        search_cover(search_string);
    }

    void get_image_url(char* buf)
    {
       /*   should return XML ...
            I'm not using XML but I'm always looking for the first image as this is a decent match and not too big

        */

        char* start =0;
        char* end = 0;
        int len = 0;
        start = strstr(buf,"<image>");
        if (start)
        {
            end = strstr(start,"</image>");
            if (end)
            {
                start += strlen("<image>");
                start = strstr(start,DB_URL);
                if (!start) return;

                start += strlen(DB_URL);

                len = end - start;
                if (len > 0 && len < MED_MEM)
                {
                    memcpy(current_img_url,start,len);
                }
            }

        }

    };

    void load_cover_surface()
    {
        if (cover_surf) SDL_FreeSurface(cover_surf); // free if already in use

        cover_surf = IMG_Load(make_path("cache/aa.jpg"));


    }

    int remove_header(char* buf)
    {
        char* end = strstr(buf,"\r\n\r\n");
        end += 4;

        return end - buf;
    }
    void get_cover()
    {

        if (net->client_connect((char*)DB_URL,80,TCP))
        {
            if (net->send_http_request((char*)"GET",(char*)current_img_url,(char*)DB_URL))
            {

                char* buffer = 0;
                buffer = new char[200000]; // guess max size .. should really use Content-Length
                if (!buffer) return;

                FILE* f = fopen(make_path("cache/aa.jpg"),"wb");

                unsigned long page_size = 0;
                int len = 1;

                unsigned long start_time = get_tick_count();
                bool in_header = true;

                //ok ... let real in the head
                while(len > 0) {

                    if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                    len = net->client_recv(buffer+page_size,512);
                    if (len > 0) page_size += len;
    #ifdef _WII_
                    if(len==-11) len =1;//WOULDBLOCK
    #else
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
    #endif
                    if (page_size > 512 && in_header)
                    {
                        in_header = false;
                        int remove = remove_header(buffer);
                        memmove(buffer,buffer+remove,page_size-remove);
                        page_size -= remove;

                    }
                }

                fwrite(buffer,page_size,1, f);
                fclose(f);

                delete [] buffer;
                buffer = 0;
            }

            net->client_close();
        }

        load_cover_surface();

    };

    char* clean_search(char* current)
    {
        int len = strlen(current);
        static char new_str[MED_MEM] = {0};

        // replace the - seperator with a +
        loopi(len)
        {
            if (i>=MED_MEM-1) break;

            new_str[i] = current[i];
            if (new_str[i] == '-')
            {
                new_str[i] = '+';
                continue;
            }
        }

        return new_str;
    };

    void search_cover(char* search_str)
    {
        char start[] = "/api/search/";
        char trail[] = "/Music+CD";
        char trailing_path[1024] = {0};

        // clear the curent image url
        memset(current_img_url,0,MED_MEM);

        // create request
        sprintf(trailing_path,"%s%s%s",start,clean_search(search_str),trail);

        if (net->client_connect((char*)DB_URL,80,TCP))
        {
            // get the page
            if (net->send_http_request((char*)"GET",(char*)trailing_path,(char*)DB_URL))
            {
                char* buffer = 0;
                buffer = new char[200000]; // guess max size .. freecovers does not seem to return Content-Length
                if (!buffer) return;

                //FILE* f = fopen("c://1.txt","w");


                unsigned long page_size = 0;
                int len = 1;

                unsigned long start_time = get_tick_count();

                //ok ... let real in the head
                while(len > 0) {

                    if ((get_tick_count() - start_time) > TIME_OUT_MS) break; //timeout

                    len = net->client_recv(buffer+page_size,1024);
                    if (len > 0) page_size += len;
    #ifdef _WII_
                    if(len==-11) len =1;//WOULDBLOCK
    #else
                    if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
    #endif

                }

             //   fwrite(buffer,page_size,1, f);
             //   fclose(f);

                get_image_url(buffer);

                delete [] buffer;
                buffer = 0;

            }

            net->client_close();
        }

        if (strlen(current_img_url) > 0)
            get_cover();

    };


};

#endif // COVERS_H_INCLUDES
