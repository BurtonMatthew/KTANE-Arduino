#ifndef BICOLOURLED_H
#define BICOLOURLED_H

#include <stdint.h>

class BicolourLED
{
    public:
        enum class Colour : int8_t
        {
            Off,
            Red,
            Green
        };
        BicolourLED(int8_t redPin, int8_t greenPin);
        void begin();
        void write(Colour writeColour);
    private:
        int8_t red, green;
};

#endif