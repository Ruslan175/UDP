#ifndef  _TYPES_HPP
#define  _TYPES_HPP

#include <cstdint>

#define  _SLEEP_MS(_x)    std::this_thread::sleep_for(std::chrono::milliseconds(_x));
#define   MAX_PAYLOAD_LEN     2048u


enum ePduType
{
    eReq = 0,
    eExtReq,
    eResp,
    eAck,   // not used if MSG_CONFIRM is applied for sending
    ePduTypeSz 
};

enum eError
{
    eNoError = 0,   // OK
    eInvalidProtocolVersion,
    eInvalidPayloadLen,
    eInvalidPayloadValue,
    eRequestOnlyExpected,
    eUnexpectedRequestedType,
    eInvalidRequestedDataAmount,
    eUdefinedError,
    eErrorSz 
};

enum eDataType
{// For eExtReq
    eDouble = 0,
    eLongDouble,
    eFloat,
    eDataTypeSz
};

struct tHeader
{
    uint32_t   totalPackage;      // Expected total number of packages
    uint32_t   cntPackage;        // Number of the current package
    uint32_t   protocolVersion;   // Protocol version of the sender
    uint32_t   pduType;           // Message type
    uint32_t   errorStatus;       // No_error / Error
    uint32_t   payload_len;       // size of transfered user data
};

struct tPayload
{
    uint8_t    data[MAX_PAYLOAD_LEN];   // transfered user data
};

struct tDatagram
{
    tHeader hdr;
    tPayload payload;
};

#endif
