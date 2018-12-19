
constexpr char passwords[][] = { "about", "after", "again", "below", "could",
                                 "every", "first", "found", "great", "house",
                                 "large", "learn", "never", "other", "place",
                                 "plant", "point", "right", "small", "sound",
                                 "spell", "still", "study", "their", "there",
                                 "these", "thing", "think", "three", "water",
                                 "where", "which", "world", "would", "write"};

uint8_t passwordIndex;

void setup() 
{
    randomSeed(analogRead(0));
    passwordIndex = random(35);
    do
    {
      initWheels()
    }
    while(!combinationValid());
}

void initWheels()
{
    
}

void combinationValid()
{

}

void loop() 
{

}
