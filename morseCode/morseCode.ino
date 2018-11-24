#include <BicolourLED.h>

/** TYPES **/
#define MORSE_LED 5
#define BUTTON_INPUT 8
#define LED_RED 11
#define LED_GREEN 12
#define POT_INPUT A6

template <class T, size_t N>
constexpr size_t size(const T (&array)[N]) noexcept
{
    return N;
}

int8_t clamp(int8_t value, int8_t lhs, int8_t rhs)
{
    return ((value < lhs) ? lhs : ((value > rhs) ? rhs : value));
}

enum class Morse : uint8_t
{
  None,
  Dot,
  Dash  
};

/** DATA **/
constexpr Morse codes[26][4] =
{
  { Morse::Dot,  Morse::Dash, Morse::None, Morse::None }, // A
  { Morse::Dash, Morse::Dot,  Morse::Dot,  Morse::Dot  }, // B
  { Morse::Dash, Morse::Dot,  Morse::Dash, Morse::Dot  }, // C
  { Morse::Dash, Morse::Dot , Morse::Dot,  Morse::None }, // D
  { Morse::Dot,  Morse::None, Morse::None, Morse::None }, // E
  { Morse::Dot,  Morse::Dot,  Morse::Dash, Morse::Dot  }, // F
  { Morse::Dash, Morse::Dash, Morse::Dot,  Morse::None }, // G
  { Morse::Dot,  Morse::Dot,  Morse::Dot,  Morse::Dot  }, // H
  { Morse::Dot,  Morse::Dot,  Morse::None, Morse::None }, // I
  { Morse::Dot,  Morse::Dash, Morse::Dash, Morse::Dash }, // J
  { Morse::Dash, Morse::Dot,  Morse::Dash, Morse::None }, // K
  { Morse::Dot,  Morse::Dash, Morse::Dot,  Morse::Dot  }, // L
  { Morse::Dash, Morse::Dash, Morse::None, Morse::None }, // M
  { Morse::Dash, Morse::Dot,  Morse::None, Morse::None }, // N
  { Morse::Dash, Morse::Dash, Morse::Dash, Morse::None }, // O
  { Morse::Dot,  Morse::Dash, Morse::Dash, Morse::Dot  }, // P
  { Morse::Dash, Morse::Dash, Morse::Dot,  Morse::Dash }, // Q
  { Morse::Dot,  Morse::Dash, Morse::Dot,  Morse::None }, // R
  { Morse::Dot,  Morse::Dot,  Morse::Dot,  Morse::None }, // S
  { Morse::Dash, Morse::None, Morse::None, Morse::None }, // T
  { Morse::Dot,  Morse::Dot,  Morse::Dash, Morse::None }, // U
  { Morse::Dot,  Morse::Dot,  Morse::Dot,  Morse::Dash }, // V
  { Morse::Dot,  Morse::Dash, Morse::Dash, Morse::None }, // W
  { Morse::Dash, Morse::Dot,  Morse::Dot,  Morse::Dash }, // X
  { Morse::Dash, Morse::Dot,  Morse::Dash, Morse::Dash }, // Y
  { Morse::Dash, Morse::Dash, Morse::Dot,  Morse::Dot  }  // Z
};

constexpr const char* words[] = { "SHELL", "HALLS", "SLICK", "TRICK", 
                                  "BOXES", "LEAKS", "STROBE", "BISTRO",
                                  "FLICK", "BOMBS", "BREAK", "BRICK",
                                  "STEAK", "STING", "VECTOR", "BEATS" };

constexpr float freqs[] = { 3.505f, 3.515f, 3.522f, 3.532f,
                            3.535f, 3.542f, 3.545f, 3.552f,
                            3.555f, 3.565f, 3.572f, 3.575f,
                            3.582f, 3.592f, 3.595f, 3.600f };

constexpr size_t wordCount = size(words);

static_assert(size(words) == size(freqs), "There should be one frequency per word");

// Morse Code Timings
constexpr uint32_t dotDuration = 600;
constexpr uint32_t dashDuration = 3 * dotDuration;
constexpr uint32_t symbolSpaceDuration = dotDuration;
constexpr uint32_t letterSpaceDuration = 3 * dotDuration;
constexpr uint32_t wordSpaceDuration = 7 * dotDuration;

