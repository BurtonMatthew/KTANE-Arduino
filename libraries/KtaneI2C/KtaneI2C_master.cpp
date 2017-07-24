#include "KtaneI2C_master.h"
#include <Arduino.h>
#include <Wire.h>

KtaneI2CMaster::KtaneI2CMaster() 
    : numModules(0)
{

}

void KtaneI2CMaster::begin()
{
    // Init the wire library
    Wire.begin();

    // Scan for connected modules
    for(int8_t addr = 1; addr < 127; ++addr)
    {
        Wire.beginTransmission(addr);
        if(Wire.endTransmission() == 0)
        {
            modules[numModules].address = addr;
            ++numModules;
        }
    }
}
