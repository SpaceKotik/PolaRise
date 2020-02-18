#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

/*struct MouseState{
    Vector2f pos;
    bool RightButtonPressed = false;
    bool LeftButtonPressed = false;
};*/






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

    Vector2f mousePos;


public:
    Game();
    RenderWindow* getHandle();
//    ~Game();
    void setup();
    void run();
    Vector2f input();
    void update();
    void draw(Level level, RayTracing rayTracing);

};
