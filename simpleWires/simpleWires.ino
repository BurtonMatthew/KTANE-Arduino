// Keep Talking and Noboby Explodes Simple Wires Module for Arduino
// By Matthew Burton

#include <BicolourLED.h>
#include <KtaneI2C.h>

#define I2C_ADDRESS 15

#define NUM_WIRES 6
#define WIRE0 A0
#define WIRE1 A1
#define WIRE2 A2
#define WIRE3 A3 // Pins A4 and A5 needed for I2C
#define WIRE4 A6
#define WIRE5 A7

#define LED_RED 11
#define LED_GREEN 12

// Read values for pins using the arduino's internal pullup resistors
#define BLACK_READ 987  // 1M ohm
#define BLUE_READ 971   // 680k
#define RED_READ 950    // 470k
#define YELLOW_READ 913 // 300k
#define WHITE_READ 879  // 220k

// Read values for pins using an external pullup resistor, which may have a different resistance than the internal pullup (I used a 20k for these values)
#define BLACK_READ_EXT 1004  // 1M ohm
#define BLUE_READ_EXT 994   // 680k
#define RED_READ_EXT 982    // 470k
#define YELLOW_READ_EXT 961 // 300k
#define WHITE_READ_EXT 939  // 220k

enum class WireColour : int8_t
{
  None = -1,
  Black = 0,
  Blue,
  Red,
  Yellow,
  White,
  MAX
};

constexpr int16_t readValues_internal[static_cast<size_t>(WireColour::MAX)] = { BLACK_READ, BLUE_READ, RED_READ, YELLOW_READ, WHITE_READ };
constexpr int16_t readValues_external[static_cast<size_t>(WireColour::MAX)] = { BLACK_READ_EXT, BLUE_READ_EXT, RED_READ_EXT, YELLOW_READ_EXT, WHITE_READ_EXT };
constexpr int8_t wirePins[NUM_WIRES] = { WIRE0, WIRE1, WIRE2, WIRE3, WIRE4, WIRE5 };
constexpr const int16_t* readValues[NUM_WIRES] = { readValues_internal, readValues_internal, readValues_internal, readValues_internal, readValues_external, readValues_external };

constexpr int8_t setupResponse = static_cast<int8_t>(RequestSetupInfo::RequestSerial);

BicolourLED led(LED_RED, LED_GREEN);
WireColour lastWires[NUM_WIRES];
int8_t solution;

KtaneI2CSlave I2C;

void setup()
{
  for (int8_t i = 0; i < NUM_WIRES; ++i)
    pinMode(wirePins[i], INPUT_PULLUP);

  led.begin();
  readWires(lastWires);
  solution = findSolution(lastWires, false);

  // Disable the builtin LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  I2C.setSetupResponse(setupResponse);
  I2C.begin(I2C_ADDRESS);
}

void loop()
{
  WireColour currentWires[NUM_WIRES];
  readWires(currentWires);
  
  int8_t cutIndex = getCutIndex(lastWires, currentWires);
  if (cutIndex != -1)
  {
    if (cutIndex == solution)
    {
      led.write(BicolourLED::Colour::Green);
    }
    else
    {
      led.write(BicolourLED::Colour::Red);
      delay(225);
      led.write(BicolourLED::Colour::Off);
    }
    memcpy(lastWires, currentWires, sizeof(lastWires[0]) * NUM_WIRES);
  }
}

void readWires(WireColour* outWires)
{
  for (int8_t i = 0; i < NUM_WIRES; ++i)
  {
    int16_t wire = analogRead(wirePins[i]);
    outWires[i] = WireColour::None;

    int16_t minDist = abs(wire - 1023); 
    for (int j = 0; j < static_cast<int>(WireColour::MAX); ++j)
    {
      int16_t dist = abs(wire - readValues[i][j]);
      if(dist < minDist)
      {
        outWires[i] = static_cast<WireColour>(j);
        minDist = dist;
      }
    }
  }
}

