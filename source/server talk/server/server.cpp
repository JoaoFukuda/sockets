#include <iostream>
#include <string>
#include <cstring>

#include <sys/types.h>      // Nothing yet?
#include <sys/socket.h>     // socket() and bind()
#include <netdb.h>          // addrinfo and related stuff
#include <unistd.h>         // gethostname() and fork()

#include <arpa/inet.h>      // inet_ntop and pton
#include <netinet/in.h>     // Nothing yet?

#define MAXBUFLEN 64
#define BACKLOG 1

using namespace std;

void QuitProgram(int n)
{
    cout << "Error! Exit code: " << n << endl;
    exit(n);
}

int main(int argc, char *argv[])
{
    string my_port = "42000";
    if(argc == 2)
    {
        my_port = argv[1];
    }
    int status, sock, c_sock;
    socklen_t addr_size;
    struct sockaddr_storage c_addr;
    struct addrinfo hint, *res;
    
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL, my_port.c_str(), &hint, &res)) != 0) QuitProgram(1);

    if((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) QuitProgram(2);
    if((status = bind(sock, res->ai_addr, res->ai_addrlen)) == -1) QuitProgram(3);
    if((status = listen(sock, BACKLOG)) == -1) QuitProgram(4);

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, res->ai_addr, ipstr, INET_ADDRSTRLEN);

    cout << "Listening for connections on address: " << ipstr << ":" << my_port << endl;

    addr_size = sizeof(c_addr);
    c_sock = accept(sock, (struct sockaddr *)&c_addr, &addr_size);

    cout << "> ";
    string msg;
    getline(cin, msg);
    while(msg.length() != 0)
    {
        // The buffer needs to have one fucking char left at the end of it so the end of string can happen
        short int msglen = MAXBUFLEN-1;
        if(msg.length() < MAXBUFLEN) msglen = msg.length();
        send(c_sock, msg.substr(0, msglen).c_str(), MAXBUFLEN, 0);
        msg = msg.substr(msglen, msg.length()-msglen+1);
    }

    cout << "Message sent" << endl;

    shutdown(sock, 2);
    freeaddrinfo(res);
    return 0;
}