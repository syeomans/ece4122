/*
Author: Samuel Yeomans
Class: ECE 4122
Date: October 22, 2019
Description:
Console program that takes as a command line argument the port number on which
the TCP Server will listen for connection requests. A separate thread is
created to handle the data received from each remote client and the remote
clients can continue to send and receive data on the connections until either
the server or the client closes the connection. The TCP server maintains a list
of all connected clients so that it can send out the appropriate messages. The
TCP server receives data from clients without blocking the main application
thread. The program responds to user input while handling socket communications
at the same time.

Template from: http://www.linuxhowtos.org/C_C++/socket.htm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>
using namespace std;

#ifdef _WIN32
   /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501  /* Windows XP. */
    #endif
    #include <winsock2.h>
    #include <Ws2tcpip.h>

    #pragma comment (lib, "Ws2_32.lib")
#else
   /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */

    typedef int SOCKET;
#endif
/////////////////////////////////////////////////
// Cross-platform socket initialize
int sockInit(void)
{
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
    return 0;
#endif
}
/////////////////////////////////////////////////
// Cross-platform socket quit
int sockQuit(void)
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}
/////////////////////////////////////////////////
// Cross-platform socket close
int sockClose(SOCKET sock)
{

    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0)
    {
        status = closesocket(sock);
    }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0)
    {
        status = close(sock);
    }
#endif

    return status;

}
/////////////////////////////////////////////////
// Output error message and exit
void error(const char *msg)
{
    perror(msg);
    // Make sure any open sockets are closed before calling exit
    exit(1);
}
/////////////////////////////////////////////////

// Global variables
struct tcpMessage
{
unsigned char nVersion;
unsigned char nType;
unsigned short nMsgLen;
char chMsg[1000];
};
std::atomic<bool> listenRunning(true);
std::atomic<bool> connectionsRunning(true);
tcpMessage lastMessage;
std::mutex lastMessageMutex;
std::vector<std::thread> clients;
std::mutex sendallMutex;
int sendallFlag = 1;

void Recvthread(int sockfd, sockaddr_in &serv_addr)
// std::function<void()> Recvthread = [](int sockfd, sockaddr_in &serv_addr)
// std::function<void(int sockfd, sockaddr_in &serv_addr)> Recvthread = []()
{
    int portno;
    // socklen_t clilen;
    //char buffer[256];
    // struct sockaddr_in cli_addr;
    int n;
    //int sockLen = sizeof(serv_addr);
    fd_set fdset;
    struct timeval tv;
    // FD_ZERO(&fdset);
    // FD_SET(sockfd, &fdset);
    tcpMessage rcvMsg;
    tcpMessage sndMsg;
    int thisSendallFlag = sendallFlag;

    bool threadRunning = true;
    while (threadRunning && connectionsRunning)
    {
        // tcpMessage pkt;
        // if(recvfrom(sockfd, &pkt, sizeof(tcpMessage), 0, (sockaddr*)&serv_addr, (socklen_t*)&sockLen) < 0)
        // {
        //     //Failed to Recieve Data
        //     cout << "Hello4" << endl;
        //     //threadRunning = false;
        //     break;
        // }
        // else
        // {
        //     //Recieved Data!!
        //     cout << "Hello5" << endl;
        //     //threadRunning = false;
        // }
        //break;

        // memset(buffer, 256, 0);
        if (thisSendallFlag != sendallFlag)
        {
            cout << "Sending all" << endl;
            n = send(sockfd, &lastMessage, sizeof(tcpMessage), 0);
            thisSendallFlag = thisSendallFlag * -1;
        }

        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        tv.tv_sec = 6;             /* 10 second timeout */
        tv.tv_usec = 0;
        if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
        {
            // blocks until the client connected on this socket sends a message
            //n = recv(sockfd, buffer, 255, 0);
            cout << "Hello4" << endl;
            n = recv(sockfd, &rcvMsg, sizeof(tcpMessage), 0);



            //buffer[n] = 0; // String was not null terminated

            // if (n < 0)
            //     error("ERROR reading from socket");
            cout << "Hello5" << endl;
            //cout << rcvMsg.nVersion << endl;
            //cout << "Here is the message: " << rcvMsg.nVersion << " " << rcvMsg.nType << " " << rcvMsg.nMsgLen << " " << rcvMsg.chMsg << ":)" << endl;

            // n = send(sockfd, "I got your message", 18, 0);
            sndMsg.nVersion = '1';
            sndMsg.nType = rcvMsg.nType;
            sndMsg.nMsgLen = rcvMsg.nMsgLen;
            // if (rcvMsg.nVersion != '1')
            // {
            //     continue;
            // }
            // // else if (rcvMsg.nType == '0')
            // // {
            // //     // Send to all other clients
            // //     sendallMutex.lock()
            // //     sendallId = std::this_thread::get_id();
            // //
            // // }

            // lastMessage = rcvMsg;
            if (rcvMsg.nVersion != '1')
            {
                cout << "Version not 1" << endl;
                continue;
            }

            else if (rcvMsg.nType == '0')
            {
                cout << "Type is 0" << endl;
                // Send to all other clients
                lastMessageMutex.lock();
                lastMessage = rcvMsg;
                lastMessageMutex.unlock();

                sendallMutex.lock();
                sendallFlag = sendallFlag * -1;
                sendallMutex.unlock();
                thisSendallFlag = thisSendallFlag * -1;
            }

            else if (rcvMsg.nType == '1')
            {
                cout << "Type is 1" << endl;
                lastMessageMutex.lock();
                lastMessage = rcvMsg;
                lastMessageMutex.unlock();
                memset(sndMsg.chMsg, 1000, 0);
                for (int i=0; i<rcvMsg.nMsgLen; i++)
                {
                    sndMsg.chMsg[i] = rcvMsg.chMsg[rcvMsg.nMsgLen - i-1];
                }
                //cout << "chmsg: " << sndMsg.chMsg << endl;
                n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);
            }

            //n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);

            // if (n < 0)
            //     error("ERROR writing to socket");

            //threadRunning = false;
        }
    }

    //sockClose(newsockfd);
    cout << "Closing socket" << endl;
    sockClose(sockfd);

    sockQuit();

    #ifdef _WIN32
    std::cin.get();
    #endif
    cout << "Closed socket" << endl;
}

