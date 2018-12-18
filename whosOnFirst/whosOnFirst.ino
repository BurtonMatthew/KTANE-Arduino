// Keep Talking and Nobody Explodes Who's on First Module for Arduino
// By Matthew Burton

#include <LiquidCrystal.h>
#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])
#define LCD_WIDTH 8
#define LCD_HEIGHT 1

/** DATA */
// The words that can appear in the top display
const char* const displayWords[] = { "YES", "FIRST", "DISPLAY", "OKAY", "SAYS", "NOTHING", "", "BLANK",
                                     "NO", "LED", "LEAD", "READ", "RED", "REED", "LEED", "HOLD ON", "YOU",
                                     "YOU ARE", "YOUR", "YOU'RE", "UR", "THERE", "THEY'RE", "THEIR",
                                     "THEY ARE", "SEE", "C", "CEE" };

 /*
  * The label to use  to determine which button to push, where the index in this array == the one in displayWords
  * the indices here are defined in this format for the 2x3 grid:
  * WORD
  *  0 1
  *  2 3
  *  4 5
  */
const byte displayLabelMappings[] = { 2, 1, 5, 1, 5, 2, // YES FIRST DISPLAY OKAY SAYS NOTHING
                                      4, 3, 5, 2, 5, 3, // "" BLANK NO LED LEAD READ
                                      3, 4, 4, 5, 4, 5, // RED REED LEED "HOLD ON" YOU "YOU ARE"
                                      3, 3, 0, 5, 4, 3, // YOUR YOU'RE UR THERE THEY'RE THEIR
                                      2, 5, 1, 5 };     // "THEY ARE" SEE C CEE

const char* const labelWords[] = { "READY", "FIRST", "NO", "BLANK", "NOTHING", "YES", "WHAT", "UHHH", "LEFT", "RIGHT", 
                                   "MIDDLE", "OKAY", "WAIT", "PRESS", "YOU", "YOU ARE", "YOUR", "YOU'RE", "UR", "U",
                                   "UH HUH", "UH UH", "WHAT?", "DONE", "NEXT", "HOLD", "SURE", "LIKE" };

// A table of the inorder correct answers for a given labelWords index, the first of these to be present as a label
// is the solution to the puzzle
const byte labelSolutions[ARRAY_SIZE(labelWords)][14] = { {5, 11, 6, 10, 8, 13, 9, 3, 0, 2, 1, 7, 4, 12},
                                                          {8, 11, 5, 10, 2, 9, 4, 7, 12, 0, 3, 6, 13, 1},
                                                          {3, 7, 12, 1, 6, 0, 9, 5, 4, 8, 13, 11, 2, 10},
                                                          {12, 9, 11, 10, 3, 13, 0, 4, 2, 6, 8, 7, 5, 1},
                                                          {7, 9, 11, 10, 5, 3, 2, 13, 8, 6, 12, 1, 4, 0},
                                                          {11, 9, 7, 10, 1, 6, 13, 0, 4, 5, 8, 3, 2, 12},
                                                          {7, 6, 8, 4, 0, 3, 10, 2, 11, 1, 12, 5, 13, 9},
                                                          {0, 4, 8, 6, 11, 5, 9, 2, 13, 3, 7, 10, 12, 1},
                                                          {9, 8, 1, 2, 10, 5, 3, 6, 7, 12, 13, 0, 11, 4},
                                                          {5, 4, 0, 13, 2, 12, 6, 9, 10, 8, 7, 3, 11, 1},
                                                          {3, 0, 11, 6, 4, 13, 2, 12, 8, 10, 9, 1, 7, 5},
                                                          {10, 2, 1, 5, 7, 4, 12, 11, 8, 0, 3, 13, 6, 9},
                                                          {7, 2, 3, 11, 5, 8, 1, 13, 6, 12, 4, 0, 9, 10},
                                                          {9, 10, 5, 0, 13, 11, 4, 7, 3, 8, 1, 6, 2, 12},
                                                          {26, 15, 16, 17, 24, 20, 18, 25, 22, 14, 21, 27, 23, 19},
                                                          {16, 24, 27, 20, 22, 23, 21, 25, 14, 19, 17, 26, 18, 15},
                                                          {21, 15, 20, 16, 24, 18, 26, 19, 17, 14, 22, 25, 27, 23},
                                                          {14, 17, 18, 24, 21, 15, 19, 16, 22, 20, 26, 23, 27, 25},
                                                          {23, 19, 18, 20, 22, 26, 16, 25, 17, 27, 24, 21, 15, 14},
                                                          {20, 26, 24, 22, 17, 18, 21, 23, 19, 14, 27, 25, 15, 16},
                                                          {20, 16, 15, 14, 23, 25, 21, 24, 26, 27, 17, 18, 19, 22},
                                                          {18, 19, 15, 17, 24, 21, 23, 14, 20, 27, 16, 26, 25, 22},
                                                          {14, 25, 17, 16, 19, 23, 21, 27, 15, 20, 18, 24, 22, 26},
                                                          {26, 20, 24, 22, 16, 18, 17, 25, 27, 14, 19, 15, 21, 23},
                                                          {22, 20, 21, 16, 25, 26, 24, 27, 23, 15, 18, 17, 19, 14},
                                                          {15, 19, 23, 21, 14, 18, 26, 22, 17, 24, 25, 20, 16, 27},
                                                          {15, 23, 27, 17, 14, 25, 20, 18, 26, 19, 22, 24, 16, 21},
                                                          {17, 24, 19, 18, 25, 23, 21, 22, 20, 14, 27, 26, 15, 16}};
  

