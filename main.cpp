#include "Game.h"
#include <cstdlib>
#include <ctime>
using namespace std;

int main(){
	srand(static_cast<unsigned int>(time(0)));
	Game g(15);
	g.play();
}
