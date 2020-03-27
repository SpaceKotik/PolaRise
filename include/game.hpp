#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "player.hpp"
#include "dump.hpp"
#include "lightScene.h"
#include "lightEmitter.h"

using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};


class Game {
private:
    GameState gameState;
    RenderWindow window;

    Level level;
    Player player;

    LightScene lightScene;

    Sprite bufferSprite;
    Shader shaderBlur;
    Shader shaderShadow;
    RenderTexture bufferTex;
    Dump dump;
    void logicMovement();
public:
    Game();
//  ~Game();
    RenderWindow* getHandle();

    //void setup();
    void run();
    void input();
   //void update();
    void draw();
    void logic();

    //bool setShaders();
    //RenderStates getStatesShadow(float param1, float param2);
    //RenderStates getStatesBlur(Vector2f dir, Texture buffer);
};