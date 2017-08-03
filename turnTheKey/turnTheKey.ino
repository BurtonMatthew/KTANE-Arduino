constexpr int8_t segmentPins[8] = {2,3,4,5,6,7,8,9};
constexpr int8_t digitPins[4] = {10,11,12,A1};

constexpr bool segmentsPerDigit[10][7] = { { true,  true,  true,  true,  true,  true,  false },
                                           { false, true,  true,  false, false, false, false },
                                           { true,  true,  false, true,  true,  false, true  },
                                           { true,  true,  true,  true,  false, false, true  },
                                           { false, true,  true,  false, false, true,  true  },
                                           { true,  false, true,  true,  false, true,  true  },
                                           { true,  false, true,  true,  true,  true,  true  },
                                           { true,  true,  true,  false, false, false, false },
                                           { true,  true,  true,  true,  true,  true,  true  },
                                           { true,  true,  true,  true,  false, true,  true  },
                                         };
void setup() 
{
  for(int8_t pin : segmentPins)
    pinMode(pin, OUTPUT);

  for(int8_t pin : digitPins)
    pinMode(pin, OUTPUT);
}

void loop() 
{
  int16_t num = 7491;
  int8_t digits[4];
  for(int8_t i=0; i<4; ++i)
  {
    digits[3-i] = num % 10;
    num /= 10;
  }

  for(int8_t i=0; i<4; ++i)
  {
    for(int8_t j=0; j<4; ++j)
      digitalWrite(digitPins[j], i==j ? HIGH : LOW);
    
    for(int8_t j=0; j<7; ++j)
      digitalWrite(segmentPins[j], segmentsPerDigit[digits[i]][j] ? LOW : HIGH);
    digitalWrite(segmentPins[7], HIGH);
    delay(1);
  }
}
