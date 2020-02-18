#include <SFML/Graphics.hpp>
#include <iostream>
//#include "tile.hpp"
#include "game.hpp"


//extern const int scale;
//extern const float delay;

using namespace sf;
int main()
{
	//std::cout << "a\n";
    srand(time(NULL));
    //Texts texts;
    Game game;
    //game.setup();
    game.run();

    return 0;
}
