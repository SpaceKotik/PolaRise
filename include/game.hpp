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

enum GameState {Menu, Gameplay};

class Game {
private:
    GameState gameState;
    RenderWindow window;

    Level level;
    Player player;
    LightScene lightScene;

    TextureHolder textures;
    ShaderHolder shaders;
    RenderTexture bufferWindow;

    std::string levelPath;

protected:
    void drawLight();
    void drawTiles();
public:
    Game();
    void run();
    void input();
    void draw();
    void logic();
    void restart();
    Level* getLevel();
    Player* getPlayer();
    ShaderHolder* getShaders();
    std::string getLevelPath();
};