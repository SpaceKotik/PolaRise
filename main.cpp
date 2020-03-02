#include <SFML/Graphics.hpp>
#include <iostream>
#include "tile.hpp"
#include "game.hpp"


using namespace sf;
int main()
{
    //srand(time(NULL));
    Game game;
    game.run();

    return 0;
}
