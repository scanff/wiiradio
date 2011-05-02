#include "globals.h"
#include "application.h"
#include <errno.h>
#define MAX_INIT_RETRIES 20

enum _net_protocols {
    TCP = 0,
    UDP
};

class dns
{
    public:


    char* getipbyname(const char *domain)
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

    app_wiiradio*   theapp;

    network(app_wiiradio* _theapp);
    ~network();

    //vars
    bool            client_connected;
    s32             connection_socket; //client
    struct          sockaddr_in client;
    unsigned int    clientlen;
    int             c_protocol;
    int             mode;


#ifdef _WIN32

    int net_init();
    int net_bind(s32 s, struct sockaddr * sa, int size);
    int net_listen(s32 s, int a);
    s32 net_accept (s32 s, struct sockaddr * c, u32* len);
    s32 net_socket(int domain,int type,int protocol);
    s32 net_connect(s32 s,sockaddr * addy, int socklen_t);
    int net_send(s32 s, char *buffer,int len,int flags);
    int net_recv(s32 s, char* buffer, int len, int flags);
    int net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len);
    int net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len);
    void net_ioctl(s32 s, int b, unsigned long* c);
    void net_close(s32 s);

#endif

#ifdef _LINUX_

    int net_init();
    int net_bind(s32 s, struct sockaddr * sa, int size);
    int net_listen(s32 s, int a);
    s32 net_accept (s32 s, struct sockaddr * c, u32* len);
    s32 net_socket(int domain,int type,int protocol);
    s32 net_connect(s32 s,sockaddr * addy, int socklen_t);
    int net_send(s32 s, char *buffer,int len,int flags);
    int net_recv(s32 s, char* buffer, int len, int flags);
    int net_sendto(s32 s, char* buffer, int len,int flags,struct sockaddr * addy,u32 addy_len);
    int net_recvfrom(s32 s,char*buffer,int len,int flags,struct sockaddr * addy,u32* addy_len);
    void net_ioctl(s32 s, int b, unsigned long* c);
    void net_close(s32 s);

#endif

    const int client_connect(const char* ip,int port, int protocol);
    int client_send(void* buffer,int len);
    int client_recv(char* buffer,int len);
    void client_close();
    void client_restart();
    int send_http_request(const char* type, const char* path, const char* host);
    const char* address_to_ip(const char* address_in);
    char * get_local_ip();

};

