#ifndef KTANEI2C_MASTER_H
#define KTANEI2C_MASTER_H

#include <stdint.h>
#include "KtaneI2c_types.h"

class KtaneI2CMaster
{
    public:
        KtaneI2CMaster();
        void begin();
        int8_t getNumModules() const { return numModules; }
    private:
        struct Module
        {
            int8_t address;
        };

        WireResult requestData(char* outResult, int8_t size, int8_t address);

        static constexpr int8_t MAX_MODULES = 11;
        int8_t numModules;
        Module modules[MAX_MODULES];
};

#endif