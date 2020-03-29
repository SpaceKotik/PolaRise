#pragma once

#include "../util/FpsCounter/fpsCounter.h"
#include "../SmartVector2/SmartVector2f.h"
#include <SFML/Graphics.hpp>
#include "level.hpp"
#include "player.hpp"
#include "dump.hpp"
#include "lightScene.h"
#include "emitterBehaviour.h"
#include "consts.h"

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
public:
    Game();
//  ~Game();
    RenderWindow* getHandle();
    void run();
    void input();
    void draw();
    void logic();
    void restart();
};