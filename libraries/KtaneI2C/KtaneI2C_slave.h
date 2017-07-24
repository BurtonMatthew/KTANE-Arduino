#ifndef KTANEI2C_SLAVE_H
#define KTANEI2C_SLAVE_H

#include <stdint.h>

class KtaneI2CSlave
{
    public:
        typedef void (*requestCallback)();

        KtaneI2CSlave();
        // Must be in the range of [1-127)
        void begin(int8_t address);
        void setSetupResponse(int8_t response) { setupResponse = response; }
        void onRequest();        
    private:
        int8_t setupResponse;
        bool sentSetupResponse;
};

#endif