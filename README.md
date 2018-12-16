# Sockets
Some programs I made in linux using sockets in C++

## Chat
Chat program is a program that creates a chat server or chat client to... *chat*. This program is the simplest i could think of, but the messages are not limited by size of buffer
### How to use:
#### Client program:
./chat client [address] [port] or ./chat [address] [port]
address - The server's address is put here. If none is specified, it will look for a program on the 'localhost' address.
port - The port your server is bound to. If no port is specified, it'll choose 42000 (default port).
#### Server program:
./chat server [port]
### How to compile:
Just use the default c++ compiler with the command:
g++ ./source/chat/main.cpp -o ./chat -lpthread
or modify if needed if you know what you are doing (or not).



## Server talks
Server/client programs that are but a broadcast program. Server talks and all the clients listen with no way of responding.
It was mostly made to see how to send messages of any size.
Can only support one client at the moment (but i plan on expanding that).
### How to use:
#### Client program:
./client.out [address] [port]
address - The server's address is put here. If none is specified, it will look for a program on the 'localhost' address.
port - The port your server is bound to. If no port is specified, it'll choose 42000 (default port).
#### Server program:
./server.out [port]
port - The port you will bind your program to. If no port is specified, it'll choose 42000 (default port).
Address will always be the localhost.
### How to compile:
##### Client:
g++ ./source/server\ talks/client/client.cpp -o ./source/server\ talks/client.out
##### Server:
g++ ./source/server\ talks/server/server.cpp -o ./source/server\ talks/server.out



## IpFinder
Scout the address for its IP. (Like a pocket DNS resolver for you).
### How to use:
./ipfinder [address]
address - The adress to scout.
### How to compile:
g++ ./source/ipfinder/main.cpp -o ./ipfinder

## Contributors
* [João Fukuda](https://github.com/JoaoFukuda) - *Main programmer (?)*
