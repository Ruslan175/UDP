#ifndef  _CCLIENT_HPP
#define  _CCLIENT_HPP

#include <netinet/in.h>
#include "../types.h"

class CClient
{
    public:
        CClient(uint16_t port, const char *ip);
        ~CClient();
        void handleRequest(double x);
        void handleExtRequest(double x, eDataType tp, uint32_t data_amount);
        bool isReady() const;

    private:
        CClient();
        void receiveDoubles(uint32_t tp);
        void handleErrors(uint32_t err, uint32_t servProtoVer);
        void sendRequest(uint32_t tp);

        int             Socket;
        sockaddr_in     Address;
        const uint32_t  ProtocolVersion;
        tDatagram       Data;

};

#endif
