#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

struct MouseState{
    Vector2f pos;
    bool RightButtonPressed = false;
    bool LeftButtonPressed = false;
};


class Game {
private:
    GameState gameState;
    Texture texture;
    Event event;
    RenderWindow window;
    Vector2f mousePos;

public:
    Game();
//  ~Game();
    RenderWindow* getHandle();

    void setup();
    void run();
    MouseState input();
    void update();
    void draw(Level level, RayTracing rayTracing);
};
