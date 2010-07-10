#ifndef _ICY_H_
#define _ICY_H_

#ifdef ICY_DEBUG
#define DEB(x) { printf(x); }
#else
#define DEB(x) {}
#endif

// ICY header data fields
#define ICY_META_MAX (9)
char icy_meta[ICY_META_MAX][20] = {     "icy-notice1:",
                                        "icy-notice2:",
                                        "icy-name:",
                                        "icy-genre:",
                                        "icy-url:",
                                        "content-type:",
                                        "icy-pub:",
                                        "icy-metaint:",
                                        "icy-br:"
                                    };

class icy {
    public:

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


        #define MAX_METADATA_SIZE   (4080+100) //16*255 + padding
        #define BUFFER_CHUNK        (5000)
        char            metaint_buffer[MAX_METADATA_SIZE];
        int             metaint_size;
        int             metaint_receive_pos;

        unsigned long   last_chunk;
        unsigned long   buffers_recvd;
        unsigned long   buffers_sent;

        char*           temp_buffer;

    public:

    icy() : icy_metaint(0),
            metaint_pos(0),
            looking_for_header(true),
            redirect_detected(0),
            buffered(0),
            pre_buffer(200000),
            buffer_size(2000000),
            buffer(0),
            bufferring(true),
            metaint_size(0),
            metaint_receive_pos(0),
            last_chunk(0),
            buffers_recvd(0),
            buffers_sent(0)

    {
        memset(track_title,0,SMALL_MEM);
        strcpy(last_track_title,"last");

        buffer = (char*)memalign(32,buffer_size);//new char[buffer_size];
        if(!buffer) exit(0);

        temp_buffer = (char*)memalign(32,MAX_NET_BUFFER);

        memset(buffer,0,buffer_size);
        memset(metaint_buffer,0,MAX_METADATA_SIZE);


        clean_icy_data();
    };


    ~icy()
    {
        free(temp_buffer);
        free(buffer);
    };

    // clean all the variables on new connect
    void clean_icy_data()
    {
        DEB("clean_icy_data\n");
        memset(icy_notice1,0,SMALL_MEM);
        memset(icy_notice2,0,SMALL_MEM);
        memset(icy_name,0,SMALL_MEM);
        memset(icy_genre,0,SMALL_MEM);
        memset(icy_url,0,SMALL_MEM);
        memset(icy_pls_url,0,SMALL_MEM);
        memset(content_type,0,SMALL_MEM);

        memset(track_title,0,SMALL_MEM);
        strcpy(last_track_title,"last");

        icy_pub = 0;
        icy_br = 0;

    }

    void icy_reset()
    {
        DEB("icy_reset\n");
        memset(buffer,0,buffer_size);
        icy_stream_reset();
        icy_metaint=10000; // some default, hsa to be overwritten
        metaint_pos = 0;
        metaint_receive_pos = 0;
        pre_buffer = 200000;
        looking_for_header = true;
        redirect_detected = 0;

        clean_icy_data();
    };

    void icy_stream_reset()
    {
        DEB("icy_stream_reset\n");
        buffers_sent = buffers_recvd = 0;
        buffered = 0;
        bufferring = true;
        last_chunk = 0;
        status = BUFFERING;
    };


