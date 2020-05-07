#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "Game/sceneLoader.h"
#include <TGUI/TGUI.hpp>

using namespace sf;
using namespace consts;

Game::Game() {
    menu.setup();
    ///probably will be used while adding GUI
    gameState = MenuState;

    ///Setting level path used in scene and level loaders
    levelPath = "../Levels/1/";

    /// Setting up resource holders
    textures.load(Textures::Background, "../Textures/background.png");
    textures.setRepeat(Textures::Background, true);

    shaders.load(Shaders::Pixel, "../shaders/pixel.frag");
    shaders.load(Shaders::Blur, "../shaders/blur.frag");
    shaders.load(Shaders::Shadow, "../shaders/shadow.frag");

    shaders.setParam(Shaders::Shadow, "frag_ScreenResolution", Vector2f(windowSize.x, windowSize.y));


    ///Using Mediator pattern; set mediator's pointer for all components
    level.setMediator(this);
    level.setField();

    player.setMediator(this);
    lightScene.setMediator(this);
    lightScene.setShaders();
    lightScene.loadScene();

    ///Setting window
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(VideoMode(windowSize.x, windowSize.y), "PolaRise",
                  Style::Titlebar | Style::Close);
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(600, 0));
    window.setMouseCursorGrabbed(false);


    /// TODO: move bufferWindow from Game class
    bufferWindow.create(windowSize.x, windowSize.y);
    bufferWindow.setSmooth(true);
    ///

}

void Game::run() {
    FPScounter fpsCounter;
    while(window.isOpen()) {
        if (gameState == GameplayState) {
            window.setTitle("PolaRise" + fpsCounter.get());
            input();
            logic();
            draw();
        }
        else if (gameState == MenuState) {
            menu.input();
            menu.draw();
        }
    }
}

void Game::draw() {

    window.clear();
    drawLight();
    drawTiles();
    window.draw(player.flashLight.sprite);
    gui->draw();
    window.display();
}

void Game::drawLight() {
    Sprite bufferSpr, backgroundSprite;

    RenderStates states;
    if(DOPIXEL) {
        shaders.get(Shaders::Pixel)->setUniform("image", lightScene.getTexture());
        states.shader = shaders.get(Shaders::Pixel);
    }

    /// 1st layer of light
    window.setActive(false);

    bufferSpr.setTexture(lightScene.drawToTex());
    bufferSpr.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSpr.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    window.draw(bufferSpr, states);

    /// background
    backgroundSprite.setTexture(textures.get(Textures::Background));
    backgroundSprite.setTextureRect(IntRect(0, 0, windowSize.x * 2, windowSize.y * 2));
    backgroundSprite.setPosition(-windowSize.x / 4.f , 0);
    backgroundSprite.setScale(3.f/4.f, 3.f/4.f);
    window.draw(backgroundSprite, BlendMultiply);

    /// player
    window.draw(*player.getPhysForm());

    /// 2nd layer of light
    bufferSpr.setColor(Color(110, 110, 110));
    window.draw(bufferSpr, BlendAdd);
}

void Game::drawTiles() {

    ///Set states for drawing tiles with shadow around player
    RenderStates statesTiles;
    statesTiles.blendMode = BlendMultiply;
    if (DOSHADOW) {
        statesTiles.shader = shaders.get(Shaders::Shadow);
        shaders.setParam(Shaders::Shadow, "coef1", 240.f);
        shaders.setParam(Shaders::Shadow, "coef2", 0.14f);
        shaders.setParam(Shaders::Shadow, "coef3", 0.07f);
        shaders.setParam(Shaders::Shadow, "frag_LightColor", Vector3f(255, 255, 255));
        shaders.setParam(Shaders::Shadow, "frag_LightOrigin", player.getPos());
    }
    ///States for pixelating tiles
    RenderStates statesPixel;
    shaders.get(Shaders::Pixel)->setUniform("image", bufferWindow.getTexture());
    statesPixel.blendMode = BlendAdd;
    statesPixel.shader = shaders.get(Shaders::Pixel);

    ///Sprite for drawing buffer texture
    Sprite bufferSpr;
    bufferSpr.setTexture(bufferWindow.getTexture());
    bufferSpr.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSpr.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);

    ///Start of actual drawing
    bufferWindow.clear(Color::Transparent);

    ///Draw tiles
    for (int i = 0; i < fieldSize.x*fieldSize.y; ++i) {
        if (level.getField()->at(i).checkIfDrawable()) {
            ///Basically drawing a tile and then drawing light mask above it (quite elegant solution in 1 line of code instead
            /// of rewriting the whole shader logic)
            bufferWindow.draw(level.getField()->at(i).physForm, BlendAdd);
            bufferWindow.draw(level.getField()->at(i).physForm, statesTiles);
            //spr2.setPosition(level.getField()->at(i).physForm.getPosition());
            //window.draw(spr2, statesTiles);
        }
    }
    bufferWindow.display();

    ///pixelate tiles
    if (DOPIXEL)
        window.draw(bufferSpr, statesPixel);
    else
        window.draw(bufferSpr);
}

