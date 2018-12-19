#include "KtaneI2C_master.h"
#include <Arduino.h>
#include <Wire.h>

KtaneI2CMaster::KtaneI2CMaster() 
    : numModules(0)
    , receiveEventCallback(nullptr)
{

}

void KtaneI2CMaster::begin()
{
    // Init the wire library
    Wire.begin();

    // Scan for connected modules
    for(int8_t addr=1; addr<127; ++addr)
    {
        Wire.beginTransmission(addr);
        if(Wire.endTransmission() == 0)
        {
            modules[numModules].address = addr;
            ++numModules;
        }
    }

    for(int8_t i=0; i<numModules; ++i)
    {
        RequestSetupResponse response;
        if(requestData(reinterpret_cast<char*>(&response), sizeof(response), modules[i].address) == WireResult::Ok)
        {
            Wire.beginTransmission(modules[i].address);
            if(response.requestSetupInfo & static_cast<int8_t>(RequestSetupInfo::RequestSerial))
            {
                SerialNumber serial = {{'A','B','C','D','E','1'}};
                Wire.write(static_cast<int8_t>(MessageType::SendSerial));
                Wire.write(reinterpret_cast<char*>(&serial), sizeof(serial));
            }
            if(response.requestSetupInfo & static_cast<int8_t>(RequestSetupInfo::RequestTime))
            {
                
            }
            Wire.endTransmission();
        }
    }
}

void KtaneI2CMaster::checkHeartbeat()
{
    ModuleEvent event;
    for(int8_t i=0; i<numModules; ++i)
    {
        while(requestData(reinterpret_cast<char*>(&event), sizeof(event), modules[i].address) == WireResult::Ok 
                && event != ModuleEvent::None)
        {
            receiveEventCallback(event, i);
        }
    }
}

WireResult KtaneI2CMaster::requestData(char* outResult, int8_t size, int8_t address)
{
    int8_t bytesRead = 0;
    if(Wire.requestFrom(address, sizeof(RequestSetupResponse)) == size)
    {
        while(Wire.available())
        {
            outResult[bytesRead] = Wire.read();
            ++bytesRead;
        }
    }

    return bytesRead == size ? WireResult::Ok : WireResult::Fail;
}