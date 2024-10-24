#ifndef  _CLISTENER_HPP
#define  _CLISTENER_HPP

#include "CRxTxQueue.hpp"
#include "../types.h"

class CListener
{
    public:
        CListener(int port, CRxTxQueue &queue);
        ~CListener();
        void run();
        bool isReady() const;

    private:
        CListener();
    
        int Socket;
        tDatagram Data;
        CRxTxQueue &Queue;
};

#endif
