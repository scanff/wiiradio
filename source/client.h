#include "globals.h"
#include <errno.h>
#define MAX_INIT_RETRIES 20

enum _net_protocols {
    TCP = 0,
    UDP
};



class dns
{
    public:


    char* getipbyname(char *domain)
    {
        struct hostent *host = 0;
#ifdef _WII_
        host = net_gethostbyname(domain);
#else
        host = gethostbyname(domain);
#endif
        if(host == NULL) {
            return 0;
        }

        struct sockaddr_in tmp;
#ifdef _WII_
        memcpy(&tmp.sin_addr,host->h_addr_list[0],host->h_length);
#else
        memcpy(&tmp.sin_addr,host->h_addr,host->h_length);
#endif

        return inet_ntoa(tmp.sin_addr);
    };

};

class network : public dns
{
    public:

    network()
    {
        client_connected = false;
        mode = -1;


        #ifdef _WII_
        int ret = 0;
        for(int i=0;i<MAX_INIT_RETRIES && (ret=net_init())==-EAGAIN;i++);
              if(ret >= 0)
              {
                  if (if_config ( localip, netmask, gateway, TRUE) < 0)
                  {
                      fnts->text(screen,"if_config() failed.",30,150,0);SDL_Flip(screen);
                      return;
                  }
              }else{
                  fnts->text(screen,"net_init() failed.",30,150,0);SDL_Flip(screen);
                  return;
              }

        #else
        net_init();
        #endif

    }

    ~network()
    {
       if (client_connected) client_close();

    }
    //vars
    bool            client_connected;
    s32             connection_socket; //client
    char            localip[16];
    char            gateway[16];
    char            netmask[16];
    struct          sockaddr_in client;
    unsigned int    clientlen;
    int             c_protocol;
    int             mode;

#ifdef _WIN32



    /* wrappers for functions on windows */

    int net_init()
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        wVersionRequested = MAKEWORD(2, 0);
        if(WSAStartup(wVersionRequested, &wsaData) != 0) return 0;

        return 1;
    }

    int net_bind(s32 s, struct sockaddr * sa, int size)
    {
        return bind(s,( struct sockaddr*) sa, size);
    }

    int net_listen(s32 s, int a)
    {
        return listen(s, a);
    }

    s32 net_accept (s32 s, struct sockaddr * c, u32* len)
    {
        return accept (s, (struct sockaddr *) c, len);
    }

    s32 net_socket(int domain,int type,int protocol)
    {
        return socket(domain, type, protocol);
    }

    s32 net_connect(s32 s,sockaddr * addy, int socklen_t)
    {
       return connect(s,addy,socklen_t);
    }

    int net_send(s32 s, char *buffer,int len,int flags)
    {
        return send(s, buffer, len, flags);
    }

    int net_recv(s32 s, char* buffer, int len, int flags)
    {
        return recv(s, buffer, len, flags);
    }

    int net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len)
    {
        return sendto(s,buffer,len,flags,addy,(int)addy_len);
    }

    int net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len)
    {
        return recvfrom(s,buffer,len,flags,addy,(int*)addy_len);
    }

    void net_ioctl(s32 s, int b, unsigned long* c)
    {
        ioctlsocket( s, b, c ); //non blocking
    }

    void net_close(s32 s)
    {
        closesocket(s);
    }

#endif

#ifdef _LINUX_

    /* wrappers for functions on windows */

    int net_init()
    {
        return 0;
    }

    int net_bind(s32 s, struct sockaddr * sa, int size)
    {
        return bind(s,( struct sockaddr*) sa, size);
    }

    int net_listen(s32 s, int a)
    {
        return listen(s, a);
    }

    s32 net_accept (s32 s, struct sockaddr * c, u32* len)
    {
        return accept (s, (struct sockaddr *) c, len);
    }

    s32 net_socket(int domain,int type,int protocol)
    {
        return socket(domain, type, protocol);
    }

    s32 net_connect(s32 s,sockaddr * addy, int socklen_t)
    {
       return connect(s,addy,socklen_t);
    }

    int net_send(s32 s, char *buffer,int len,int flags)
    {
        return send(s, buffer, len, flags);
    }

    int net_recv(s32 s, char* buffer, int len, int flags)
    {
        return recv(s, buffer, len, flags);
    }

    int net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len)
    {
        return sendto(s,buffer,len,flags,addy,(int)addy_len);
    }

    int net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len)
    {
        return recvfrom(s,buffer,len,flags,addy,addy_len);
    }

    void net_close(s32 s)
    {
    }

