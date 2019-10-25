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

// Custom structs to store data from a tcpMessage
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

// Struct to store a client's socket, IP address, and port number. The latter
// two are inside cli_addr.sin_addr and cli_addr.sin_port respectively.
struct clientInfoStruct
{
    sockaddr_in cli_addr;
    int sockfd;
};

// Global variables
std::atomic<bool> listenRunning(true); // Listener thread flag
std::atomic<bool> connectionsRunning(true); // Flag for all receiving threads
tcpMessage lastMessage; // Store the last message
std::mutex lastMessageMutex;
std::vector<std::thread> clients; // Store every thread in a vector
std::vector<clientInfoStruct> clientInfo; // Store info on every connection
std::mutex clientInfoMutex;

// Once the listener thread has established a connection, these threads
// take over handling the connection in terms of sending and receiving messages
void Recvthread(int sockfd, sockaddr_in &serv_addr, sockaddr_in &cli_addr)
{
    // Variable declarations
    int n;
    fd_set fdset;
    struct timeval tv;
    tcpMessage rcvMsg;
    tcpMessage sndMsg;

    // Create a new struct and add it to clientInfo vector
    clientInfoStruct thisClientInfo;
    thisClientInfo.sockfd = sockfd;
    thisClientInfo.cli_addr = cli_addr;
    clientInfoMutex.lock();
    clientInfo.push_back(thisClientInfo);
    clientInfoMutex.unlock();

    // While this and all threads are running, send/receive messages
    bool threadRunning = true;
    while (threadRunning && connectionsRunning)
    {
        // set up a 6 second timeout for recv() so it doesn't block
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        tv.tv_sec = 6; // 6 second timeout
        tv.tv_usec = 0;
        if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
        {
            // blocks until the client connected on this socket sends a message
            n = recv(sockfd, &rcvMsg, sizeof(tcpMessage), 0);

            // Check error codes on new message
            if (n < 0)
                error("ERROR reading from socket");
            else if (n == 0)
                threadRunning = false;

            // Generate a message to send
            sndMsg.nVersion = '1';
            sndMsg.nType = rcvMsg.nType;
            sndMsg.nMsgLen = rcvMsg.nMsgLen;

            // Do nothing if nVersion is not 1
            if (rcvMsg.nVersion != '1')
            {
                continue;
            }

            // Send received message to all other clients if nType is 0
            else if (rcvMsg.nType == '0')
            {
                lastMessageMutex.lock();
                lastMessage = rcvMsg;
                lastMessageMutex.unlock();

                // Walk vector of client info
                for (int i=0; i<clientInfo.size(); i++)
                {
                    // Ignore self
                    if (clientInfo[i].cli_addr.sin_port != thisClientInfo.cli_addr.sin_port)
                        // Send to every thread except this one
                        n = send(clientInfo[i].sockfd, &rcvMsg, sizeof(tcpMessage), 0);
                }

            }

            // If nType is 1, reverse the message and send it back
            else if (rcvMsg.nType == '1')
            {
                lastMessageMutex.lock();
                lastMessage = rcvMsg; // Record this as the last message received
                lastMessageMutex.unlock();
                memset(sndMsg.chMsg, 1000, 0);
                for (int i=0; i<rcvMsg.nMsgLen; i++)
                {
                    sndMsg.chMsg[i] = rcvMsg.chMsg[rcvMsg.nMsgLen - i-1];
                }
                // Send new message
                n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);

                // Check error codes on new message
                if (n < 0)
                    error("ERROR reading from socket");
                else if (n == 0)
                    threadRunning = false;
            }
        }
    }

    // Find and delete this connection from connectionInfo
    for (int i=0; i<clientInfo.size(); i++)
    {
        if (clientInfo[i].cli_addr.sin_port == thisClientInfo.cli_addr.sin_port)
            clientInfo.erase(clientInfo.begin()+i);
    }

    // Close the socket and quit
    sockClose(sockfd);
    sockQuit();

    #ifdef _WIN32
    std::cin.get();
    #endif
}

// Listen on a port specified by argv[1] and spawn new threads to handle
// sending and receiving data
void Listenthread(int portno)
{
    // Variable declarations
    struct sockaddr_in serv_addr;
    int sockfd, newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    fd_set fdset;
    struct timeval tv;

    // Create the socket
    sockInit();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Make sure the socket was created
    if (sockfd < 0)
        error("ERROR opening socket");
    // Zero out the variable serv_addr
    memset((char *)&serv_addr, sizeof(serv_addr), 0);

    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    // Listen for connections until it's time to stop
    printf("Listening for connections...\n");
    while (listenRunning)
    {
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);

        // Set up a select timeout to get around accept() blocking the thread
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        tv.tv_sec = 6; // 6 second timeout
        tv.tv_usec = 0;
        if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
        {

        // blocks until a client connects to the server
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        // Error checking
        if (newsockfd < 0)
            error("ERROR on accept");

        // Accept the connection, spawn a thread, and push new thread onto
        // vector of Recvthread thread objects
        cout << endl << "Accepted new connection..." << endl;
        clients.push_back(std::thread(Recvthread, newsockfd, std::ref(serv_addr), std::ref(cli_addr)));
        }
    }

    // close all Recvthread threads
    connectionsRunning = false;
    for (std::thread & th : clients)
    {
    	// If thread Object is Joinable then Join that thread.
    	if (th.joinable())
    		th.join();
    }
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
    std::thread listenerThread(Listenthread, portno);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get user commands
    bool mainRunning = true;
    char userCommand;
    while (mainRunning)
    {
        // get the user's command
        cout << "Please enter a command: ";
        cin >> userCommand;

        // If user entered 1, print IP address and port info
        if (userCommand == '1')
        {
            cout << "Number of Clients: " << clientInfo.size() << endl;
            cout << "IP Address\tPort" << endl;
            for (int i=0; i<clientInfo.size(); i++)
            {
                cout << inet_ntoa(clientInfo[i].cli_addr.sin_addr) << "\t";
                cout << ntohs(clientInfo[i].cli_addr.sin_port) << endl;
            }
        }

        // If user entered 0, display the last message received
        else if (userCommand == '0')
        {
            lastMessageMutex.lock();
            cout << "Last Message: " << lastMessage.chMsg;
            lastMessageMutex.unlock();
        }

        // If user entered q, quit the server
        else if (userCommand == 'q')
        {
            mainRunning = false;
        }

        // Anything else, give a warning, but ignore
        else
        {
            cout << "Command not found" << endl;
        }
    }

    // Outside the main loop. Set all thread flags to false and join threads
    listenRunning = false;
    connectionsRunning = false;
    listenerThread.join();
    // Rcvthreads are joined in the listener thread, so no need to worry

    // Exit program
    return 0;
}
