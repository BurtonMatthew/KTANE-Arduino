#include "BicolourLED.h"
#include <Arduino.h>

BicolourLED::BicolourLED(int8_t redPin, int8_t greenPin) : red(redPin), green(greenPin) 
{ 
    
}

void BicolourLED::begin()
{
    pinMode(red, OUTPUT);
    pinMode(green, OUTPUT);
    write(Colour::Off);
}

void BicolourLED::write(Colour writeColour)
{
    digitalWrite(red, writeColour == Colour::Red ? HIGH : LOW);
    digitalWrite(green, writeColour == Colour::Green ? HIGH : LOW);
}