#endif


    int client_connect(char* ip,int port, int protocol, bool block = false)
    {

        char* ip_checked = 0;
        c_protocol = protocol;

        ip_checked = address_to_ip(ip);
        if (!ip_checked) return 0;

        client_connected = false;
        connection_socket = net_socket(AF_INET, protocol == TCP ? SOCK_STREAM : SOCK_DGRAM,IPPROTO_IP);
        if (connection_socket < 0) return 0;

        memset(&client,0, sizeof(client));

#ifdef _WIN32
        client.sin_addr.S_un.S_addr = inet_addr(ip_checked);
#endif
#ifdef _WII_
        client.sin_addr.s_addr = inet_addr(ip_checked);
#endif
#ifdef _LINUX_
        client.sin_addr.s_addr = inet_addr(ip_checked);
#endif

        client.sin_family = AF_INET;
        client.sin_port = htons(port);

        // set this before so net_connect does not block
        //if (!block) -- don't bother .. never want blocking anyway!
        {
#ifdef _WII_

            s32 res = net_fcntl (connection_socket, F_GETFL, 0);
            res = net_fcntl (connection_socket, F_SETFL, res | 4);

#endif

        }

        if(protocol == TCP)
        {
//#ifdef _WII_
#ifndef _WIN32
            unsigned long start_time = get_tick_count();

            while(1)
            {
                if ((get_tick_count() - start_time) > 5000) return 0; // 5 sec timeout

                int res = net_connect(connection_socket,(struct sockaddr*)&client,sizeof(client));

                if (res < 0)
                {
                    if (res == -EISCONN)
                    {
                        break;
                    }

                    if (res == -EINPROGRESS || res == -EALREADY)
                    {
                        usleep (20 * 1000);
                        continue;
                    }


                    net_close(connection_socket);
                    connection_socket = 0;
                    return 0;

                }else break;

            }

#else

    if ((net_connect(connection_socket,(struct sockaddr*)&client,sizeof(client)) < 0))
    {
        net_close(connection_socket);
        connection_socket = 0;
        return 0;
    }

    unsigned long flag = O_NONBLOCK;
    net_ioctl(connection_socket, FIONBIO, &flag); // does not work on WII

#endif

    }
        client_connected = true;

        return 1;
    }

    int client_send(void* buffer,int len)
    {
        if (!client_connected) return 0;

        u32 clen = sizeof(client);
        return c_protocol == TCP ?   net_send(connection_socket, (char*)buffer, len, 0) :
                                     net_sendto(connection_socket, (char*)buffer, len, 0,(struct sockaddr*)&client,clen);
    }

    int client_recv(char* buffer,int len)
    {
        int ret;
        if (!client_connected) return 0;
        // Check if there is data to read before trying to read,
        // otherwise we cannot detect closed sockets
#ifdef HAVE_WII_SELECT
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(connection_socket, &rfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        #ifdef _WII_
          ret = net_select(connection_socket+1, &rfds, NULL, NULL, &tv);
        #else
          ret = select(connection_socket+1, &rfds, NULL, NULL, &tv);
        #endif
#else
        #ifdef _WII_
          pollsd fds;
          fds.socket = connection_socket;
          fds.events = POLLIN | POLLPRI;
          ret = net_poll(&fds, 1, 1);
        #else
            #ifdef _WIN32
                ret = 1;
            #else
              pollfd fds;
              fds.fd = connection_socket;
              fds.events = POLLIN | POLLPRI;
              ret = poll(&fds, 1, 1);
            #endif
        #endif
#endif
        // If there is no data to read, don't try
        if (!ret) return -EAGAIN;
        // TODO: We probably should do some error handling here
        if (ret < 0) return ret;

        u32 clen = sizeof(client);
        if (c_protocol == TCP)
            ret = net_recv(connection_socket, buffer, len, 0);
        else
            ret = net_recvfrom(connection_socket, (char*)buffer, len, 0,
                               (struct sockaddr*)&client,&clen);
        // If select() returned >0 but recv() returned 0, we got EOF
        // and the socket has to be regarded as closed from now on
#ifndef _WIN32
        if (ret == 0)
        {
            client_close();
            client_connected = 0;
        }
#endif
        return ret;
    }

    void client_close()
    {
        net_close(connection_socket);
    }

    void client_restart()
    {
#ifdef _WII_
        //net_close(connection_socket);
//        net_shutdown(connection_socket,SHUT_RDWR);
//        if_config ( localip, netmask, gateway, TRUE);
#endif
    }

    // helper functions

    int send_http_request(char* type, char* path, char* host)
    {

        char request[1024] = {0};

        //create the server request (to get the stream)
         sprintf(request,
                "%s %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
                "Accept: */*\r\n"
                "Connection: Keep-Alive\r\n\r\n",
                type,path,host);


        int len_req = strlen(request);
        return client_send(request,len_req);

    };

    // are we dealing with an ip or address
    char* address_to_ip(char* address_in)
    {
        for (unsigned int i=0;i<strlen(address_in);i++) {
            char digit = address_in[i];
            if ((digit < 48 || digit > 57) && digit != 46) {
                return getipbyname(address_in);
            }
        }

        return address_in; //already ip
    }

    char * get_local_ip()
    {
#ifdef _WII_
        return localip;
#else
        char buffer[80] = {0};
        struct in_addr addr;
        static char sip[80];
        sip[0] = '\0';

        if (gethostname(buffer, sizeof(buffer)) == 0 )
        {
            struct hostent *phe = gethostbyname(buffer);

            if (phe != 0){
                memcpy(&addr, phe->h_addr_list[0], sizeof( struct in_addr ));
                strcpy(sip, inet_ntoa( addr ));
            }else{
                strcpy(sip, "Host Unknown");
            }
        }else{
            strcpy(sip, "Host Unknown");
        }

        return sip;

#endif
   }
};

