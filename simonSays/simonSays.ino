// Simon Says

#include <Button.h>

#define RemindDelay 5000
#define RemindFlashGap 200
#define FlashTime 500
#define SequenceLength 4

//Pins
#define ButtonRed 5
#define ButtonBlue 6
#define ButtonGreen 7
#define ButtonYellow 8
#define LightRed 9
#define LightBlue 10
#define LightGreen 11
#define LightYellow 12

enum class Colour
{
  Red,
  Blue,
  Green,
  Yellow,
  Count
};

// [SerialHasValue][Strikes][FlashingColour]
Colour SolutionMappings[2][3][(int)Colour::Count] =
{
  // No Vowel in Serial
  {
    { Colour::Blue, Colour::Yellow, Colour::Green, Colour::Red },
    { Colour::Red, Colour::Blue, Colour::Yellow, Colour::Green },
    { Colour::Yellow, Colour::Green, Colour::Blue, Colour::Red }
  },

  // Vowel in Serial
  {
    { Colour::Blue, Colour::Red, Colour::Yellow, Colour::Green },
    { Colour::Yellow, Colour::Green, Colour::Blue, Colour::Red },
    { Colour::Green, Colour::Red, Colour::Yellow, Colour::Blue }
  }
};

Button Buttons[(int)Colour::Count] =
{
  Button(ButtonRed, PULLUP),
  Button(ButtonBlue, PULLUP),
  Button(ButtonYellow, PULLUP), 
  Button(ButtonGreen, PULLUP)
};

int Lights[(int)Colour::Count] = { LightRed, LightBlue, LightYellow, LightGreen };

unsigned long lastInputTime;
Colour Sequence[SequenceLength];
int SequenceStep;
int UserSequenceStep;

int Strikes = 0;
int Vowel = 0;

bool DoingRemind;
unsigned long RemindStartTime;

void setup() 
{
  for(int i=0; i<(int)Colour::Count; ++i)
  {
    pinMode(Lights[i], OUTPUT);
  }
  startModule();
}

void loop() 
{
  unsigned long Time = millis();
  /*
  if(Buttons[(int)Colour::Red].isPressed())
  {
    lastInputTime = millis();
    digitalWrite(LightRed, HIGH);
  }
  */

  for(int i=0; i<(int)Colour::Count; ++i)
  {
    if(Buttons[i].uniquePress())
    {
      lastInputTime = Time;
      DoingRemind = false;

      // Correct answer?
      if((int) SolutionMappings[Vowel][Strikes][(int)Sequence[UserSequenceStep]] == i)
      {
        UserSequenceStep++;

        // Done with this pattern?
        if(UserSequenceStep == SequenceStep)
        {
          UserSequenceStep = 0;
          SequenceStep++;
          lastInputTime = lastInputTime - RemindDelay + (RemindFlashGap * 3);
        }
      }
      else
      {
        // we got it wrong womp womp
        Strikes++;
        UserSequenceStep = 0;
        lastInputTime = lastInputTime - RemindDelay + (RemindFlashGap * 3);
      }
    }
  }

  // Do remind if its been too long since last input
  if(!DoingRemind && lastInputTime + RemindDelay > Time)
  {
    DoingRemind = true;
    RemindStartTime = Time;
    digitalWrite(LightRed, LOW);
  }

  // Stop Remind if we've finished it
  if(DoingRemind && Time > RemindStartTime + ( SequenceStep * (FlashTime + RemindFlashGap) ) )
  {
    DoingRemind = false;
    lastInputTime = Time;
  }

  // Do Remind flashes
  if(DoingRemind)
  {
    for(int i=0; i<SequenceStep; ++i)
    {
      if(Time > i * (FlashTime + RemindFlashGap) && Time < (i+1) * (FlashTime + RemindFlashGap) - RemindFlashGap)
      {
        digitalWrite(Lights[(int)Sequence[i]], HIGH);
      }
      else
      {
        digitalWrite(Lights[(int)Sequence[i]], LOW);
      }
    }
  }
}

void startModule()
{
  for(int i=0; i<SequenceLength; ++i)
  {
    Sequence[i] = (Colour) random((int) Colour::Count);
  }

  SequenceStep = 1;
  UserSequenceStep = 0;
  lastInputTime = 0;
  DoingRemind = false;
  RemindStartTime = 0;
}

