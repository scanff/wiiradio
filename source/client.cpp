#include "globals.h"
#include "application.h"
#include <errno.h>

#include "client.h"


network::network(app_wiiradio* _theapp)
: client_connected(false)
{
}

network::~network()
{
    if (client_connected) client_close();
}

#ifdef _WIN32

/* wrappers for functions on windows */

int network::net_init()
{
    return 0;
}

int network::net_bind(s32 s, struct sockaddr * sa, int size)
{
    return bind(s,( struct sockaddr*) sa, size);
}

int network::net_listen(s32 s, int a)
{
    return listen(s, a);
}

s32 network::net_accept (s32 s, struct sockaddr * c, u32* len)
{
    return accept ((SOCKET)s, (struct sockaddr *) c, (int*)len);
}

s32 network::net_socket(int domain,int type,int protocol)
{
    return socket(domain, type, protocol);
}

s32 network::net_connect(s32 s,sockaddr * addy, int socklen_t)
{
   return connect(s,addy,socklen_t);
}

int network::net_send(s32 s, char *buffer,int len,int flags)
{
    return send(s, buffer, len, flags);
}

int network::net_recv(s32 s, char* buffer, int len, int flags)
{
    return recv(s, buffer, len, flags);
}

int network::net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len)
{
    return sendto(s,buffer,len,flags,addy,(int)addy_len);
}

int network::net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len)
{
    return recvfrom(s,buffer,len,flags,addy,(int*)addy_len);
}

void network::net_ioctl(s32 s, int b, unsigned long* c)
{
    ioctlsocket( s, b, c ); //non blocking
}

void network::net_close(s32 s)
{
    closesocket(s);
}

#endif

#ifdef _LINUX_

/* wrappers for functions on linux */

int network::net_init()
{
    return 0;
}

int network::net_bind(s32 s, struct sockaddr * sa, int size)
{
    return bind(s,( struct sockaddr*) sa, size);
}

int network::net_listen(s32 s, int a)
{
    return listen(s, a);
}

s32 network::net_accept (s32 s, struct sockaddr * c, u32* len)
{
    return accept (s, (struct sockaddr *) c, len);
}

s32 network::net_socket(int domain,int type,int protocol)
{
    return socket(domain, type, protocol);
}

s32 network::net_connect(s32 s,sockaddr * addy, int socklen_t)
{
   return connect(s,addy,socklen_t);
}

int network::net_send(s32 s, char *buffer,int len,int flags)
{
    return send(s, buffer, len, flags);
}

int network::net_recv(s32 s, char* buffer, int len, int flags)
{
    return recv(s, buffer, len, flags);
}

int network::net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len)
{
    return sendto(s,buffer,len,flags,addy,(int)addy_len);
}

int network::net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len)
{
    return recvfrom(s,buffer,len,flags,addy,addy_len);
}

void network::net_close(s32 s)
{
}

 void network::net_ioctl(s32 s, int b, unsigned long* c)
{
//        ioctlsocket( s, b, c ); //non blocking
}

#endif


const int network::client_connect(const char* ip,int port, int protocol)
{
    client_connected = false;

    if(!ip[0]) return 0;

    c_protocol = protocol;

    const char* ip_checked = address_to_ip(ip);
    if (!ip_checked) return 0;


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
#ifdef _WIN32
        unsigned long flag = O_NONBLOCK;
        net_ioctl(connection_socket, FIONBIO, &flag); // does not work on WII
#endif
    }

    if(protocol == TCP)
    {
        u64 start_time = get_tick_count();

        while(1)
        {
            if ((get_tick_count() - start_time) > 2000) // 2 sec timeout
            {
                net_close(connection_socket);
                connection_socket = 0;
                return 0;
            }

            int res = net_connect(connection_socket,(struct sockaddr*)&client,sizeof(client));

            if (res < 0)
            {
#ifndef _WIN32
                if (res == -EISCONN)
#else
                res = WSAGetLastError();

                if(res == WSAEISCONN)
#endif
                {
                    break;
                }
#ifndef _WIN32
                if (res == -EINPROGRESS || res == -EALREADY)
#else
                if (res == WSAEWOULDBLOCK || res == WSAEALREADY || res == WSAEINPROGRESS)
#endif
                {
                    Sleep(2);
                    continue;
                }


                net_close(connection_socket);
                connection_socket = 0;
                return 0;

            }else break;

        }

    }
    client_connected = true;

    return 1;
}

int network::client_send(void* buffer,int len)
{
    if (!client_connected) return -1;

    u32 clen = sizeof(client);
    return c_protocol == TCP ?   net_send(connection_socket, (char*)buffer, len, 0) :
                                 net_sendto(connection_socket, (char*)buffer, len, 0,(struct sockaddr*)&client,clen);
}

int network::client_recv(char* buffer,int len)
{
    int ret = 1;

    if (!client_connected) return -1;
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
            timeval timevalInfo; // tells select how long to wait
            timevalInfo.tv_sec = 0;
            timevalInfo.tv_usec = 10; // wait for 10
            //made fd_set struct
            fd_set Sockets; // contains list of sockets for select
            Sockets.fd_count = 1; // one socket
            Sockets.fd_array[0] = connection_socket; // add it
            //call select to determine readability of function
            ret = select(NULL, &Sockets, NULL, NULL, &timevalInfo);

        #else
          pollfd fds;
          fds.fd = connection_socket;
          fds.events = POLLIN | POLLPRI;
          ret = poll(&fds, 1, 1);
        #endif
    #endif
#endif
    // If there is no data to read, don't try
    if (!ret)
        return -EAGAIN;
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

void network::client_close()
{
    if (!client_connected) return;
    net_close(connection_socket);
}

void network::client_restart()
{
#ifdef _WII_
    //net_close(connection_socket);
//        net_shutdown(connection_socket,SHUT_RDWR);
//        if_config ( localip, netmask, gateway, TRUE);
#endif
}

// helper functions

int network::send_http_request(const char* type, const char* path, const char* host)
{
//        if(!netinit) return 0;

    char request[2048];


    //create the server request (to get the stream)
     sprintf(request,
            "%s %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.2.16) Gecko/20110319 Firefox/3.6.16\r\n"//"User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
            "Accept: */*\r\n"
            "Keep-Alive: 115\r\n"
            "Connection: keep-alive\r\n"
            "Cookie: 345093458304985093845; UNAUTHID=1.f90f347048e111e0907a4dee3834dc8a.6e14\r\n"
            "\r\n\r\n",
            type,path,host);


    int len_req = strlen(request);
    return client_send(request,len_req);

}

// are we dealing with an ip or address
const char* network::address_to_ip(const char* address_in)
{
    for (unsigned int i=0;i<strlen(address_in);i++) {
        char digit = address_in[i];
        if ((digit < 48 || digit > 57) && digit != 46) {
            return getipbyname(address_in);
        }
    }

    return address_in; //already ip
}

char * network::get_local_ip()
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