    void parse_header_item(const char* obj)
    {
        DEB("parse_header_item\n");
        char* start = 0;
        char* end = 0;
        char tmp[10] = {0};

        clean_icy_data();

        loopi(ICY_META_MAX)
        {
            start =  strstr(obj,icy_meta[i]);
            if (start) {
                // find the end of this meta data
                end = strstr(start,"\r\n");
                if (end) {
                    start += strlen(icy_meta[i]);
                    switch(i){
                        case 0: memcpy(icy_notice1,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 1: memcpy(icy_notice2,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 2: memcpy(icy_name,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 3: memcpy(icy_genre,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 4: memcpy(icy_url,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 5: memcpy(content_type,start,(end-start < SMALL_MEM-1 ? end-start : SMALL_MEM-1)); break;
                        case 6:
                            memset(tmp,0,10);
                            memcpy(tmp,start,(end-start < 10 ? end-start : 9));
                            icy_pub = atoi(tmp);

                         break;
                        case 7:
                            memset(tmp,0,10);
                            memcpy(tmp,start,(end-start < 10 ? end-start : 9));
                            icy_metaint = atoi(tmp);
#ifdef ICY_DEBUG
                            printf("parsed icy-metaint: %d\n", icy_metaint);
#endif

                        break;
                        case 8:
                            memset(tmp,0,10);
                            memcpy(tmp,start,(end-start < 10 ? end-start : 9));
                            icy_br = atoi(tmp);
                        break;


                    }

                }
            }
        }

    };

    void parse_http_redirect(char *buf)
    {
        DEB("HTTP redirect\n");
        char *found = strstr(buf,"Location: ");
        if (!found)
        {
            DEB("  redirect target not found\n");
            return;
        }
        found += strlen("Location: ");
        char *end = strstr(found,"\r\n");
        memcpy(icy_url,found,(end-found < SMALL_MEM-1 ? end-found : SMALL_MEM-1));
        redirect_detected++;
#ifdef ICY_DEBUG
        printf("redirect: %s\n", icy_url);
#endif
    }

    // return value: number of char* to remove from buffer. negative: error
    int parse_header(char* buf)
    {
        DEB("parse_header\n");

        char* found = strstr(buf,"icy-metaint:"); // expect this response from the server !
        if (!found)
        {
          // This does not look like a icy stream, but it still could be
          // something else we can handle, e.g. an html redirect
          DEB("  icy header not found\n");
          found = strstr(buf," 302 Moved Temporarily\r\n");
          if (found)
              parse_http_redirect(buf);
#ifdef ICY_DEBUG
          printf("%s\n", buf);
#endif
          return -1;
        }
        DEB("  found\n");

        parse_header_item(buf);

        //find the end of the header then remove the header from the stream.
        char* end_header = strstr(buf,"\r\n\r\n");
        if (!end_header)
            end_header = strstr(buffer,"\r\n\r\0");
        if (end_header)
        {
            return (end_header + 4) - buf;
        }
#ifdef ICY_DEBUG
        else
        {
            printf("End of header not found\n");
            printf("%s\n", buf);
        }
#endif
        return -1;
    };

    void parse_meta_data(char* data)
    {
        char* title_start = 0;
        char* title_end = 0;

        DEB("parse_meta_data\n");
#ifdef ICY_DEBUG
        printf("%s\n", data);
#endif
        title_start = strstr(data,"StreamTitle='");
        if (title_start) {
            title_start += strlen("StreamTitle='");
            title_end = strstr(title_start,"';");
            if (title_end) {
                int sizeof_tile = title_end-title_start;
                if (sizeof_tile >= SMALL_MEM) sizeof_tile = SMALL_MEM;

                // If an empty title is sent, use the station name as title
                memset(track_title,0,SMALL_MEM);

                if (title_start == title_end)
                {
                    strcpy(track_title, "Unknown Track");
                }else{
                    strncpy(track_title, title_start, sizeof_tile);
                }
                // Make sure the title string is complete
                track_title[SMALL_MEM-1] = '\0';
                // clear the holding metadata buffer
                memset(metaint_buffer,0,MAX_METADATA_SIZE);
            }
        }
     };

    // Parses received net-buffer for meta information and removes it from
    // stream, returning new buffer length
    int parse_metaint(char* net_buffer,int len)
    {
        // Position in real data stream
        int netbuf_counter = 0;
        int new_len = len;

        loopi(len)
        {
            // Check if we are inside a metaint section
            if (metaint_size != 0)
            {
                // Copy current data to metaint buffer
                metaint_buffer[metaint_receive_pos++] = net_buffer[i];

                // If we reached the end of the metaint section
                if (metaint_receive_pos == metaint_size)
                {
#ifdef ICY_DEBUG
                    if (net_buffer[i] != '\0')
                    {
                      printf("Error in metaint section: no padding at end, metaint; %d\n",
                             icy_metaint);
                      exit(1);
                    }
#endif
                    // Parse the metaint section
                    parse_meta_data(metaint_buffer); // parse the data
                    // Return to data stream handling
                    metaint_size = 0;
                }

                // remove current metaint data from stream
                new_len--;
            }
            // We are inside a data section
            else
            {
                // Check for end of data section
                if (metaint_pos++ < icy_metaint)
                {
                     // Not yet reached: shift data in buffer if necessary
                     // and go on
                     net_buffer[netbuf_counter++] = net_buffer[i];
                }
                else
                {
                    // End of data section reached, reset metaint buffers
                    memset(metaint_buffer,0,MAX_METADATA_SIZE);
                    metaint_size = 16 * (unsigned int)((unsigned char)net_buffer[i]);

#ifdef ICY_DEBUG
                    printf("Metaint_size: %d\n", metaint_size);
#endif

                    metaint_pos = 0;
                    metaint_receive_pos = 0;

                    // If there is no data to parse, remove that zero and
                    // continue
                    if (metaint_size == 0)
                    {
                        new_len--;
                    }
                }
            }
        }
#ifdef ICY_DEBUG
        printf("parse_metaint len, new_len (diff), metaint: %d %d (%d) %d\n", len, new_len, len-new_len, icy_metaint);
#endif

        return new_len;
    };

    void buffer_data(char* net_buffer,int len)
    {
        /*
            Check for header first
        */

        memset(temp_buffer,0,MAX_NET_BUFFER); // clear the old data
        memcpy(temp_buffer,net_buffer,len); // copy

        if(looking_for_header)
        {
            loopi(len) buffer[buffered++] = net_buffer[i];

            if (buffered > 20000) // We should have found the header by now!!!
            {
                status = FAILED;
                return;
            }

            // overwrite last byte of buffer temporarily to protect strstr
            char tmp = buffer[buffered-1];
            buffer[buffered-1] = '\0';
            if (strstr(buffer,"\r\n\r\n") || strstr(buffer,"\r\n\r\0"))
            {
                DEB("looking_for_header\n");
                int remove = parse_header(buffer);

                if (remove < 0) // stream not available! could be old link
                {
                    status = FAILED;
                    return;
                }

                buffer[buffered-1] = tmp;
                if (remove)
                {
                    memcpy(temp_buffer, buffer+remove, buffered-remove);
                    buffered -= remove;
                    metaint_pos -= remove;

                    pre_buffer = ((icy_br * 8) * 1000) / 8;
                    // Protect against too less or too high buffer values
                    if (pre_buffer < 64000)
                    {
                        pre_buffer = 64000;
                    }
                    else if (pre_buffer > 512000)
                    {
                        pre_buffer = 512000;
                    }
#ifdef ICY_DEBUG
                    printf("pre_buffer: %d\n", (int)pre_buffer);
#endif
                }

                looking_for_header = false;

                /*
                    Reset lens .. we need tp parse out metaint data from this buffer
                */
                len = buffered;
                metaint_pos = buffered = 0;

            }
        }

        // Still looking for header ... return if we are
        if (looking_for_header) return;

        // Got header start looking for meta-int
        len = parse_metaint(temp_buffer,len);

        // Get the audio data
        unsigned long new_buffer_size = buffered + len;

        if (new_buffer_size > buffer_size)
        {
            int end_bytes = buffer_size - buffered;

            // copy to the end of the buffer
            if (end_bytes > 0) memcpy(buffer+buffered,temp_buffer,end_bytes);

            long remain = len-end_bytes;

            // copy to the start of the buffer
            if (remain > 0 )
            {
                memcpy(buffer,temp_buffer+end_bytes,remain);
                buffered = remain;

            } else buffered = 0;


            buffers_recvd++; // total number of buffers we've gotten for this stream
        } else {
            // keep copying to the buffer
            memcpy(buffer+buffered,temp_buffer,len);
            buffered += len;
        }

        if (buffered >= pre_buffer) bufferring = false; // got enough data, change the state to allow playback

    };

    // return a chunk of data
    int get_buffer(void* cbuf,int len)
    {
        if (len <= 0) return 0;
        if (len > BUFFER_CHUNK) len = BUFFER_CHUNK;

        //need to restart
        if ((last_chunk + len) > buffer_size)
        {
           len = (buffer_size - last_chunk); // send whats left

            if (len <= 0)
            {
                buffers_sent++;
                last_chunk = 0;
                return 0; // have to reset the stream :(
            }

            memcpy(cbuf,buffer+last_chunk,len); // copy remaining

            // update and reset
            buffers_sent++;
            last_chunk = 0;

            return len;

        }
        else if ((last_chunk + len) > buffered)
        {
            if (buffers_recvd == buffers_sent)
            {
                // reset ... need to rebuffer !!!!
                // check your signal strength :P
                icy_stream_reset();

                return 0;
            }


        }

        if (len > 0) {
            memcpy(cbuf,buffer+last_chunk,len);
            last_chunk += len;
        }


        return len;

    };

};

#endif //_ICY_H_

