#pragma once

#include "util/FpsCounter/fpsCounter.h"
#include "util/SmartVector2/SmartVector2f.h"
#include <SFML/Graphics.hpp>
#include "level.hpp"
#include "player.hpp"
#include "dump.hpp"
#include "LightSystem/include/lightScene.h"
#include "LightSystem/include/emitterBehaviour.h"
#include "lantern.h"
#include "consts.h"
#include "util/ResourceHolders/TextureHolder/textureHolder.h"
#include "util/ResourceHolders/ShaderHolder/shaderHolder.h"
#include "util/ResourceHolders/resourceIdentifiers.h"
#include "tile.hpp"
#include <TGUI/TGUI.hpp>
#include "GUI/Menu/MenuGUI.h"
#include "GUI/Pause/PauseGUI.h"

using namespace sf;

enum GameState {Menu, Gameplay, Pause};

class Game {
private:
    MenuGUI menuScreen;
    PauseGUI pauseScreen;

    GameState gameState;
    RenderWindow window;

    Level level;
    Player player;
    LightScene lightScene;

    TextureHolder textures;
    ShaderHolder shaders;
    RenderTexture bufferWindow;

    int currentLevel;

    tgui::Gui* gui = new tgui::Gui{window};

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
    void changeState(std::string State);
    void loadNextLevel();
    Level* getLevel();
    Player* getPlayer();
    ShaderHolder* getShaders();
    RenderWindow* getWindow();
    LightScene* getLightScene();
    std::string getLevelPath();
};
