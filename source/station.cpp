#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include "station.h"

/* Class for radio stations. */

station::station() : local(0)
{
    name = "Uninitialized";
    split_url("");
}

station::station(const char *c_url) : local(0)
{
    name = "Uninitialized";
    string url = c_url;
    split_url(url);
}

station::station(const char *c_name, const char *c_url) : local(0)
{
    name = c_name;
    string url = c_url;
    split_url(url);
}

station::station(string url) : local(0)
{
    name = "Uninitialized";
    split_url(url);
}

station::station(string _name, string url) : local(0)
{
    name = _name;
    split_url(url);
}

station::station(string _name, string _path,bool _local)
{
    name = _name;
    path = _path;
    local = _local;
}

void station::split_url(string url)
{
    // Default value for port
    port = 80;
    // The URL has to start with http://
    if (url.substr(0, 7).compare("http://")) return;
    // Now we have to parse for different kinds of URLs, with port/paths and
    // without, for example
    // server:port/path
    // server/path

    // First occurance of either ':' or "/"
    size_t pos = url.find_first_of(":/", 7);
    // If port or path are found, parse them
    if (pos != url.npos)
    {
        // Save url
        server = url.substr(7, pos-7);
        // Try to find port
        if (url[pos] == ':')
        {
            // Parse for path after port
            size_t pos2 = url.find_first_of("/", pos-7);
            port = atoi(url.substr(pos+1).c_str());
            // If not found, only port is mentioned
            if (pos2 != url.npos)
                path = url.substr(pos2);
        }
        // No port delimiter, only save path
        else
            path = url.substr(pos);
    }
}

string station::url()
{
    ostringstream s;
    s << server << ":" << port << path;
    return s.str();
}

void station::debug_print()
{
    printf("Station Name  : %s\n        Server: %s\n        Port  : %d\n        Path  : %s\n",
           name.c_str(), server.c_str(), port, path.c_str());
}
