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


struct States {
    bool right = 0;
    bool left = 0;

    bool space = 0;
    bool jumpAble = true;
    bool jumpMade = false;
    Clock jumpTime;

    bool isOnStart = 0;
    bool isOnFinish = 0;
};

class Game {
private:
    GameState gameState;
    RenderWindow window;

    Level level;
    Player player;

    LightScene lightScene;


    States keys;
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