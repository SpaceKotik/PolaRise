#include <SFML/Graphics.hpp>
#include "game.hpp"

using namespace sf;
using namespace consts;

Game::Game() {
    ///Using Mediator pattern; set mediator's pointer for all components
    level.setMediator(this);
    player.setMediator(this);
    lightScene.setMediator(this);

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


    lightScene.addEmitter(eVector2f(1020, 750), eVector2f(0, 1), new EmitterBehaviour::Rotate(0.04));
    lightScene.addEmitter(eVector2f(900, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1100, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 300), eVector2f(0, 1), new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));
    lightScene.addEmitter(eVector2f(210, 615), eVector2f(-1, 0), new EmitterBehaviour::Flicker(9));
    lightScene.addEmitter(eVector2f(210, 495), eVector2f(-1, 0), new EmitterBehaviour::Flicker(6));
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

void Game::draw() {
    window.clear(backgroundColor);
    bufferTex.clear(backgroundColor);


    ///draw light
//////////////////////
    window.setActive(false);
    lightScene.draw();
    // TODO: Make tempSprite not that temp, not effective
    Sprite tempSprite;
    tempSprite.setTexture(lightScene.getTexture());
    tempSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    tempSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    window.draw(tempSprite, BlendAdd);
//////////////////////

    ///draw tiles
    for (int i = 0; i < fieldSize.x*fieldSize.y; ++i) {
        if (level.getField()->at(i).checkIfLightAbsorb()) {
            window.draw(level.getField()->at(i).physForm, BlendAdd);
        }
    }

    ///draw player
    window.draw(*player.getPhysForm());

    // TODO: Make resource holder for this
    ///pixelate everyrhing
    if (DOPIXEL) {
        if (!shaderShadow.loadFromFile("../shaders/pixelate.frag", sf::Shader::Fragment)) {
            window.close();
        }
        RenderStates states;
        shaderShadow.setUniform("image", lightScene.getTexture());
        states.blendMode = BlendNone;
        states.shader = &shaderShadow;
        window.draw(tempSprite, states);
    }

    window.display();
}

void Game::input()  {
    // TODO: maybe replace with calling Mouse::getPosition(window) each time?
    Vector2f mousePos;
    mousePos = (Vector2f)Mouse::getPosition(window);

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
                    lightScene.addEmitter( eVector2f(Vector2f(Mouse::getPosition(window))), eVector2f(0, 1));
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
            level.addTile(mousePos, Standart);
            lightScene.updateEmittersRayTracing();///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Middle)) {
            level.addTile(mousePos, Dynamic);
            lightScene.updateEmittersRayTracing();///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            level.removeTile(mousePos);
            lightScene.updateEmittersRayTracing();///////
        }
    }

    // FIXME: this definitely must be moved to player's logic
    ///normalized view
    player.view = eVector2f((Vector2f)Mouse::getPosition(window) - player.getPos()).norm();
}

void Game::logic() {
    ///Stress test
    /*
    for (int i = 0; i < 200; ++i) {
        lightScene.addEmitter(eVector2f(500, 300), eVector2f(0, 1));
        lightScene.setBehaviour(3, new EmitterBehaviour::Rotate(0.04));
        lightScene.deleteEmitter({500, 300});
    }*/
    // TODO: Refactor this
    level.resetActive();
    lightScene.update();
    lightScene.setActiveTiles();
    player.disableDynamicTiles();
    level.update();
    player.updateMovement();

}

void Game::restart() {
    level.setField();
    lightScene.reset();
    player.reset();
    ///TODO: Must load form level in the future
    lightScene.addEmitter(eVector2f(1020, 750), eVector2f(0, 1), new EmitterBehaviour::Rotate(0.04));
    lightScene.addEmitter(eVector2f(900, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1100, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 300), eVector2f(0, 1), new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));
    lightScene.addEmitter(eVector2f(210, 615), eVector2f(-1, 0), new EmitterBehaviour::Flicker(9));
    lightScene.addEmitter(eVector2f(210, 495), eVector2f(-1, 0), new EmitterBehaviour::Flicker(6));
    ////
}

void Game::notify(Player *player) {

}

void Game::notify(Level *level) {

}

void Game::notify(LightScene *lightScene) {

}

Level *Game::getLevel() {
    return &level;
}

RenderWindow* Game::getHandle() {
    return &window;
}
///may be useful for applying effects to menu etc.
/*
bool Game::setShaders() {
    if(DOSHADOW) {
        if(!shaderShadow.loadFromFile("../shaders/shadow.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderShadow.setParameter("frag_ScreenResolution", Vector2f(field_x*scale, field_y*scale));
        shaderShadow.setParameter("frag_LightColor", Vector3f(255, 255, 255));
    }

    if(DOBLUR) {
        if(!shaderBlur.loadFromFile("../shaders/blur.frag", sf::Shader::Fragment)) {
            window.close();
        }
        shaderBlur.setParameter("resolution", sf::Vector2f(field_x*scale, field_y*scale));
    }


    return true;
}

RenderStates Game::getStatesBlur(Vector2f dir, Texture buffer) {
    RenderStates states;
    //states.blendMode = BlendMultiply;
    if (!DOBLUR)
        return states;

    shaderBlur.setUniform("dir", dir);
    shaderBlur.setUniform("image", buffer);
    states.shader = &shaderBlur;

    return states;
}

//Note: param1 must be about 20000-30000 to make smooth light
//      param2 is inverse to the size of the light circle
//This shader uses kind of gaussian distribution function with different sigmas
RenderStates Game::getStatesShadow(float param1, float param2) {
    RenderStates states;
    states.blendMode = BlendMultiply;
    if (!DOSHADOW)
        return states;

    shaderShadow.setUniform("frag_LightOrigin", player.getPos());
    shaderShadow.setUniform("frag_ShadowParam1", param1);
    shaderShadow.setUniform("frag_ShadowParam2", param2);
    states.shader = &shaderShadow;

    return states;
}
 */
