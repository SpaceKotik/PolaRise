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


  /*if (attenuation < 0.0001)
    attenuation = attenuation / (linear_distance * 0.002);*/