/** GLOBALS */
LiquidCrystal displayLCD(2,7,3,4,5,6);
LiquidCrystal label0LCD(2,8,3,4,5,6);
LiquidCrystal label1LCD(2,9,3,4,5,6);
LiquidCrystal label2LCD(2,10,3,4,5,6);
LiquidCrystal label3LCD(2,11,3,4,5,6);
LiquidCrystal label4LCD(2,12,3,4,5,6);
LiquidCrystal label5LCD(2,13,3,4,5,6);

LiquidCrystal* const labelLCDs[] = {&label0LCD, &label1LCD, &label2LCD, &label3LCD, &label4LCD, &label5LCD};

byte solution;

void setup() 
{
  displayLCD.begin(LCD_WIDTH, LCD_HEIGHT);
  for(int i=0; i<6; ++i)
  {
    labelLCDs[i]->begin(LCD_WIDTH, LCD_HEIGHT);
  }

  pinMode(A0, INPUT); // Buttons for label 0..n
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, OUTPUT); // Finish LED
  
  startRound();
}

void loop() 
{
  // TODO: COMMUNICATION WITH MAIN MODULE
  // TODO: CHECK FOR USER BUTTON INPUTS
  // buttonState = digitalRead(buttonPin);
  // TODO: PLAY 3 TIMES TO WIN
}

void startRound()
{
  long displayIndex = random(ARRAY_SIZE(displayWords));
  displayLCD.print(displayWords[displayIndex]);
  setupLabels(displayLabelMappings[displayIndex]);
}

void setupLabels(byte targetIndex)
{
  // Label list actually split into two halves, random for which half to use
  long useFirstLabelHalf = random(2);

  // Fill up an array with the valid label indices, we're going to shuffle it and take the first 6
  byte labelIndexList[ARRAY_SIZE(labelWords) / 2];
  for(byte i=0; i<ARRAY_SIZE(labelIndexList); ++i)
  {
    labelIndexList[i] = useFirstLabelHalf ? i : i + ARRAY_SIZE(labelWords)/2;
  }

  fisherYates(labelIndexList, ARRAY_SIZE(labelIndexList));

  // Store off the correct solution
  solution = findSolution(labelIndexList, targetIndex);
  if(solution == -1)
  {
    displayLCD.print("Error");
  }
  
  // Write labels to LCDs
  for(byte i=0; i<ARRAY_SIZE(labelLCDs); ++i)
  {
    labelLCDs[i]->print(labelWords[labelIndexList[i]]);
  }
}

byte findSolution(byte* labelIndexList, byte targetIndex)
{
  for(int i=0; i<14; ++i)
  {
    byte checkSolution = labelSolutions[labelIndexList[targetIndex]][i];
    for(byte j=0; j<ARRAY_SIZE(labelLCDs); j++)
    {
      if(labelIndexList[j] == checkSolution)
      {
        return j;
      }
    }
  }
  return -1;
}

void fisherYates(byte* toShuffle, byte size)
{
  for(byte i=size-1; i>0; --i)
  {
    long j = random(i+1);
    byte tmp = toShuffle[j];
    toShuffle[j] = toShuffle[i];
    toShuffle[i] = tmp;
  }
}



