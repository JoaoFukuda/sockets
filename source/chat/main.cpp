#include <iostream>
#include <cstring>
#include "rlutil.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>        //For multithreadin in Unix-based OS

#define DEFAULT_ADDR "localhost"
#define DEFAULT_PORT "42000"
#define MAXBUFLEN 100
#define MAXUSERS 3

using namespace std;

struct UsrInfo{
    int id, c_sock;
    int *users;
    UsrInfo(int x, int y, int *users)
    {
        id = x;
        c_sock = y;
        this->users = users;
    }
};

struct SrvInfo{
    int *users;
    SrvInfo(int *users)
    {
        this->users = users;
    }
};

int sock, status;

// Default exit mode for when errors are given on a method call
//  Should be changed later for some methods (for them to be solved automatically)
void ErrorQuit(int n)
{
    cout << "Exit error " << n << endl;
    exit(1);
}

void SendMessage(string msg, int send_sock = sock)
{
    while(msg.length() != 0)
    {
        short int msglen = MAXBUFLEN-1;
        if(msg.length() < MAXBUFLEN) msglen = msg.length();
        send(send_sock, msg.substr(0, msglen).c_str(), MAXBUFLEN, 0);
        msg = msg.substr(msglen, msg.length()-msglen+1);
    }
}

// Handles receiving data from a client
void *HandleConnection(void *info)
{
    cout << "User connected..." << endl;
    // Translate user info to something we can understand
    UsrInfo usrInfo = *(UsrInfo*)info;

    //Create a buffer to receive messages from the user
    char msg[MAXBUFLEN];

    // Loop while the user is still connected
    while(recv(usrInfo.c_sock, msg, MAXBUFLEN, 0) != 0)
    {
        for(int i = 0; i < MAXUSERS; i++)
        {
            if(i != usrInfo.id && usrInfo.users[i] != -1)
            {
                SendMessage(msg, usrInfo.users[i]);
            }
        }
    }
    
    usrInfo.users[usrInfo.id] = -1;
    cout << "User " << usrInfo.id << " disconnected." << endl;

    pthread_exit(NULL);
}

// Return the first free position
int FindFreeSpace(int *users)
{
    for(int i = 0; i < MAXUSERS; i++)
    {
        if(users[i] == -1) return i;
    }
    return -1;
}

// Handle all incomming connections comming to the server
void *HandleConnections(void *info)
{
    // Translate the server info that came through the thread to be used
    SrvInfo srvInfo = *(SrvInfo*)info;

    while(true)
    {
        //Wait for a free space on the users list
        while(FindFreeSpace(srvInfo.users) == -1);

        // declare variables for storing info on the new connection
        struct sockaddr_storage clientInfo;
        socklen_t addrLen = sizeof(clientInfo);

        // Make this new connection
        int c_sock = accept(sock, (struct sockaddr*)&clientInfo, &addrLen);

        // Prepare the informations to be sent to its thread
        int usrId = FindFreeSpace(srvInfo.users);
        srvInfo.users[usrId] = c_sock;
        UsrInfo usrInfo(usrId, c_sock, srvInfo.users);

        // Create a thread, fire and forget
        pthread_t thread;
        pthread_create(&thread, NULL, HandleConnection, &usrInfo);
    }
}

void Server(struct addrinfo *serverinfo)
{
    // Bind the socket to a port and start listening for incomming connections
    if(bind(sock, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
        ErrorQuit(3);
    if(listen(sock, 1) == -1)
        ErrorQuit(4);

    cout << "Listening to incomming connections..." << endl;

    // Make the user list to store the socket int of all users
    int users[MAXUSERS];
    for(int n = 0; n < MAXUSERS; n++) users[n] = -1;

    // Put all server info on a package to send it to a thread
    //  The thread takes care of all incomming connections
    SrvInfo srvInfo(users);
    pthread_t connHandler;
    pthread_create(&connHandler, NULL, HandleConnections, &srvInfo);

    // Sees if the client list has someone, if not, then exit the program
    while(users[0] == -1);  //Wait for the first connection, then exit as soon as there is no other connections
    bool hold = true;
    while(hold)
    {
        hold = false;
        for(int n = 0; n < MAXUSERS; n++)
        {
            if(users[n] != -1)
            {
                hold = true;
                continue;
            }
        }
    }

    // Frees the memory getaddrinfo() kindly reserved for us
    freeaddrinfo(serverinfo);
    shutdown(sock, 2);
}

void *C_Receiver(void *vd)
{
    char msg[MAXBUFLEN];
    while(recv(sock, msg, MAXBUFLEN, 0) != 0)
    {
        cout << msg << endl;
    }
    pthread_exit(NULL);
}

void C_Sender()
{
    string msg;
    getline(cin, msg);
    while(strcmp(msg.c_str(), "/quit") != 0)
    {
        SendMessage(msg);
        getline(cin, msg);
    }
}

void Client(struct addrinfo *serverinfo)
{
    if(connect(sock, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
       ErrorQuit(6);

    pthread_t Treceiver;
    pthread_create(&Treceiver, NULL, C_Receiver, NULL);
    C_Sender();

    // Frees the memory getaddrinfo() kindly reserved for us
    freeaddrinfo(serverinfo);
    shutdown(sock, 2);
}

int main(int argc, char const *argv[])
{
    bool is_server = false;                                 // See wether the current program is a server or not
    const char *ip = DEFAULT_ADDR, *port = DEFAULT_PORT;    // Default values for the ip and host
    struct addrinfo temp, *serverinfo;                      // Info on the local machine (server) or the target's machine (client)

    // Clears the memory and set the default values of the connection to fit the required needs
    memset(&temp, 0, sizeof temp);
    temp.ai_family = AF_UNSPEC;
    temp.ai_socktype = SOCK_STREAM;

    // Decies wether the program will be a server or a client based on their arguments
    //  A server does not need ip adress as it will always be set on the localhost
    //  If client or no arguments about it be given, then a client it will be
    if(argc > 1)
    {
        if(strcmp(argv[1], "server") == 0)
        {
            is_server = true;
            if(argc > 2) port = argv[2];
        }
        else if(strcmp(argv[1], "client") == 0)
        {
            if(argc > 2)
            {
                ip = argv[2];
                if(argc > 3) port = argv[2];
            }
        }
        else
        {
            ip = argv[1];
            if(argc > 2) port = argv[1];
        }
    }

    // Get info on the targeted machine based on the ip and port and get it's relative socket
    if((status = getaddrinfo(ip, port, &temp, &serverinfo)) != 0)
        ErrorQuit(1);
    if((sock = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1)
        ErrorQuit(2);

    if(is_server) Server(serverinfo);
    else Client(serverinfo);

    return 0;
}
