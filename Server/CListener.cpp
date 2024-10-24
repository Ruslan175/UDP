#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include "CListener.hpp"

#define SERVER_IP   "127.0.0.1"

using namespace std;


CListener::CListener(int port, CRxTxQueue &queue)
: Socket(-1)
, Data()
, Queue(queue)
{
    do
    {
        // Creating socket file descriptor 
        int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverSocket < 0)
        {
            cout << "Server socker creation error: errno = " << errno << endl;
            break; 
        } 
           
        // Filling server information 
        sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr)); 
        servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
        servaddr.sin_port = htons(port); 
           
        // Bind the socket with the server address 
        if (0 != bind(serverSocket, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)))
        {
            cout << "Server bind error: errno = " << errno << endl;
            break;
        }

        Socket = serverSocket;
        memset(&Data, 0, sizeof(Data));
    }
    while (false);
}


void CListener::run()
{
    sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    memset(&cliaddr, 0, sizeof(cliaddr));
    while(1)
    {// Checking port for any data
        int rd = recvfrom(Socket, &Data, sizeof(Data), MSG_DONTWAIT, reinterpret_cast<sockaddr*>(&cliaddr), &len);
        if (rd > 0)
        {// Handle received data
            cout << "New rx-message, client port: " << cliaddr.sin_port << endl;
            Queue.pushRxQ(cliaddr, Data);
        }
        
        // Handle Tx-queue (client acks are not needed as MSG_CONFIRM-flag is used)
        if (true == Queue.popTxQ(cliaddr, Data))
        {
            //cout << "Send to client = " << cliaddr.sin_port << ", cntPackage = " << Data.hdr.cntPackage << endl;
            sendto(Socket, &Data, sizeof(Data), MSG_CONFIRM, reinterpret_cast<sockaddr*>(&cliaddr), sizeof(sockaddr));
        }
        _SLEEP_MS(1)
    }
}


CListener::~CListener()
{
    if (true == isReady()) close(Socket);
    Queue.stop();
}


bool CListener::isReady() const
{
    return (Socket > 0);
}