void Game::input()  {
    Event event{};
    static bool isMouseInside = true;
    while (window.pollEvent(event)) {
        gui->handleEvent(event);
        switch (event.type) {
            case Event::MouseLeft:
                isMouseInside = false;
                break;
            case Event::MouseEntered:
                isMouseInside = true;
                break;
            case Event::Closed:
                window.close();
                break;
            case Event::KeyPressed:
                if(event.key.code == Keyboard::LShift) {
                    //lightScene.addEmitterOnInput(eVector2f(Vector2f(Mouse::getPosition(window))));
                    lightScene.addEmitter(eVector2f(Vector2f(Mouse::getPosition(window))));
                }
                if(event.key.code == Keyboard::LControl) {
                    lightScene.deleteEmitter(Vector2f(Vector2f(Mouse::getPosition(window))));
                }
                if(event.key.code == Keyboard::Space) {
                    // TODO: maybe move all logic of changing to player class?
                    if (player.states.jumpAble)
                        player.states.jumpMade = true;
                }
                if(event.key.code == Keyboard::Escape) {
                    window.close();
                }
                if(event.key.code == Keyboard::S) {
                    lightScene.removeShotForce();
                    lightScene.addEmitter(player.getPos(), player.getView(),
                                          new EmitterBehaviour::Shootable(&player, &level));

                }
                if(event.key.code == Keyboard::R) {
                    restart();
                }

                if(event.key.code == Keyboard::D) {
                    player.states.right = true;
                }
                if(event.key.code == Keyboard::A) {
                    player.states.left = true;
                }
                break;

            case Event::KeyReleased:
                if(event.key.code == Keyboard::D) {
                    player.states.right = false;
                }
                if(event.key.code == Keyboard::A) {
                    player.states.left = false;
                }
                if(event.key.code == Keyboard::Space) {
                    player.states.space = false;
                    player.states.jumpAble = false;
                }
                if (event.key.code == Keyboard::Num1) {
                    level.addTile((Vector2f)Mouse::getPosition(window), Deadly);
                    lightScene.updateEmittersRayTracing(true);///////
                }
            default:
                break;
            }
    }
    if(isMouseInside) {
        if (Mouse::isButtonPressed(sf::Mouse::Left)) {
            level.addTile((Vector2f)Mouse::getPosition(window), Standart);
            lightScene.updateEmittersRayTracing(true);///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Middle)) {
            level.addTile((Vector2f)Mouse::getPosition(window), Dynamic);
            lightScene.updateEmittersRayTracing(true);///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            level.removeTile((Vector2f)Mouse::getPosition(window));
            lightScene.updateEmittersRayTracing(true);///////
        }
    }
}

void Game::logic() {
    player.update((Vector2f)Mouse::getPosition(window));
    //lightScene.updateEmittersRayTracing();
    lightScene.update();

}

void Game::restart() {
    //levelPath = "../Levels/2/";
    player.reset();
    level.setField();
    lightScene.reset();
    lightScene.loadScene();
}

void Game::changeState(std::string state) {
    if (state == "Menu")
        gameState = MenuState;
    else if (state == "Gameplay")
        gameState = GameplayState;
}

Level* Game::getLevel() {
    return &level;
}

Player* Game::getPlayer() {
    return &player;
}

ShaderHolder* Game::getShaders() {
    return &shaders;
}

RenderWindow *Game::getWindow() {
    return &window;
}

std::string Game::getLevelPath() {
    return levelPath;
}


/*
///Tiles texturing
Texture tex2;
tex2.loadFromFile("../Textures/tile.png");
Sprite spr2;
spr2.setTexture(tex2);
spr2.scale(scale/24.f, scale/24.f);
spr2.setColor(Color(255, 210, 140));
*/