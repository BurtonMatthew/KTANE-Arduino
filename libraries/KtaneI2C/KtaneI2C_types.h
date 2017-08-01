#ifndef KTANEI2C_TYPES_H
#define KTANEI2C_TYPES_H

enum class WireResult : int8_t
{
    Ok,
    Fail
};

enum class RequestSetupInfo : int8_t
{
    Needy = (1 << 0),
    RequestSerial = (1 << 1),
    RequestIndicators = (1 << 2),
    RequestNumBatteries = (1 << 3),
    RequestPorts = (1 << 4)
};

enum class Indicators : int16_t
{
    SND = (1 << 0),
    CLR = (1 << 1),
    CAR = (1 << 2),
    IND = (1 << 3),
    FRQ = (1 << 4),
    SIG = (1 << 5),
    NSA = (1 << 6),
    MSA = (1 << 7),
    TRN = (1 << 8),
    BOB = (1 << 9),
    FRK = (1 << 10),
};

enum class Ports : int8_t
{
    DviD = (1 << 0),
    Parallel = (1 << 1),
    PS2 = (1 << 2),
    RJ45 = (1 << 3),
    Serial = (1 << 4),
    StereoRCA = (1 << 5),
};

struct RequestSetupResponse
{
    int8_t requestSetupInfo;
};

enum class MessageType : int8_t
{
    SendSerial
};

struct SerialNumber
{
    static constexpr int8_t Digits = 6;
    char serial[Digits];
};

constexpr int8_t MessageSizes[] =
{
    [MessageType::SendSerial] = sizeof(SerialNumber)
};

enum class ModuleEvent : int8_t
{
    None,
    Complete,
    Strike
};

#endif