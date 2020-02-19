#include <SFML/Graphics.hpp>
#include <iostream>
#include "tile.hpp"
#include "game.hpp"


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
