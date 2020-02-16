#include <SFML/Graphics.hpp>
//#include "tile.hpp"
#include "game.hpp"


//extern const int scale;
//extern const float delay;

using namespace sf;
int main()
{
    srand(time(NULL));
    //Texts texts;
    Game game;
    //game.setup();
    game.run();

    return 0;
}
