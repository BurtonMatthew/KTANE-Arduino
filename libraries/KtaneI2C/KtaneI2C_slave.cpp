#include "KtaneI2C_slave.h"
#include "KtaneI2C_types.h"
#include <Arduino.h>
#include <Wire.h>

KtaneI2CSlave* instance;
void onRequest() { instance->onRequest(); }

KtaneI2CSlave::KtaneI2CSlave()
    : setupResponse(0)
    , sentSetupResponse(false)
{
    instance = this;
}

void KtaneI2CSlave::begin(int8_t address)
{
    Wire.onRequest(::onRequest);
    Wire.begin(address);
}

void KtaneI2CSlave::onRequest()
{
    if(!sentSetupResponse)
    {
        RequestSetupResponse response;
        response.requestSetupInfo = setupResponse;
        Wire.write(reinterpret_cast<byte*>(&response), sizeof(response));
    }
    else
    {
        //requestHeartbeatCallback();
    }
}