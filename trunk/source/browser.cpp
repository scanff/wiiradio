#include "globals.h"
#include "application.h"
#include "browser.h"
#include "client.h"

#include "station_cache.h"

void browser::SetMax(const int m) {view_max = m; }
const int browser::GetPosition() { return position; }
void browser::MoveNext()
{
    if (position + view_max > total)
        return;

    position += view_max;
}

void browser::MoveBack()
{
    if (position - view_max <= 0)
    {
        position = 0;
        return;
    }

    position -= view_max;
}
const int browser::GetSize() { return total; }

browser::browser(app_wiiradio* _theapp) :  net(0), type(0), total(0), theapp(_theapp)
{
    net = new network(theapp);
    position = 0;
}

browser::~browser()
{

    delete net;
    net = 0;

    clear_list();
}


struct station_list* browser::new_sl_list()
{
    station_list* x = new station_list();
    if(!x) return 0;


    sl.push_back(x);
    return x;
}

void browser::clear_list()
{
    position = 0;

    for(u32 i = 0; i < sl.size(); i++)
    {
        if (sl[i])
        {
            delete sl[i];
            sl[i] = NULL;
        }
    }

    sl.clear();
    total = 0;
}

const char* browser::get_station(const int num)
{
    const u32 p = num + GetPosition();

    if(p >= 0 && p < sl.size())
        return sl[p]->station_name;

    return NULL;
}

struct station_list* browser::GetItem(const u32 n)
{
    const u32 p = n + GetPosition();

    if (p < 0 || p > sl.size())
        return NULL;

    return sl[p];
}



const int browser::connect(const char* path,char* genre,int itype = 0 /* SHOUTCAST DEFAULT */)
{
    type = itype;

    char lpath[255] = {0};
    char lgenre[255] = {0};
    char* connect_url = 0;

    sc_error = 1;
    SDL_mutexP(listviews_mutex);
    clear_list();
    SDL_mutexV(listviews_mutex);

    switch(type)
    {
        case SERVICE_SHOUTCAST:

            strcpy(lpath,path);
            strcpy(lgenre,genre);
            connect_url = (char*)NAVIX_DB_URL;

        break;
        case SERVICE_ICECAST:
            strcpy(lpath,"/yp.xml");
            // can't find a way to request genres with the backend XML ? HELP !
            sprintf(lgenre,"%s",genre);
            connect_url = (char*)ICECAST_DB_URL;
        break;
        default: //???
        break;
    }

    char* current_page = 0;
    current_page = new char[MAX_PAGE_SIZE];
    if(!current_page) return 0;

    memset(current_page,0,MAX_PAGE_SIZE);

    char tmp_buffer[3100];
    u32 page_size = 0;
    u32 size = MAX_PAGE_SIZE - 1;
    int tmp_size = 3050;
    int len = 1;

    if (net->client_connect(connect_url,80,TCP))
    {
        if(net->send_http_request((char*)"GET",(char*)lpath,connect_url))
        {


            u64 start_time = get_tick_count();

            while (len > 0)
            {
                if ((get_tick_count() - start_time) > TIME_OUT_MS)
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
                if(len==-11) len = 1; // BLOCK
                //if(WSAGetLastError()==WSAEWOULDBLOCK) len=1;
#endif
#ifdef _LINUX_
                if(net->client_connected) len = 1;
#endif

                usleep(800);
            }
        }

        // close the connection
        net->client_close();


        // validate that we got good data!!!!!
        if (page_size == 0 )
        {
                if(current_page)
                {
                    delete [] current_page;
                    current_page = 0;
                }

                return 0;
        }
/*
HUMMMM this is causing a really bad crash on WII right now ...
so I'm just removing till I can figure out why


            // bad !!! .... ShoutCast.com DB is busy or down ... look for a cache if we have one
            if (!load_cache(lgenre,current_page))
            {
                // nothing more we can do ... try again later :(
                sc_error = true;

                if(current_page)
                {
                    delete [] current_page;
                    current_page = 0;
                }

                return 0;
            }
        }else{
            // save a cache of the latest
            save_cache(current_page,page_size,lgenre);
        }
*/
    }else{
        if(current_page)
        {
            delete [] current_page;
            current_page = 0;
        }

        return 0;
    }

    parse_page(current_page,lgenre,page_size);

    if(current_page)
    {
        delete [] current_page;
        current_page = 0;
    }

    return 1;

}

inline void browser::strip_quotes(char* in)
{
    int len = strlen(in);
    if (len <=0) return;

    char* p_in = in;

    loopi(len)
    {
        if(in[i] != '"')
        {
            *p_in++ = in[i];
        }
    }
    *p_in = '\0';
}

char* browser::get_attrib_navix(char* data,const char* attrib,char* attib_buffer,int max)
{
    char* attrib_start = 0;
    char* attrib_end = 0;

    attrib_start = strstr(data,attrib);
    if (!attrib_start) return 0;

    attrib_start += strlen(attrib);

    attrib_end = strstr(attrib_start,"\r");
    if(!attrib_end) attrib_end = strstr(attrib_start,"\n");
    if (!attrib_end) return 0;

    int cpy_len = attrib_end-attrib_start;
    if (cpy_len > max)
    {
        cpy_len = max;
        memcpy(attib_buffer,attrib_start,max);
        attib_buffer[cpy_len] = '\0';
    }
    else if (cpy_len==0)
    {
        strcpy(attib_buffer,"Unknown");
    }
    else
    {
        memcpy(attib_buffer,attrib_start,cpy_len);
        attib_buffer[cpy_len] = '\0';
    }

    strip_quotes(attib_buffer);



    return attrib_end;

}

