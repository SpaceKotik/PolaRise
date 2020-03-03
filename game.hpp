#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "hero.hpp"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

struct MouseState{
    Vector2f pos;
    bool RightButtonPressed = false;
    bool LeftButtonPressed = false;
};


struct States {
    bool right = 0;
    bool left = 0;
    bool up = 0;
    bool down = 0;
    bool space = 0;
    bool mouseLeft = 0;
    bool mouseRight = 0;
    bool isOnStart = 0;
    bool isOnFinish = 0;

};

class Game {
private:
    GameState gameState;
    Texture texture;
    Event event;
    RenderWindow window;
    //Vector2f mousePos;
    Hero hero;

    RayTracing rayTracing;
    Level level;
    States keys;

public:
    Game();
//  ~Game();
    RenderWindow* getHandle();

    void setup();
    void run();
    MouseState input();
    void update();
    void draw(Level level, RayTracing rayTracing);
    void logic();
};