constexpr uint32_t displayThrottle = 70;

constexpr uint32_t failLedDisplayDuration = 600;

/** RUNTIME VARS **/
BicolourLED resultLed(LED_RED, LED_GREEN);
uint32_t nextSymbolTime;
uint32_t nextDisplayTime;
uint32_t failLedOffTime;
int8_t selectedWordIndex;
int8_t nextSymbolIndex;
int8_t nextLetterIndex;
int8_t lastDisplayIndex;
int8_t prevButtonVal;
bool morseLit;
bool completed;

void setup() 
{
  pinMode(MORSE_LED, OUTPUT);
  pinMode(POT_INPUT, INPUT);
  pinMode(BUTTON_INPUT, INPUT_PULLUP);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  resultLed.begin();
  initModule();
}

void initModule()
{
  selectedWordIndex = random(wordCount);
  //Serial.write(words[selectedWordIndex]);
  //Serial.write("\n");
  nextSymbolTime = 0;
  nextSymbolIndex = 0;
  nextLetterIndex = 0;
  lastDisplayIndex = -1;
  nextDisplayTime = 0;
  failLedOffTime = 0;
  morseLit = false;
}

void loop() 
{
  if(!completed)
  {
    checkSubmit();
    updateDisplay();
    updateFlash();
  }
}

void checkSubmit()
{
  int8_t buttonVal = digitalRead(BUTTON_INPUT);
  if(buttonVal == LOW && prevButtonVal == HIGH)
  {
    if(lastDisplayIndex == selectedWordIndex)
    {
      resultLed.write(BicolourLED::Colour::Green);
      digitalWrite(MORSE_LED, LOW);
      completed = true;
    }
    else
    {
      resultLed.write(BicolourLED::Colour::Red);
      failLedOffTime = millis() + failLedDisplayDuration;
    }
  }
  else if(millis() > failLedOffTime)
  {
    resultLed.write(BicolourLED::Colour::Off);
  }
  prevButtonVal = buttonVal;
}

void updateDisplay()
{
  int16_t rawValue = analogRead(POT_INPUT);
  float bucketSize = 1024.f / static_cast<float>(wordCount);
  int8_t index = clamp(static_cast<int8_t>(rawValue / bucketSize), 0, wordCount-1);
  float remainder = fmod(rawValue, bucketSize);

  if(index != lastDisplayIndex && remainder > bucketSize * 0.05f && remainder < bucketSize * 0.95f &&  millis() > nextDisplayTime)
  {
    lastDisplayIndex = index;
    nextDisplayTime = millis() + displayThrottle;
    Serial.print(index, DEC);
    Serial.write("\n");
  }
}

void updateFlash()
{
  if(millis() < nextSymbolTime)
    return;

  const char* selectedWord = words[selectedWordIndex];
  if(!morseLit)
  {
    // Light the light
    digitalWrite(MORSE_LED, HIGH);
    morseLit = true;

    // Set up light duration
    switch(codes[selectedWord[nextLetterIndex] - 'A'][nextSymbolIndex])
    {
      case Morse::Dot:
        nextSymbolTime = millis() + dotDuration;
        break;
      case Morse::Dash:
        nextSymbolTime = millis() + dashDuration;
        break;
    };
  }
  else
  {
    // Turn off the light
    digitalWrite(MORSE_LED, LOW);
    morseLit = false;
 
    // Advance symbol
    ++nextSymbolIndex;
    if(nextSymbolIndex < 4 && codes[selectedWord[nextLetterIndex] - 'A'][nextSymbolIndex] != Morse::None)
    {
      nextSymbolTime = millis() + symbolSpaceDuration;
    }
    else // done with symbols, go to next letter
    {
      nextSymbolIndex = 0;
      ++nextLetterIndex;
      if(selectedWord[nextLetterIndex] != '\0')
      {
        nextSymbolTime = millis() + letterSpaceDuration;
      }
      else // done with letters, go to start of word
      {
        nextLetterIndex = 0;
        nextSymbolTime = millis() + wordSpaceDuration;
      }
    }
  }
}

