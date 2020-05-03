#pragma once

#include "../util/FpsCounter/fpsCounter.h"
#include "../SmartVector2/SmartVector2f.h"
#include <SFML/Graphics.hpp>
#include "level.hpp"
#include "player.hpp"
#include "dump.hpp"
#include "lightScene.h"
#include "emitterBehaviour.h"
#include "lantern.h"
#include "consts.h"
#include "util/ResourceHolders/TextureHolder/textureHolder.h"
#include "util/ResourceHolders/ShaderHolder/shaderHolder.h"
#include "util/ResourceHolders/resourceIdentifiers.h"
#include "tile.hpp"


using namespace sf;

enum GameState {Menu, Menu_2, Gameplay, Leaderboard};

class Game {
private:
    GameState gameState;
    RenderWindow window;

    Level level;
    Player player;
    LightScene lightScene;

    TextureHolder textures;
    ShaderHolder shaders;

    Sprite bufferSprite;
    RenderTexture bufferTex;

protected:
    void drawLight();
    void drawTiles();
public:
    Game();
    //~Game();
    void run();
    void input();
    void draw();
    void logic();
    void restart();
    Level* getLevel();
    Player* getPlayer();
    ShaderHolder* getShaders();
};