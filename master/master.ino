//#include <SevenSegmentTM1637.h>

#include <TM1637Display.h>
#include <KtaneI2C.h>

#define DISPLAY_CLK 6
#define DISPLAY_DIO 7
#define BUZZER 4

KtaneI2CMaster I2C;

constexpr int16_t moduleTime = 120;

//SevenSegmentTM1637 display2(DISPLAY_CLK, DISPLAY_DIO);
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
char serial[SerialNumber::Digits];
int32_t startTime;

void setup()
{
  Serial.begin(9600);
  pinMode(OUTPUT, BUZZER);
  //display2.init();
  //display2.print(5.23);

  delay(1000); // Give everything else time to boot
  //I2C.onEvent(onEvent);
  //I2C.begin();
  //Serial.print(I2C.getNumModules());
  startTime = millis();

  for (int i = 0; i < 3; i++)
  {
    tone(BUZZER, 4400, 250);
    delay(300);
    tone(BUZZER, 4150, 75);
    delay(625);
  }

  display.setBrightness(7);
  display.showNumberDec(5);
}

void loop()
{

  //I2C.checkHeartbeat();
  updateTimer();
  delay(10);
}

void onEvent(ModuleEvent event, int8_t moduleIndex)
{
  if (event == ModuleEvent::Strike)
    Serial.print("strike\n");
  else if (event == ModuleEvent::Complete)
    Serial.print("complete\n");
}

void generateSerial(char* outSerial)
{

}

void updateTimer()
{
  int16_t time = moduleTime - ((millis() - startTime) / 1000);
  int16_t displayValue = (100 * (time / 60)) + (time % 60);
}
