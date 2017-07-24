#include <KtaneI2C.h>

KtaneI2CMaster I2C;

char serial[BombInfo::SerialDigits];
void setup() 
{
  Serial.begin(9600);
  delay(1000); // Give everything else time to boot
  I2C.begin();
  Serial.print(I2C.getNumModules());
}

void loop()
{
  // put your main code here, to run repeatedly:

}

void generateSerial(char* outSerial)
{
  
}


