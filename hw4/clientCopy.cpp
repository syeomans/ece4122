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

#ifdef _WIN32
   /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#else
   /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <string.h>
#include <stdio.h>
#include <thread>
#include <atomic>
#include <mutex>

typedef int SOCKET;
#endif

using namespace std;

int sockInit(void)
{
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
    return 0;
#endif
}

int sockQuit(void)
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}

/* Note: For POSIX, typedef SOCKET as an int. */

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

void error(const char *sndMsg)
{
    perror(sndMsg);

    exit(0);
}

struct tcpMessage
{
unsigned char nVersion;
unsigned char nType;
unsigned short nMsgLen;
char chMsg[1000];
};
std::atomic<bool> running(true);
mutex consoleMutex;


void sendThread(int sockfd)
{
    char buffer[256];
    tcpMessage sndMsg;
    // tcpMessage rcvMsg;
    sndMsg.nVersion = '1';
    int n;
    // fd_set fdset;
    // struct timeval tv;

    while(running)
    {
        printf("Please enter the message: ");

        memset(buffer, 256, 0);
        fgets(buffer, 255, stdin);

        if (buffer[0] == 'v' and buffer[1] == ' ')
        {
            string s = "";
            for (int i = 2; i < sizeof(buffer); i++) {
                if (isdigit(buffer[i]))
                {s = s + buffer[i];}
                else
                {break;}
            }
            sndMsg.nVersion = stoi(s);
        }

        else if (buffer[0] == 'q')
        {
            // memset(sndMsg.chMsg, 1000, 0);
            // sndMsg.nType = 1;
            // sndMsg.chMsg[0] = 'q';
            // n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);
            running = false;
        }

        else if (buffer[0] == 't' and buffer[1] == ' ')
        {
            //memset(sndMsg.chMsg, 1000, 0);
            sndMsg.nType = buffer[2];
            string s = "";
            unsigned short charCounter = 0;
            for (int i = 4; i < sizeof(buffer); i++) {
                sndMsg.chMsg[charCounter] = buffer[i];
                charCounter++;
            }
            //sndMsg.chMsg[charCounter-1] = ' ';
            sndMsg.nMsgLen = strlen(sndMsg.chMsg)-1;

            //n = send(sockfd, buffer, strlen(buffer), 0);
            n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);

            if (n < 0)
                error("ERROR writing to socket");

            std::this_thread::sleep_for(std::chrono::seconds(2));

            // //memset(buffer, 256, 0);
            // FD_ZERO(&fdset);
            // FD_SET(sockfd, &fdset);
            // tv.tv_sec = 6;             /* 6 second timeout */
            // tv.tv_usec = 0;
            // if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
            // {
            //     // n = recv(sockfd, buffer, 255, 0);
            //     n = recv(sockfd, &rcvMsg, sizeof(tcpMessage), 0);
            //
            //     // buffer[n] = 0;
            //     //
            //     // if (n < 0)
            //     //     error("ERROR reading from socket");
            //
            //     //printf("%s\n", buffer);
            //     printf("Received Msg Type: %c; Msg: %s\n", rcvMsg.nType, rcvMsg.chMsg);
            // }
            // else
            // {
            //     printf("No message received\n");
            // }
        }

        else
        {
            printf("Command not found\n");
        }
    }
}

void recThread(int sockfd)
{
    // char buffer[256];
    // tcpMessage sndMsg;
    tcpMessage rcvMsg;
    // sndMsg.nVersion = '1';
    int n;
    fd_set fdset;
    struct timeval tv;

    while(running)
    {
        // printf("Please enter the message: ");
        //
        // memset(buffer, 256, 0);
        // fgets(buffer, 255, stdin);
        //
        // if (buffer[0] == 'v' and buffer[1] == ' ')
        // {
        //     string s = "";
        //     for (int i = 2; i < sizeof(buffer); i++) {
        //         if (isdigit(buffer[i]))
        //         {s = s + buffer[i];}
        //         else
        //         {break;}
        //     }
        //     sndMsg.nVersion = stoi(s);
        // }
        //
        // else if (buffer[0] == 'q')
        // {
        //     // memset(sndMsg.chMsg, 1000, 0);
        //     // sndMsg.nType = 1;
        //     // sndMsg.chMsg[0] = 'q';
        //     // n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);
        //     running = false;
        // }
        //
        // else if (buffer[0] == 't' and buffer[1] == ' ')
        // {
        //     //memset(sndMsg.chMsg, 1000, 0);
        //     sndMsg.nType = buffer[2];
        //     string s = "";
        //     unsigned short charCounter = 0;
        //     for (int i = 4; i < sizeof(buffer); i++) {
        //         sndMsg.chMsg[charCounter] = buffer[i];
        //         charCounter++;
        //     }
        //     //sndMsg.chMsg[charCounter-1] = ' ';
        //     sndMsg.nMsgLen = strlen(sndMsg.chMsg)-1;
        //
        //     //n = send(sockfd, buffer, strlen(buffer), 0);
        //     n = send(sockfd, &sndMsg, sizeof(tcpMessage), 0);
        //
        //     if (n < 0)
        //         error("ERROR writing to socket");

            //memset(buffer, 256, 0);
            FD_ZERO(&fdset);
            FD_SET(sockfd, &fdset);
            tv.tv_sec = 6;             /* 6 second timeout */
            tv.tv_usec = 0;
            if (select(sockfd + 1, &fdset, NULL, NULL, &tv) == 1)
            {
                // n = recv(sockfd, buffer, 255, 0);
                n = recv(sockfd, &rcvMsg, sizeof(tcpMessage), 0);

                // buffer[n] = 0;
                //
                // if (n < 0)
                //     error("ERROR reading from socket");

                //printf("%s\n", buffer);
                consoleMutex.lock();
                printf("\nReceived Msg Type: %c; Msg: %s\n", rcvMsg.nType, rcvMsg.chMsg);
                consoleMutex.unlock();
            }
            // else
            // {
            //     printf("No message received\n");
            // }
        //}

        // else
        // {
        //     printf("Command not found\n");
        // }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // fd_set fdset;
    // struct timeval tv;



    // char buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    sockInit();
    // Convert string to int
    portno = atoi(argv[2]);
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    // Zero out serv_addr variable
    memset((char *)&serv_addr, sizeof(serv_addr), 0);

    serv_addr.sin_family = AF_INET;

    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // code goes here
    std::thread tSnd(sendThread, sockfd);
    std::thread tRec(recThread, sockfd);
    tSnd.join();
    tRec.join();

    sockClose(sockfd);
    sockQuit();

#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
