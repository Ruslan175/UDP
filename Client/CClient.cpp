#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <fstream>
#include "../CVariant.hpp"
#include "CClient.hpp"

#define  CLIENT_PROTVERSION     10u
#define  CLIENT_LOGS            "client.log"
#define  REQUSTED_DATA_LOG      "req_data.bin"
#define _LOG_ERROR(_A)  case _A:  log << #_A; break;


using namespace std;


CClient::CClient(uint16_t port, const char *ip)
: Socket(-1)
, Address()
, ProtocolVersion( CLIENT_PROTVERSION )
, Data()
{
    do
    {
        int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

        if (clientSocket < 0)
        {
            cout << "Client socker: creation error " << endl;
            break;
        }

        // specifying server address
        memset(&Address, 0, sizeof(Address));
        Address.sin_family = AF_INET;
        Address.sin_port = htons(port);
        Address.sin_addr.s_addr = inet_addr(ip);

        Socket = clientSocket;
        memset(&Data, 0, sizeof(Data));
    }
    while (false);
}


CClient::~CClient()
{
    if (true == isReady()) close(Socket); 
}


void CClient::handleRequest(double x)
{
    cout << endl << "Client: sending request" << endl;
    Data.hdr.pduType = eReq;
    Data.hdr.payload_len = sizeof(x);
    memcpy(&Data.payload.data, &x, sizeof(x));
    sendRequest(eDouble);
}


bool CClient::isReady() const
{
    return (Socket > 0);
}


void CClient::handleErrors(uint32_t err, uint32_t servProtoVer)
{ // Write error to log-file
    ofstream log;
    log.open(CLIENT_LOGS, ios::app);
    if (false == log.is_open())
    {
        cout << "Log file " << CLIENT_LOGS << " cannot be open" << endl;
        return;
    }

    // Store a timestamp
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    string str(ctime(&t));
    str.pop_back(); // remove '\n'
    log << str << " : ";

    // Store error type
    switch(Data.hdr.errorStatus)
    {
        case eNoError:  log << "No error detected"; break;
        _LOG_ERROR(eInvalidProtocolVersion)
        _LOG_ERROR(eInvalidPayloadLen)
        _LOG_ERROR(eInvalidPayloadValue)
        _LOG_ERROR(eRequestOnlyExpected)
        _LOG_ERROR(eUnexpectedRequestedType)
        _LOG_ERROR(eInvalidRequestedDataAmount)
        default:    log << "eUdefinedError";    break;
    }
    log << ", server protocol version = " << servProtoVer << endl;
    log.close();
}


void CClient::receiveDoubles(uint32_t tp)
{
    cout << "Client: waiting for data array ..." << endl;

    char * ptr_raw_data = NULL;
    uint32_t raw_data_len = Data.hdr.totalPackage * MAX_PAYLOAD_LEN;
    CVariant varBuffer(static_cast<eDataType>(tp));

    // Select correct size
    varBuffer.resize(raw_data_len);
    ptr_raw_data = varBuffer.getRawDataPtr();

    // Copy the 1st package
    memcpy(ptr_raw_data, &Data.payload, Data.hdr.payload_len);
    uint32_t sz = Data.hdr.payload_len;
    socklen_t len;
    do
    {// Receive other packeges
        recvfrom(Socket, &Data, sizeof(Data), MSG_WAITALL, reinterpret_cast<sockaddr*>(&Address), &len);
        memcpy(&(ptr_raw_data[sz]), &Data.payload, Data.hdr.payload_len);
        sz += Data.hdr.payload_len;
    }
    while (Data.hdr.totalPackage > Data.hdr.cntPackage);
    cout << "Total received bytes = " << sz << endl;

    varBuffer.resize(sz);
    varBuffer.sort();
    varBuffer.show1stLast();

    // Storing to file
    ofstream out(REQUSTED_DATA_LOG, ios::out | ios::binary);
    if (true == out.is_open())
    {
        out.write(ptr_raw_data, sz);
        out.close();
    }
    else 
    {
        cout << "Out file " << REQUSTED_DATA_LOG << " cannot be open" << endl;
    }

    // Clear datagram
    memset(&Data, 0, sizeof(Data));
}


void CClient::handleExtRequest(double x, eDataType tp, uint32_t data_amount)
{
    cout << endl <<  "Client: sending extended request" << endl;
    Data.hdr.pduType = eExtReq;
    Data.hdr.payload_len = sizeof(x) + 2 * sizeof(uint32_t);
    memcpy(&Data.payload.data, &x, sizeof(x));
    uint32_t val = (tp < eDataTypeSz) ? tp : eDouble;
    memcpy(&Data.payload.data[sizeof(x)], &val, sizeof(val));
    memcpy(&Data.payload.data[sizeof(x) + sizeof(val)], &data_amount, sizeof(data_amount));
    sendRequest(val);
}


void CClient::sendRequest(uint32_t tp)
{
    Data.hdr.totalPackage = 1u;
    Data.hdr.cntPackage = 1u;
    Data.hdr.protocolVersion = ProtocolVersion;
    Data.hdr.errorStatus     = eNoError;
 
    // Send request to server
    _SLEEP_MS(3000) // wait 3s to send request
    sendto(Socket, &Data, sizeof(Data), MSG_CONFIRM, reinterpret_cast<sockaddr*>(&Address), sizeof(Address));

    // Expect  response from server
    socklen_t len;
    do
    {
        if (recvfrom(Socket, &Data, sizeof(Data), MSG_WAITALL, reinterpret_cast<sockaddr*>(&Address), &len) < 0)
        {
            cout << "Client: recvfrom() error, errno = " << errno << endl;
            break;
        }

        if (eNoError == Data.hdr.errorStatus)
        {
            receiveDoubles(tp);
        }
        else 
        { // Store error and exit
            handleErrors(Data.hdr.errorStatus, Data.hdr.protocolVersion);
        }
    }
    while(false);
}
