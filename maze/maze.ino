#include <Button.h>

constexpr int8_t northButtonPin = 5;
constexpr int8_t southButtonPin = 2;
constexpr int8_t westButtonPin = 4;
constexpr int8_t eastButtonPin = 3;

constexpr int8_t ind = 1 << 7; // indicator
constexpr int8_t n = 1 << 0; // north valid move
constexpr int8_t s = 1 << 1; // south valid
constexpr int8_t w = 1 << 2;
constexpr int8_t e = 1 << 3;
constexpr int8_t ns = n | s; // north south valid
constexpr int8_t nw = n | w;
constexpr int8_t ne = n | e;
constexpr int8_t sw = s | w;
constexpr int8_t se = s | e;
constexpr int8_t we = w | e;
constexpr int8_t nsw = n | s | w;
constexpr int8_t nse = n | s | e;
constexpr int8_t nwe = n | w | e;
constexpr int8_t swe = s | w | e;

constexpr int8_t numMazes = 9;
constexpr int8_t mazeSize = 6;
constexpr int8_t mazes[numMazes][mazeSize][mazeSize] = 
{
    {
        {se, we, sw, se, we, w},
        {ns|ind, se, nw, ne, we, sw},
        {ns, ne, sw, se, we, nsw|ind},
        {ns, e, nwe, nw, e, nsw},
        {nse, we, sw, se, w, ns},
        {ne, w, ne, nw, e, nw}
    },
    {
        {e, swe, w, se, swe, w},
        {se, nw, se, nw, ne|ind, sw},
        {ns, se, nw, se, we, nsw},
        {nse, nw|ind, se, nw, s, ns},
        {ns, s, ns, s, n, ns},
        {n, ne, nw, ne, we, nw}
    },
    {
        {se, we, sw, s, se, sw},
        {n, s, ns, ne, nw, ns},
        {se, nsw, ns, se, sw, ns},
        {ns, ns, ns, ns|ind, ns, ns|ind},
        {ns, ne, nw, ns, ns, ns},
        {ne, we, we, nw, ne, nw}
    },
    {
        {se|ind, sw, e, we, we, sw},
        {ns, ns, se, we, we, nsw},
        {ns, ne, nw, se, w, ns},
        {ns|ind, e, we, ne, we, nsw},
        {nse, we, we, we, sw, ns},
        {ne, we, w, e, nw, n}
    },
    {
        {e, we, we, we, swe, sw},
        {se, we, we, swe, nw, n},
        {nse, sw, e, nw, se|ind, sw},
        {ns, ne, we, sw, n, ns},
        {ns, se, we, nwe, w, ns},
        {n, ne, we, we|ind, we, nw}
    },
    {
        {s, se, sw, e, swe|ind, sw},
        {ns, ns, ns, se, nw, ns},
        {nse, nw, n, ns, ne, nw},
        {ne, sw, se, nsw, ns, s},
        {se, nw, n|ind, ns, ne, nsw},
        {ne, we, we, nw, e, nw}
    },
    {
        {se, we|ind, we, sw, se, sw},
        {ns, se, w, ne, nw, ns},
        {ne, nw, se, w, se, nw},
        {se, sw, nse, we, nw, s},
        {ns, n, ne, we, sw, ns},
        {ne, we|ind, we, we, nwe, nw}
    },
    {
        {s, se, we, sw|ind, se, sw},
        {nse, nwe, w, ne, nw, ns},
        {ns, se, we, we, sw, ns},
        {ns, ne, sw|ind, e, nwe, nw},
        {ns, s, ne, we, we, w},
        {ne, nwe, we, we, we, w}
    },
    {
        {s, se, we, we, swe, sw},
        {ns, ns, se|ind, w, ns, ns},
        {nse, nwe, nw, se, nw, ns},
        {ns, s, se, nw, e, nsw},
        {ns|ind, ns, ns, se, sw, n},
        {ne, nw, ne, nw, ne, w}
    }
};

int8_t mazeIndex;
int8_t playerRow;
int8_t playerCol;
int8_t goalRow;
int8_t goalCol;

Button buttons[] =
{
  Button(northButtonPin),
  Button(southButtonPin),
  Button(westButtonPin), 
  Button(eastButtonPin)
};

void setup() 
{
    Serial.begin(9600);

    randomSeed(analogRead(0));

    mazeIndex = random(9);
    do
    {
        goalRow = random(mazeSize);
        goalCol = random(mazeSize);
    }
    while(mazes[mazeIndex][goalRow][goalCol] & ind == 1); // dont spawn goal at indicator

    do
    {
        playerRow = random(mazeSize);
        playerCol = random(mazeSize);
    }
    while(mazes[mazeIndex][goalRow][goalCol] & ind == 1); // dont spawn player at an indicator or goal
    printStatus();
}

void loop()
{
    for(int8_t i = 0; i<4; ++i)
    {
        if(buttons[i].uniquePress())
        {
            tryMove(1 << i);
        }
    }
}

void tryMove(int8_t direction)
{
    if(mazes[mazeIndex][playerRow][playerCol] & direction)
    {
        switch(direction)
        {
            case n:
                --playerRow;
                break;
            case s:
                ++playerRow;
                break;
            case w:
                --playerCol;
                break;
            case e:
                ++playerCol;
                break;
        }
        printStatus();
    }
    else
    {
        Serial.println("BZZZT!");
    }
}

void printStatus()
{
    Serial.print("Maze: ");
    Serial.print(mazeIndex);
    Serial.print("\nPlayer: ");
    Serial.print(playerRow);
    Serial.print(" ");
    Serial.print(playerCol);
    Serial.print("\n");
}
