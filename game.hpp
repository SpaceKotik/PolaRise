#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
//#include "texts.hpp"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

class Game {
private:
    int field_size_x;
    int field_size_y;
    GameState gameState;
    Texture texture;
    Event event;
    //Tile **field;
    RenderWindow window;
    //Texts texts;
    int total_point;

    Tile testRect;
public:
    Game();
    RenderWindow* getHandle();
//    ~Game();
    void setup();
    void run();
    void input();
    void update();
    void draw();

};
