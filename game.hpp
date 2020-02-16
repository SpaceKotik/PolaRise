#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "rayTracing.hpp"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

 /*   Vertex test_line[] =
{
    Vertex(Vector2f(10.f, 10.f)),
    Vertex(Vector2f(150.f, 150.f))
}; */

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



    Vertex testLine[8][2];

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
