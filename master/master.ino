#include <KtaneI2C.h>

KtaneI2CMaster I2C;

char serial[SerialNumber::Digits];
void setup() 
{
  Serial.begin(9600);
  delay(1000); // Give everything else time to boot
  I2C.onEvent(onEvent);
  I2C.begin();
  Serial.print(I2C.getNumModules());
}

void loop()
{
  I2C.checkHeartbeat();
  delay(10000);
}

void onEvent(ModuleEvent event, int8_t moduleIndex)
{
  if(event == ModuleEvent::Strike)
    Serial.print("strike\n");
  else if(event == ModuleEvent::Complete)
    Serial.print("complete\n");
}

void generateSerial(char* outSerial)
{
  
}


