#ifndef _STATION_H_
#define _STATION_H_

#include <string>

using namespace::std;

class station {
  public:
    string       name;
    string       server;
    string       path;
    unsigned int port;

    station();
    station(const char *url);
    station(const char *name, const char *url);
    station(string url);
    station(string name, string url);

    string url();
    void debug_print();
  private:
    void split_url(string url);
};

#endif
