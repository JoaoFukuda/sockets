#include <iostream>
#include <cstring>

#include <sys/types.h>      // Nothing yet?
#include <sys/socket.h>     // Nothing yet?
#include <netdb.h>          // addrinfo and related stuff

#include <arpa/inet.h>      // inet_ntop and pton
#include <netinet/in.h>     // Nothing yet?

#define MAXBUFLEN 64

using namespace std;

void QuitProgram(int n)
{
    cout << "Error! Exit code: " << n << endl;
    exit(n);
}

int main(int argc, char const *argv[])
{
    const char *ip = "localhost";
    const char *port = "42000";
    if(argc > 1)
    {
        ip = argv[1];
        if(argc > 2)
        {
            port = argv[2];
        }
    }

    int status, sock;
    struct addrinfo hint, *res;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(ip, port, &hint, &res)) != 0) QuitProgram(2);

    if((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) QuitProgram(3);
    if((status = connect(sock, res->ai_addr, res->ai_addrlen)) == -1) QuitProgram(4);

    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_storage *tempaddr;
    getpeername(sock, (struct sockaddr*)tempaddr, (socklen_t *)sizeof(*tempaddr));
    inet_ntop(AF_INET, tempaddr, ipstr, INET6_ADDRSTRLEN);

    cout << "Connected to address: " << ipstr << ":" << port << endl;

    char tempmsg[MAXBUFLEN];
    string msg = "";
    cout << "Message: ";
    int msgitter = 0;
    while(recv(sock, &tempmsg, MAXBUFLEN, 0) != 0)
    {
        msg += tempmsg;
    }
    cout << msg << endl;

    shutdown(sock, 2);
    freeaddrinfo(res);
    return 0;
}
