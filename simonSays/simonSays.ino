// Simon Says
#include <BicolourLED.h>
#include <Button.h>

#define RemindDelay 5000
#define RemindFlashGap 200
#define FlashTime 500
#define SequenceLength 4

constexpr uint32_t failLedDisplayDuration = 300;

//Pins
#define LED_RED 3
#define LED_GREEN 4
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
  Red = 0,
  Blue = 1,
  Green = 2,
  Yellow = 3,
  Count
};

// [SerialHasVowel][Strikes][FlashingColour]
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
  Button(ButtonRed, INPUT_PULLUP),
  Button(ButtonBlue, INPUT_PULLUP),
  Button(ButtonGreen, INPUT_PULLUP), 
  Button(ButtonYellow, INPUT_PULLUP)
};

int Lights[(int)Colour::Count] = { LightRed, LightBlue, LightGreen, LightYellow };
BicolourLED resultLed(LED_RED, LED_GREEN);


Colour Sequence[SequenceLength];
uint32_t ErrorFlashTime = 0;
uint8_t Stage = 0;
uint8_t PressedButton = 255;
uint32_t LastButtonPressTime[4] = {0};

enum class State
{
  Setup,
  Idle,
  Remind,
  Input,
  Done
};

void ChangeState(State nextState);

bool CheckButtonPressed()
{
  PressedButton = 255;
  for(uint8_t i=0; i<(int)Colour::Count; ++i)
  {
    if(Buttons[i].uniquePress() && )
    {
      if(LastButtonPressTime[i] + 150 < millis())
      {
        PressedButton = i;
      }
      LastButtonPressTime[i] = millis();
    }
  }

  return PressedButton < 255;
}

class StateSetup
{
  public:
    static void Enter() 
    {
      Stage = 1;
      for(int i=0; i<SequenceLength; ++i)
      {
        Sequence[i] = (Colour) random((int) Colour::Count);
      }
    }
    static void Exit()  {}
    static void Update() 
    {
      ChangeState(State::Idle);
    }
  private:
};

class StateIdle
{
  public:
    static void Enter() 
    {
      enterTime = millis();
    }
    static void Exit() {}
    static void Update()
    {
      uint32_t time = millis();
      if(CheckButtonPressed())
      {
        ChangeState(State::Input);
      }
      else if(time > enterTime + RemindDelay)
      {
        ChangeState(State::Remind);
      }
    }
  private:
   static uint32_t enterTime;
};
uint32_t StateIdle::enterTime = 0;

class StateRemind
{
  public:
    static void Enter() 
    {
      enterTime = millis();
      
    }
    static void Exit() 
    {
      for(uint8_t i = 0; i<(int)Colour::Count; ++i)
      {
        digitalWrite(Lights[i], LOW);
      }
    }
    static void Update()
    {
      uint32_t seqTime = millis() - enterTime;
      uint32_t step = seqTime / (FlashTime + RemindFlashGap);
      uint32_t stepTime = seqTime % (FlashTime + RemindFlashGap);
      
      if(CheckButtonPressed())
      {
        ChangeState(State::Input);
      }
      else if(step >= Stage)
      {
        ChangeState(State::Idle);
      }
      else
      {
        digitalWrite(Lights[(int)Sequence[step]], stepTime < FlashTime);
      }
    }
    private:
      static uint32_t enterTime;
};
uint32_t StateRemind::enterTime = 0;

class StateInput
{
  public:
    static void Enter() 
    { 
      inputStage = 0;
    }
    static void Exit() 
    {
      for(uint8_t i = 0; i<(int)Colour::Count; ++i)
      {
        digitalWrite(Lights[i], LOW);
      }
    }
    static void Update()
    {
      // Set LEDs
      for(uint8_t i = 0; i < (int)Colour::Count; ++i)
      {
        digitalWrite(Lights[i], !digitalRead(ButtonRed + i));
      }

      if(PressedButton < 255)
      {
        if((uint8_t)SolutionMappings[0][0][(int)Sequence[inputStage]] == PressedButton)
        {
          ++inputStage;
          if(inputStage >= Stage)
          {
            ++Stage;
            if(Stage > SequenceLength)
            {
              ChangeState(State::Done);
            }
            else
            {
              ChangeState(State::Idle);
            }
          }
        }
        else
        {
            ErrorFlashTime = millis();
            resultLed.write(BicolourLED::Colour::Red);
            ChangeState(State::Idle);
        }
      }

      CheckButtonPressed();
    }
  private:
    static uint8_t inputStage;
};
uint8_t StateInput::inputStage = 0;

class StateDone
{
  public:
    static void Enter()
    {
      resultLed.write(BicolourLED::Colour::Green);
    }
    static void Exit() {}
    static void Update() 
    {
      // Todo power saving sleep
      delay(1000);
    }
};

State CurrentState = State::Idle;

void ChangeState(State nextState)
{
  switch(CurrentState)
  {
    case State::Setup:
      StateSetup::Exit();
      break;
    case State::Idle:
      StateIdle::Exit();
      break;
    case State::Remind:
      StateRemind::Exit();
      break;
    case State::Input:
      StateInput::Exit();
      break;
    case State::Done:
      StateDone::Exit();
      break;
  }

  switch(nextState)
  {
    case State::Setup:
      StateSetup::Enter();
      break;
    case State::Idle:
      StateIdle::Enter();
      break;
    case State::Remind:
      StateRemind::Enter();
      break;
    case State::Input:
      StateInput::Enter();
      break;
    case State::Done:
      StateDone::Enter();
      break;
  }

  CurrentState = nextState;
}

void setup()
{
  int seed = 0;
  for(int i=0; i<100; ++i)
  {
    seed += analogRead(0);
  }
  randomSeed(seed);

  for(int i=0; i<(int)Colour::Count; ++i)
  {
    pinMode(Lights[i], OUTPUT);
  }
  resultLed.begin();

  ChangeState(State::Setup);
}

void loop()
{
  switch(CurrentState)
  {
    case State::Setup:
      StateSetup::Update();
      break;
    case State::Idle:
      StateIdle::Update();
      break;
    case State::Remind:
      StateRemind::Update();
      break;
    case State::Input:
      StateInput::Update();
      break;
    case State::Done:
      StateDone::Update();
      break;
  }

  if(CurrentState != State::Done && millis() > ErrorFlashTime + failLedDisplayDuration)
  {
    resultLed.write(BicolourLED::Colour::Off);
  }
}