char* browser::get_element(char* data,const char* element,char* element_buffer,int max)
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
    if (cpy_len > max)
    {
        cpy_len = max;
        memcpy(element_buffer,element_start,max);
    }
    else if (cpy_len==0)
    {
        strcpy(element_buffer,"Unknown");
    }
    else memcpy(element_buffer,element_start,cpy_len);

    element_buffer[cpy_len] = '\0';

    return element_end;

}

void browser::get_stationid(const char* url,char* stationid)
{
    if (!url) return;

    char* sid = (char*)strstr(url,"id=");
    if (sid)
    {
        sid += 3;
        char* eid = strstr(sid,"&");
        if (!eid) return;

        const int len = eid - sid;
        if (len > 0 && len < MED_MEM-1)
        {
            strncpy(stationid,sid,len);
            stationid[len] = '\0';


            return;
        }
    }
    return;

}


/*
    genre_filter

    ICEcast does not provide an api that I can find to search for genres.
    We will have to filter the whole list for the genre we've searched for.
*/
void browser::parse_page(char* data,char* genre_filter,int page_size)
{

#ifdef SCANFF_DUMP
    char t[255];
    sprintf(t,"c:\\%s.txt",genre_filter);
    FILE* f = fopen(t,"wb");
    fwrite(data,1,page_size,f);
    fclose(f);
#endif
    char* stream_pos = data;
    char codec_tmp[SMALL_MEM];
    char name_tmp[MED_MEM];
    char genres_tmp[SMALL_MEM];

    sc_error = false; // set to false as we have a list of stations

    u64 start_time = get_tick_count();

    SDL_mutexP(listviews_mutex);

    if (type == SERVICE_SHOUTCAST)
    { // SHOUTcast

        while(((get_tick_count() - start_time) < TIME_OUT_MS))
        {
            memset(codec_tmp,0,SMALL_MEM);
            memset(name_tmp,0,MED_MEM);

            stream_pos = strstr(stream_pos,"#"); //class name we are looking for
            if (stream_pos)
            {
                stream_pos = get_attrib_navix(stream_pos,"name=",name_tmp,MED_MEM-1); //-1 for null terminator
                if(!stream_pos)
                    break;

                station_list* sl_new = new_sl_list();
                if (!sl_new)
                    break;

                total++; //running count for validity check

                strcpy(sl_new->station_name,name_tmp);

                stream_pos = get_attrib_navix(stream_pos,"URL=",sl_new->station_playing,MED_MEM-1);//-1 for null terminator

                get_stationid(sl_new->station_playing,sl_new->station_id);

                // just set to zero right now, when more codecs are supported we can do something with this
                sl_new->codec_type = 0;
                sl_new->service_type = SERVICE_SHOUTCAST; // shoutcast ... TODO enum's

                if(!stream_pos)
                    break;

                stream_pos = strstr(stream_pos,"#");

            }

            if (!stream_pos)
                break;

            usleep(100);

        }

        SDL_mutexV(listviews_mutex);
    }
    else if (type == SERVICE_ICECAST)
    { // ICECast

        SDL_mutexP(listviews_mutex);

        while(((get_tick_count() - start_time) < TIME_OUT_MS))
        {
            if(!stream_pos)
                break;

            memset(codec_tmp,0,SMALL_MEM);
            memset(name_tmp,0,MED_MEM);

            stream_pos = strstr(stream_pos,"<entry>"); //class name we are looking for
            if (stream_pos)
            {
                stream_pos = get_element(stream_pos,"server_name",name_tmp,MED_MEM-1); //-1 for null terminator
                if(!stream_pos)
                    break;

                get_element(stream_pos,"server_type",codec_tmp,SMALL_MEM-1);

                if (strstr(codec_tmp,"audio/mpeg") == 0)
                {
                    stream_pos = strstr(stream_pos,"</entry>");

                    continue;
                }


                get_element(stream_pos,"genre",genres_tmp,SMALL_MEM-1);

                string_lower(genres_tmp); // format string from XML file

                if (!strstr(genres_tmp,genre_filter))
                {
                    stream_pos = strstr(stream_pos,"</entry>");

                    continue;
                }

                station_list* sl_new = new_sl_list();
                if (!sl_new)
                    break;

                total++; //running count for validity check

                strcpy(sl_new->station_name,name_tmp);
                get_element(stream_pos,"listen_url",sl_new->station_id,MED_MEM-1);//-1 for null terminator

                // just set to zero right now, when more codecs are supported we can do something with this
                sl_new->codec_type = 0;
                sl_new->service_type = SERVICE_ICECAST;

                stream_pos = strstr(stream_pos,"</entry>");

                if(!stream_pos)
                    break;
            }

            usleep(100);
        }

        SDL_mutexV(listviews_mutex);

    }
}

void browser::string_lower(char* string)
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

}