void Listenthread(int portno)
{
    struct sockaddr_in serv_addr;
    int sockfd, newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    fd_set fdset;
    struct timeval tv;
    // int recVal;
    // int sockLen = sizeof(serv_addr);

    sockInit();
    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Make sure the socket was created
    if (sockfd < 0)
        error("ERROR opening socket");
    // Zero out the variable serv_addr
    memset((char *)&serv_addr, sizeof(serv_addr), 0);

    // bool newConnection = true;
    // if (newConnection)
    // {
    //
    // }
    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    printf("Listening for connections...\n");

    // listen(sockfd, 5);

    while (listenRunning)
    {
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);

        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);

        //bool running = true;
        cout << "Hello1" << endl;

        tv.tv_sec = 6;             /* 6 second timeout */
        tv.tv_usec = 0;
        if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
        {
            cout << "Hello3" << endl;

            // blocks until a client connects to the server
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

            if (newsockfd < 0)
                error("ERROR on accept");

            printf("Accepted new connection...\n");

            clients.push_back(std::thread(Recvthread, newsockfd, std::ref(serv_addr)));
        }

        // // blocks until a client connects to the server
        // newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        //
        // if (newsockfd < 0)
        //     error("ERROR on accept");
        //
        // printf("Accepted new connection.../n");
        //
        // clients.push_back(std::thread(Recvthread, newsockfd, std::ref(serv_addr)));
    }

    // close all connection threads
    cout << "Closing listener and all sockets" << endl;
    connectionsRunning = false;
    for (std::thread & th : clients)
    {
    	// If thread Object is Joinable then Join that thread.
    	if (th.joinable())
    		th.join();
    }
    cout << "Closed listener and all sockets" << endl;

}

// Main
int main(int argc, char *argv[])
{
    // Check command line arguments
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // Inits
    int portno = atoi(argv[1]);

    // Spawn thread to manage connections
    std::thread connectionThread(Listenthread, portno);

    // Do stuff
    bool mainRunning = true;
    char userCommand;
    while (mainRunning)
    {
        // get the user's command
        cout << "Please enter a command: ";
        cin >> userCommand;

        if (userCommand == '1')
        {
            cout << '1' << endl;
        }
        else if (userCommand == '0')
        {
            lastMessageMutex.lock();
            cout << "Last Message: " << lastMessage.chMsg;
            lastMessageMutex.unlock();
        }
        else if (userCommand == 'q')
        {
            cout << 'q' << endl;
            mainRunning = false;
        }
        else
        {
            cout << "Command not found" << endl;
        }
    }

    cout << "Closing listener and main" << endl;
    listenRunning = false;
    connectionsRunning = false;
    connectionThread.join();
    cout << "Closed listener and main" << endl;

    return 0;
}