int8_t getCutIndex(WireColour* oldWires, WireColour* newWires)
{
  for (int8_t i = 0; i < NUM_WIRES; ++i)
  {
    if (oldWires[i] != newWires[i])
    {
      return i;
    }
  }

  return -1;
}

int8_t findSolution(WireColour* wires, bool serialOdd)
{
  WireColour condensed[NUM_WIRES];
  int8_t mapping[NUM_WIRES];
  int8_t numWires;
  condenseWires(condensed, mapping, numWires, wires);

  int8_t condensedSolution = -1;
  switch (numWires)
  {
    case 3:
      condensedSolution = findSolutionThreeWires(condensed, serialOdd);
      break;
    case 4:
      condensedSolution = findSolutionFourWires(condensed, serialOdd);
      break;
    case 5:
      condensedSolution = findSolutionFiveWires(condensed, serialOdd);
      break;
    case 6:
      condensedSolution = findSolutionSixWires(condensed, serialOdd);
      break;
  }

  if (condensedSolution != -1)
  {
    return mapping[condensedSolution];
  }
  else
  {
    return -1;
  }
}

void condenseWires(WireColour* outCondensed, int8_t* outMapping, int8_t& outNumWires, WireColour* wires)
{
  outNumWires = 0;
  for (int8_t i = 0; i < NUM_WIRES; ++i)
  {
    if (wires[i] != WireColour::None)
    {
      outCondensed[outNumWires] = wires[i];
      outMapping[outNumWires] = i;
      outNumWires++;
    }
  }

  for (int8_t i = outNumWires; i < NUM_WIRES; ++i)
  {
    outCondensed[i] = WireColour::None;
    outMapping[i] = -1;
  }
}

int8_t findSolutionThreeWires(WireColour* wires, bool serialOdd)
{
  if (countWiresByColour(wires, 3, WireColour::Red) == 0)
    return 1;
  else if (wires[2] == WireColour::White)
    return 2;
  else if (countWiresByColour(wires, 3, WireColour::Blue) > 1)
    return positionOfLastWireByColour(wires, 3, WireColour::Blue);
  else
    return 2;
}

int8_t findSolutionFourWires(WireColour* wires, bool serialOdd)
{
  if (serialOdd && countWiresByColour(wires, 4, WireColour::Red) > 1)
    return positionOfLastWireByColour(wires, 4, WireColour::Red);
  else if (wires[3] == WireColour::Yellow && countWiresByColour(wires, 4, WireColour::Red) == 0)
    return 0;
  else if (countWiresByColour(wires, 4, WireColour::Blue) == 1)
    return 0;
  else if (countWiresByColour(wires, 4, WireColour::Yellow) > 1)
    return 3;
  else
    return 1;
}

int8_t findSolutionFiveWires(WireColour* wires, bool serialOdd)
{
  if (serialOdd && wires[4] == WireColour::Black)
    return 3;
  else if (countWiresByColour(wires, 5, WireColour::Red) == 1 && countWiresByColour(wires, 5, WireColour::Yellow) > 1)
    return 0;
  else if (countWiresByColour(wires, 5, WireColour::Black) == 0)
    return 1;
  else
    return 0;
}

int8_t findSolutionSixWires(WireColour* wires, bool serialOdd)
{
  if (serialOdd && countWiresByColour(wires, 6, WireColour::Yellow) == 0)
    return 2;
  else if (countWiresByColour(wires, 6, WireColour::Yellow) == 1 && countWiresByColour(wires, 6, WireColour::White) > 1)
    return 3;
  else if (countWiresByColour(wires, 6, WireColour::Red) == 0)
    return 5;
  else
    return 3;
}

int8_t countWiresByColour(WireColour* wires, int8_t numWires, WireColour colour)
{
  int8_t result = 0;
  for (int8_t i = 0; i < numWires; ++i)
  {
    if (wires[i] == colour)
      ++result;
  }
  return result;
}

int8_t positionOfLastWireByColour(WireColour* wires, int8_t numWires, WireColour colour)
{
  int8_t result = -1;
  for (int8_t i = 0; i < numWires; ++i)
  {
    if (wires[i] == colour)
      result = i;
  }
  return result;
}

