#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        cout << "Usage:\t ./ipfinder [link] [service]" << endl;
        cout << "\tlink - Link to follow. E.g. www.example.com, 192.0.2.123, ipv6.google.com, etc." << endl;
        // cout << "\tservice - Service/port to look for when connecting. E.g. 8080, 80, http, etc." << endl;
        exit(0);
    }

    cout << "Getting IPs from '" << argv[1] << "':\n" << endl;

    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(argv[1], "http", &hints, &res) != 0) exit(1);

    cout << "IPs:" << endl;
    for(p = res; p != NULL; p = p->ai_next)
    {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];

        if(p->ai_family == AF_INET)
        {
            addr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
        }else
        {
            addr = &((struct sockaddr_in6*)res->ai_addr)->sin6_addr;
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        cout << ipstr << endl;
    }
    freeaddrinfo(res);

    return 0;
}
