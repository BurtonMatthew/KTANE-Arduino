#ifndef KTANEI2C_SLAVE_H
#define KTANEI2C_SLAVE_H

#include <stdint.h>
enum class MessageType : int8_t;
enum class ModuleEvent : int8_t;

class KtaneI2CSlave
{
    public:
        typedef void (*messageCallback)(MessageType, char*);

        KtaneI2CSlave();
        // Must be in the range of [1-127)
        void begin(int8_t address);
        void setSetupResponse(int8_t response) { setupResponse = response; }
        void onReceiveMessage(messageCallback callback) { receiveMessageCallback = callback; }
        void onRequest();
        void onReceive(int numBytes);
        void writeQueue(ModuleEvent event);        
    private:
        int8_t setupResponse;
        bool sentSetupResponse;
        messageCallback receiveMessageCallback;

        // Ringbuffer queue for module events
        static constexpr int8_t eventQueueSize = 5;
        ModuleEvent eventQueue[eventQueueSize];
        int8_t queueReadIndex, queueWriteIndex;
};

#endif