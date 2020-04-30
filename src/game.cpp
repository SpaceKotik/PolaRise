#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "Game/sceneLoader.h"

using namespace sf;
using namespace consts;

Game::Game() {

    textures.load(Textures::Background, "../Textures/background.png");
    textures.setRepeat(Textures::Background, true);

    shaders.load(Shaders::Pixel, "../shaders/pixel.frag");
    shaders.load(Shaders::Blur, "../shaders/blur.frag");
    shaders.load(Shaders::Shadow, "../shaders/shadow.frag");

    shaders.setParam(Shaders::Shadow, "frag_ScreenResolution", Vector2f(windowSize.x, windowSize.y));


    ///Using Mediator pattern; set mediator's pointer for all components
    level.setMediator(this);
    player.setMediator(this);
    lightScene.setMediator(this);
    lightScene.setShaders();

    ///probably will be used while adding GUI
    gameState = Gameplay;

    ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(VideoMode(windowSize.x, windowSize.y), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(600, 0));
    window.setMouseCursorGrabbed(false);


    /// TODO: extract method
    bufferTex.create(windowSize.x, windowSize.y);
    bufferTex.setSmooth(true);
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    ///


    //SceneLoader sceneLoader;
    //sceneLoader.loadFromFile(&lightScene);
    lightScene.loadScene();
}

void Game::run() {
    FPScounter fpsCounter;
    while(window.isOpen()) {
        window.setTitle("PolaRise" + fpsCounter.get());
        input();
        logic();
        draw();
    }
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
    statesTiles.blendMode = BlendAdd;
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
    shaders.get(Shaders::Pixel)->setUniform("image", bufferTex.getTexture());
    statesPixel.blendMode = BlendAdd;
    statesPixel.shader = shaders.get(Shaders::Pixel);

    ///Sprite for drawing buffer texture
    Sprite bufferSpr;
    bufferSpr.setTexture(bufferTex.getTexture());
    bufferSpr.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSpr.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);

    ///Start of actual drawing
    bufferTex.clear(Color::Transparent);

    ///Draw tiles
    for (int i = 0; i < fieldSize.x*fieldSize.y; ++i) {
        if (level.getField()->at(i).checkIfDrawable()) {
            bufferTex.draw(level.getField()->at(i).physForm, statesTiles);
            //spr2.setPosition(level.getField()->at(i).physForm.getPosition());
            //window.draw(spr2, statesTiles);
        }
    }
    bufferTex.display();

    ///pixelate tiles
    if (DOPIXEL)
        window.draw(bufferSpr, statesPixel);
    else
        window.draw(bufferSpr);
}

void Game::draw() {

    window.clear();
    drawLight();
    drawTiles();
    window.draw(player.flashLight.sprite);
    window.display();
}

void Game::input()  {
    Event event{};
    static bool isMouseInside = true;

    while (window.pollEvent(event)) {
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
                    lightScene.addEmitterOnInput(eVector2f(Vector2f(Mouse::getPosition(window))));
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
    ///memory test
    /*
    for (int i = 0; i < 200; ++i) {
        lightScene.addEmitter(eVector2f(500, 300), eVector2f(0, 1), new EmitterBehaviour::Rotate(0.04));
        //lightScene.setBehaviour(3, new EmitterBehaviour::Rotate(0.04));
        lightScene.deleteEmitter({500, 300});
    }
    */
    player.update((Vector2f)Mouse::getPosition(window));
    lightScene.updateEmittersRayTracing();
    lightScene.update();

}

void Game::restart() {
    player.reset();
    level.setField();
    lightScene.reset();
    lightScene.loadScene();
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

/*
///Tiles texturing
Texture tex2;
tex2.loadFromFile("../Textures/tile.png");
Sprite spr2;
spr2.setTexture(tex2);
spr2.scale(scale/24.f, scale/24.f);
spr2.setColor(Color(255, 210, 140));
*/