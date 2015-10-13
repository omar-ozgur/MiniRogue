#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED

const char ARROW_LEFT  = 'h';
const char ARROW_RIGHT = 'l';
const char ARROW_UP    = 'k';
const char ARROW_DOWN  = 'j';

char getCharacter();
void clearScreen();

// Random int from 0 to limit-1
int randInt(int limit);
// Return true with probability p
bool trueWithProbability(double p);

#endif // UTILITIES_INCLUDED
