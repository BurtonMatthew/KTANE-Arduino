#include <TM1637Display.h>
#include <Button.h>

// Pins
constexpr int8_t ButtonPins[] = {1,0,2,3};
constexpr int8_t SevenSegmentPins[] = {4,5,6,7,8,9,10};
constexpr int8_t DisplayCLK = 11;
constexpr int8_t DisplayDIO = 12;
constexpr int8_t StageLEDs[] = {13,14,15,16};
constexpr int8_t VictoryLED = 17;

// Vars
TM1637Display displayLabels(DisplayCLK, DisplayDIO);
Button buttons[] =
{
    Button(ButtonPins[0]),
    Button(ButtonPins[1]),
    Button(ButtonPins[2]), 
    Button(ButtonPins[3])
};
int8_t curStage = 0;
int8_t display = 0;
int8_t labels[4] = {1,2,3,4};

int8_t pastLabels[4] = {0};
int8_t pastPositions[4] = {0};
int8_t answer = 0;
bool done = false;

void setup() 
{
    randomSeed(analogRead(A7));

    displayLabels.setBrightness(4);

    for(int8_t i=0; i<7; ++i)
    {
        pinMode(SevenSegmentPins[i], OUTPUT);
        digitalWrite(SevenSegmentPins[i], LOW);
    }

    for(int8_t i=0; i<4; ++i)
    {
        pinMode(StageLEDs[i], OUTPUT);
        digitalWrite(StageLEDs[i], LOW);
    }

    beginStage();
}

void loop() 
{
    if(!done)
    {
        for(int8_t i = 0; i<4; ++i)
        {
            if(buttons[i].uniquePress())
            {
                pressPosition(i);
            }
        }
    }
}

void beginStage()
{
    curStage++;
    display = random(4) + 1;
    fisherYates(labels, 4);

    answer = correctPosition();

    drawDisplay();
    drawLabels();
    drawStage();
}

void drawDisplay()
{
    constexpr bool segments[4][7] = 
    {
        {false, true, true, false, false, false, false},
        {true, true, false, true, true, false, true},
        {true, true, true, true, false, false, true},
        {false, true, true, false, false, true, true},
    };

    for(int8_t i=0; i<7; ++i)
    {
        digitalWrite(SevenSegmentPins[i], segments[display-1][i] ? HIGH : LOW);
    }
}


void drawLabels()
{
    int16_t sumLabels = 0;
    for(int8_t i=0; i<4; ++i)
    {
        sumLabels *= 10;
        sumLabels += labels[i];
    }

    displayLabels.showNumberDec(sumLabels);
}

void drawStage()
{
    for(int8_t i=0; i<4; ++i)
    {
        digitalWrite(StageLEDs[i], curStage-1 > i ? HIGH : LOW);
    }
}

void pressPosition(int8_t position)
{
    if(position == answer)
    {
        if(curStage == 5)
        {
            setComplete();
        }
        else
        {
            pastPositions[curStage-1] = position;
            pastLabels[curStage-1] = labels[position];
        }
    }
    else
    {
        curStage = 0;
    }

    if(!done)
        beginStage();
}

int8_t correctPosition()
{
    switch(curStage)
    {
        case 1:
        {
            switch(display)
            {
                case 1:
                case 2:
                    return position(2);
                case 3:
                    return position(3);
                case 4:
                    return position(4);
                
            }
        }
        case 2:
        {
            switch(display)
            {
                case 1:
                    return positionOfLabel(4);
                case 2:
                    return pastPositions[stage(1)];
                case 3:
                    return position(1);
                case 4:
                    return pastPositions[stage(1)];
            }
        }
        case 3:
        {
            switch(display)
            {
                case 1:
                    return positionOfLabel(pastLabels[stage(2)]);
                case 2:
                    return positionOfLabel(pastLabels[stage(1)]);
                case 3:
                    return position(3);
                case 4:
                    return positionOfLabel(4);
                
            }
        }
        case 4:
        {
            switch(display)
            {
                case 1:
                    return pastPositions[stage(1)];
                case 2:
                    return position(1);
                case 3:
                case 4:
                    return pastPositions[stage(2)];
            }
        }
        case 5:
        {
            switch(display)
            {
                case 1:
                    return positionOfLabel(pastLabels[stage(1)]);
                case 2:
                    return positionOfLabel(pastLabels[stage(2)]);
                case 3:
                    return positionOfLabel(pastLabels[stage(4)]);
                case 4:
                    return positionOfLabel(pastLabels[stage(3)]);
            }
        }
    }
}

// syntactic sugar to fix off-by-one and make solution function read identical to manual
// can't do constexpr functons?! :(
int8_t position(int8_t pos)
{
    return pos-1;
}
int8_t stage(int8_t s)
{
    return s-1;
}

int8_t positionOfLabel(int8_t label)
{
    for(int8_t i=0; i<4; ++i)
    {
        if(labels[i] == label)
            return i;
    }
}

void setComplete()
{
    done = true;

    for(int8_t i=0; i<7; ++i)
    {
        digitalWrite(SevenSegmentPins[i], LOW);
    }

    for(int8_t i=0; i<4; ++i)
    {
        digitalWrite(StageLEDs[i], HIGH);
    }

    displayLabels.clear();
    digitalWrite(VictoryLED, HIGH);
}


void fisherYates(int8_t* toShuffle, int8_t size)
{
    for(int8_t i=size-1; i>0; --i)
    {
        long j = random(i+1);
        int8_t tmp = toShuffle[j];
        toShuffle[j] = toShuffle[i];
        toShuffle[i] = tmp;
    }
}