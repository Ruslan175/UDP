#ifndef  _CRXTXQUEUE_HPP
#define  _CRXTXQUEUE_HPP

#include <deque>
#include <vector>
#include <mutex>
#include <netinet/in.h>
#include "../CVariant.hpp"


class CRxTxQueue
{
    public:
        CRxTxQueue();
        void pushRxQ(const sockaddr_in &adr, const tDatagram &data);
        bool popTxQ(sockaddr_in &adr, tDatagram &data);
        void stop();
        void run();
        bool isRunning();

    private:
        struct tQueElem
        {
            sockaddr_in sockAdr;
            tDatagram   data;

            tQueElem(const sockaddr_in &a, const tDatagram &d) : sockAdr(a), data(d) {};
        };

        bool fillupOutPdu();
        void initOutPdu();
        void generateDoubles(double mod, uint32_t sz);
        uint32_t checkPduError(const tDatagram &dt, double &x, eDataType &tp, uint32_t &amount);

        std::deque<tQueElem>        rxQue; // recieved from clients
        std::deque<tQueElem>        txQue; // to be sent to clients
        const uint32_t              txQueCapacity; // max out-queue size
        const uint32_t              protocolVersion;   // server protocol version
        std::mutex                  mxLocker;          // Multithread access
        tDatagram                   Data;              // PDU buffer 
        const uint32_t              reqDoubleAmount;   // Default value for request
        CVariant                    randomData;        // Storage of doubles, long doubles, floats
        bool                        bStopped;          // Has work been stopped ?
        const uint32_t              maxReqExtDataAmount;    // Max possible data amount for extended request
        uint8_t *                   Random_data_byte_ptr;   // To point data for a new Tx-package
        uint32_t                    Random_data_sz;         // Total size of Tx-data
};

#endif
