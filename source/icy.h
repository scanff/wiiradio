#ifndef _ICY_H_
#define _ICY_H_

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
        char            icy_url[SMALL_MEM]; // url
        char            content_type[SMALL_MEM]; // type
        int             icy_pub; // public
        unsigned int    icy_metaint; // metaint .. very important
        int             icy_br; // bitrate
        char            track_title[SMALL_MEM]; // track info
        char            last_track_title[SMALL_MEM]; // track info - last

        // vars
        unsigned long   metaint_pos;
        bool            looking_for_header;
        unsigned long   buffered;
        unsigned long   pre_buffer; // buffer before playback ...
        unsigned long   buffer_size;
        char*           buffer;
        bool            bufferring;


        #define MAX_METADATA_SIZE   (4080) //16*255
        #define BUFFER_CHUNK        (5000)
        char            metaint_buffer[MAX_METADATA_SIZE];
        int             metaint_size;
        int             metaint_receive_pos;

        unsigned long   last_chunk;
        unsigned long   buffers_recvd;
        unsigned long   buffers_sent;

    public:

    icy() : icy_metaint(10000), // must not be zero on start, otherwise will never buffer
			metaint_pos(0),
            looking_for_header(true),
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

        buffer = new char[buffer_size];
        if(!buffer) exit(0);

        memset(buffer,0,buffer_size);
        memset(metaint_buffer,0,MAX_METADATA_SIZE);

        clean_icy_data();
    };

    ~icy()
    {
        delete [] buffer;
        buffer = 0;
    };

    // clean all the variables on new connect
    void clean_icy_data()
    {
        memset(icy_notice1,0,SMALL_MEM);
        memset(icy_notice2,0,SMALL_MEM);
        memset(icy_name,0,SMALL_MEM);
        memset(icy_genre,0,SMALL_MEM);
        memset(icy_url,0,SMALL_MEM);
        memset(track_title,0,SMALL_MEM);
        memset(content_type,0,SMALL_MEM);

        strcpy(last_track_title,"last");


        icy_pub = 0;
        icy_br = 0;

    }

    void icy_reset()
    {
        memset(buffer,0,buffer_size);
        memset(track_title,0,SMALL_MEM);
        strcpy(last_track_title,"last");
        metaint_pos = 0;
        looking_for_header = true;

        icy_stream_reset();
    };

    void icy_stream_reset()
    {
        buffers_sent = buffers_recvd = 0;
        buffered = 0;
        bufferring = true;
        last_chunk = 0;
        status = BUFFERING;
    };


    void parse_header_item(const char* obj)
    {
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

    int parse_header(char* buf,int len)
    {
        char* found = strstr(buf,"ICY 200 OK"); // expect this response from the server !
        if (!found) return 0;

        parse_header_item(buf);

        //find the end of the header then remove the header from the stream.
        char* end_header = strstr(buf,"\r\n\r\n");
        if (end_header)
        {
            return (end_header + 4) - buf;
        }

        return 0;
    };

    void parse_meta_data(char* data)
    {
        char* title_start = 0;
        char* title_end = 0;

        title_start = strstr(data,"StreamTitle='");
        if (title_start) {
            title_start += strlen("StreamTitle='");
            title_end = strstr(title_start,"';");
            if(title_end) {
                memset(track_title,0,SMALL_MEM);
                memcpy(track_title,title_start,(title_end-title_start)>SMALL_MEM-1 ? SMALL_MEM-1 : (title_end-title_start)); //127 for NULL term
                memset(metaint_buffer,0,MAX_METADATA_SIZE); // clear the holding metadata buffer

            }
        }
     };

    int parse_metaint(char* net_buffer,int len)
    {

        int netbuf_counter = 0;
        int new_len = len;

        loopi(len)
        {

            if (metaint_size != 0)
            {

                metaint_buffer[metaint_receive_pos++] = net_buffer[i];

                if (metaint_receive_pos == metaint_size)
                {
                    parse_meta_data(metaint_buffer); // parse the data
                    metaint_size = 0;
                }

                // remove from stream
                new_len--;
            }
            else
            {
                if (metaint_pos++ < icy_metaint) // only add data
                {
                     net_buffer[netbuf_counter++] = net_buffer[i];
                }
                else
                {
                    memset(metaint_buffer,0,MAX_METADATA_SIZE);
                    metaint_size = (unsigned int)((unsigned char)net_buffer[i]);
                    metaint_size > 0 ? metaint_size *= 16 : 0;
                    metaint_pos = 0;
                    metaint_receive_pos = 0;

                    if (metaint_size == 0)
                    { // remove from stream
                        new_len--;
                    }
                }
            }

        }

        return new_len;
    };

    void buffer_data(char* net_buffer,int len)
    {
        unsigned long new_buffer_size = buffered + len;

        if (new_buffer_size > buffer_size)
        {
            long remain = new_buffer_size - buffer_size;
            int end_bytes = buffer_size - buffered;

            // copy to the end of the buffer
            if (end_bytes > 0) memcpy(buffer+buffered,net_buffer,end_bytes);

            remain = len-end_bytes;

            // copy to the start of the buffer
            if (remain > 0 ) {
                memcpy(buffer,net_buffer+end_bytes,remain);
                buffered = remain;
            }else buffered = 0;


            buffers_recvd++; // total number of buffers we've gotten for this stream
        }else{
            // keep copying to the buffer
            memcpy(buffer+buffered,net_buffer,len);
            buffered += len;
        }

        /*  state on fist connected ... needed to parse out the header.
            512 is a number I selected to ensure that there is enough data in the buffer
            to check for a full header.
        */
        if (looking_for_header && buffered > 512)
        {
            int remove = parse_header(buffer,buffered);
            if (remove)
            {
                memcpy(buffer,buffer+remove,buffered+remove);
                buffered -= remove;
                metaint_pos-=remove;

                pre_buffer = ((icy_br * 8) * 1000) / 8;

                looking_for_header = false;

            }

        }

        if (buffered >= pre_buffer) bufferring = false; // got enough data, change the state to allow playback

    };

    // return a chunk of data
    int get_buffer(void* cbuf,int len)
    {
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

