#include <cstring>
#include <iostream>
#include "CRxTxQueue.hpp"
#include "generator.h"


#define SERVER_PROTVERSION     10u
#define _LOCK                  std::lock_guard<std::mutex>  lock(mxLocker);


CRxTxQueue::CRxTxQueue()
: rxQue()
, txQue()
, txQueCapacity(4u)
, protocolVersion(SERVER_PROTVERSION)
, mxLocker()
, Data()
, reqDoubleAmount(1000000u) // should be 1000000u
, randomData()
, bStopped(false)
, maxReqExtDataAmount(5000000u)
, Random_data_byte_ptr(NULL)
, Random_data_sz(0u)
{
    memset(&Data, 0, sizeof(Data));
}


void CRxTxQueue::stop()
{
    _LOCK
    bStopped = true;
}

bool CRxTxQueue::isRunning()
{
    _LOCK    
    return !bStopped;
}

void CRxTxQueue::pushRxQ(const sockaddr_in &adr, const tDatagram &data)
{
    _LOCK
    rxQue.push_back(tQueElem(adr, data));
}


bool CRxTxQueue::popTxQ(sockaddr_in &adr, tDatagram &data)
{
    bool ret = false;
    _LOCK
    if (false == txQue.empty())
    {
        memcpy(&adr, &txQue.front().sockAdr, sizeof(adr));
        memcpy(&data, &txQue.front().data, sizeof(data));
        txQue.pop_front();
        ret = true;
    }
    return ret;
}


void CRxTxQueue::run()
{
    _LOCK
    if ((false == rxQue.empty()) && (txQue.size() < txQueCapacity))
    {
        bool clear = fillupOutPdu();
        txQue.push_back(tQueElem(rxQue.front().sockAdr, Data));
        if (true == clear)
        {// All related packeges have been sent => remove in-PDU and clear buffers
            rxQue.pop_front();
            memset(&Data.hdr, 0, sizeof(Data.hdr));
            randomData.clear();
            Random_data_byte_ptr = NULL;
            Random_data_sz = 0u;
        }
    }
}


bool CRxTxQueue::fillupOutPdu()
{
    tHeader &h = Data.hdr;
    if (0u == h.totalPackage)
    {// New one: init header data
        initOutPdu();
    }
    else if (h.cntPackage < h.totalPackage)
    {
        h.cntPackage++;
        if (h.totalPackage == h.cntPackage)
        {// copy the rest of data
            h.payload_len = Random_data_sz - MAX_PAYLOAD_LEN * ( h.cntPackage - 1);
        }
        const uint32_t shift = (h.cntPackage - 1) * MAX_PAYLOAD_LEN;
        memcpy(&Data.payload, &Random_data_byte_ptr[shift], h.payload_len);
    }
    return (h.totalPackage == h.cntPackage);
}


void CRxTxQueue::initOutPdu()
{
    double x = 0.0;
    eDataType valType = eDataTypeSz;
    uint32_t Amount = 0u;
    Data.hdr.totalPackage = 1u;
    Data.hdr.cntPackage = 1u;
    Data.hdr.protocolVersion = protocolVersion;
    Data.hdr.pduType = eResp;
    Data.hdr.payload_len = 0u;
    Data.hdr.errorStatus = checkPduError(rxQue.front().data, x, valType, Amount);
    
    if (eNoError == Data.hdr.errorStatus) 
    {
        randomData.setVariant(valType);
        Random_data_sz = randomData.getElementSize() * Amount;
        Data.hdr.totalPackage = 1 + Random_data_sz / MAX_PAYLOAD_LEN;
        Data.hdr.payload_len = MAX_PAYLOAD_LEN;
        std::cout << "type = " << valType << ", amout = " << Amount << std::endl;
        switch (valType)
        {
            case eLongDouble:
                generateRandoms(static_cast<long double>(x), Amount, randomData.BufferLongDouble);
                break;

            case eFloat:
                generateRandoms(static_cast<float>(x), Amount, randomData.BufferFloat);
                break;

            default: // double
                generateRandoms(static_cast<double>(x), Amount, randomData.BufferDouble);
                break;
        }
        Random_data_byte_ptr = reinterpret_cast<uint8_t*>(randomData.getRawDataPtr());
        memcpy(&Data.payload, Random_data_byte_ptr, MAX_PAYLOAD_LEN);
    }
}

//  Expected payload data:
// eReq:    <double_x>
// eExtReq: <double_x> <uint32_t_eDataType> <uint32_t_AmountOfData>
uint32_t CRxTxQueue::checkPduError(const tDatagram &dt, double &x, eDataType &tp, uint32_t &amount)
{
    uint32_t ret = eNoError;
    x = *(reinterpret_cast<const double*>(&dt.payload.data[0]));
    tp = eDouble;
    amount = reqDoubleAmount;

    // Common check
    if ((eReq != dt.hdr.pduType) && (eExtReq != dt.hdr.pduType))
    {
        ret = eRequestOnlyExpected;
    }
    if (protocolVersion != dt.hdr.protocolVersion)
    {
        ret = eInvalidProtocolVersion;
    }
    else if (0.0 == x)
    {
        ret = eInvalidPayloadValue;
    } 
    else if (eExtReq == dt.hdr.pduType)
    {// <uint32_t eDataType> <uint32_t amount>
         eDataType tp0 = *(reinterpret_cast<const eDataType*>(&dt.payload.data[sizeof(double)]));
         uint32_t amount0 = *(reinterpret_cast<const uint32_t*>(&dt.payload.data[sizeof(double) + sizeof(uint32_t)]));
         if (tp0 >= eDataTypeSz) ret = eUnexpectedRequestedType;
         else tp = tp0;
         if ((amount0 > maxReqExtDataAmount) || (0u == amount0)) ret = eInvalidRequestedDataAmount;
         else amount = amount0;
    }
    else if (eReq == dt.hdr.pduType)
    {
        if (sizeof(double) != dt.hdr.payload_len)  ret = eInvalidPayloadLen;
    }
    return ret;
}

