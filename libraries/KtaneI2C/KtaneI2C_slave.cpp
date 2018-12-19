#include "KtaneI2C_slave.h"
#include "KtaneI2C_types.h"
#include <Arduino.h>
#include <Wire.h>
#include <alloca.h>

KtaneI2CSlave* instance;
void onRequest() { instance->onRequest(); }
void onReceive(int numBytes) { instance->onReceive(numBytes); }

KtaneI2CSlave::KtaneI2CSlave()
    : setupResponse(0)
    , sentSetupResponse(false)
    , receiveMessageCallback(nullptr)
    , eventQueue{ModuleEvent::None}
    , queueReadIndex(0)
    , queueWriteIndex(0)
{
    instance = this;
}

void KtaneI2CSlave::begin(int8_t address)
{
    Wire.onRequest(::onRequest);
    Wire.onReceive(::onReceive);
    Wire.begin(address);
}

void KtaneI2CSlave::onRequest()
{
    // If this is the first request, master is looking for setup information
    if(!sentSetupResponse)
    {
        RequestSetupResponse response;
        response.requestSetupInfo = setupResponse;
        Wire.write(reinterpret_cast<byte*>(&response), sizeof(response));
    }
    // Otherwise this is a heartbeat, if we have queue data send it
    else if(queueReadIndex != queueWriteIndex)
    {
        Wire.write(static_cast<byte>(eventQueue[queueReadIndex++]));
        if(queueReadIndex == eventQueueSize)
            queueReadIndex = 0;
    }
    else
    {
        Wire.write(static_cast<byte>(ModuleEvent::None));
    }
}

void KtaneI2CSlave::onReceive(int numBytes)
{
    while(numBytes > 0)
    {
        MessageType messageType = static_cast<MessageType>(Wire.read());
        --numBytes;
        int8_t messageSize = MessageSizes[static_cast<int8_t>(messageType)];
        {
            char* messageData = static_cast<char*>(alloca(messageSize));
            for(int8_t i=0; i<messageSize; ++i)
            {
                messageData[i] = Wire.read();
            }
            numBytes -= messageSize;
            if(receiveMessageCallback)
                receiveMessageCallback(messageType, messageData);
        }
    }
}

void KtaneI2CSlave::writeQueue(ModuleEvent event)
{
    eventQueue[queueWriteIndex++] = event;
    if(queueWriteIndex == eventQueueSize)
        queueWriteIndex = 0;
}