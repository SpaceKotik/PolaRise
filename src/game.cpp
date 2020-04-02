#include <SFML/Graphics.hpp>
#include "game.hpp"


using namespace sf;
using namespace consts;

Game::Game() {

    level.setMediator(this);
    player.setMediator(this);
    lightScene.setMediator(this);

    gameState = Gameplay;

    ContextSettings settings;
    settings.antialiasingLevel = 8;
    
    window.create(VideoMode(windowSize.x, windowSize.y), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(600, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();
    player.setLevel(&level);

    bufferTex.create(windowSize.x, windowSize.y);
    bufferTex.setSmooth(true);
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);

    if(!lightScene.setShaders(DOBLUR, DOSHADOW))
       window.close();

    lightScene.addEmitter(eVector2f(1020, 750), eVector2f(0, 1), new EmitterBehaviour::Rotate(0.04));
    lightScene.addEmitter(eVector2f(900, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1100, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 300), eVector2f(0, 1), new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));
    lightScene.addEmitter(eVector2f(210, 615), eVector2f(-1, 0), new EmitterBehaviour::Flicker(9));
    lightScene.addEmitter(eVector2f(210, 495), eVector2f(-1, 0), new EmitterBehaviour::Flicker(6));

    /*lightScene.setBehaviour(0, new EmitterBehaviour::Rotate(0.04));
    lightScene.setBehaviour(1, new EmitterBehaviour::Flicker(3));
    lightScene.setBehaviour(2, new EmitterBehaviour::Flicker(3));
    lightScene.setBehaviour(3, new EmitterBehaviour::Flicker(3));
    lightScene.setBehaviour(4, new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));
    lightScene.setBehaviour(5, new EmitterBehaviour::Flicker(9));
    lightScene.setBehaviour(6, new EmitterBehaviour::Flicker(6));*/

    lightScene.updateEmittersRayTracing();
}

RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
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

//////////////////////
    window.setActive(false);


    lightScene.draw();
    Sprite tempSprite;
    tempSprite.setTexture(lightScene.getTexture());
    tempSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    tempSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    window.draw(tempSprite, BlendAdd);
//////////////////////

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

    ///draw all tiles
    for (int i = 0; i < fieldSize.x*fieldSize.y; ++i) {
        if (level.getField()->tiles[i].checkIfLightAbsorb()) {
            window.draw(level.getField()->tiles[i].physForm, BlendAdd);
        }
    }


    ///Debug does not work like this now
    /*
    //DEBUG
    if (!NDEBUG) {
        //DEBUG DRAW POINTS
        for (int i = 0; i < rayTracing.vertices.size(); ++i) {
            CircleShape currCirle;
            currCirle.setRadius(2);
            currCirle.setOrigin(2, 2);
            currCirle.setPosition(rayTracing.vertices.at(i));
            currCirle.setFillColor(Color::Red);
            window.draw(currCirle);
        }
        //DDEBUG DRAW EDGES
        for (int i = 0; i < rayTracing.edges.size(); ++i) {
            Vertex currLine[2];
            currLine[0].position = rayTracing.edges.at(i).startCoord;
            currLine[1].position = rayTracing.edges.at(i).startCoord + Vector2f(rayTracing.edges.at(i).dirX, rayTracing.edges.at(i).dirY);
            currLine[0].color = Color::Green;
            currLine[1].color = Color::Green;
            window.draw(currLine, 2, Lines);
        }
        //DEBUG DRAW RAYS
        rayTracing.update(player.getPos(), true, player.view, player.lineOfSight);
        dump.add("Rays on screen: " + std::to_string(rayTracing.raysVertex.size()) + "\n");
        for (int i = 0; i < rayTracing.raysVertex.size(); ++i) {
            
            Vertex currRay[2];
            currRay[0].position = rayTracing.raysVertex.at(i)[0].position;
            currRay[1].position = rayTracing.raysVertex.at(i)[1].position;
            currRay[0].color = Color::White;
            currRay[1].color = Color::White;
            window.draw(currRay, 2, Lines);
        }

    }*/

    ///draw player
    window.draw(*player.getPhysForm());
    window.display();

}

void Game::input()  {

    Vector2f mousePos;
    mousePos = (Vector2f)Mouse::getPosition(window);

    Event event;
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
                    lightScene.updateEmittersRayTracing();
                }
                if(event.key.code == Keyboard::LControl) {
                    lightScene.deleteEmitter(Vector2f(Vector2f(Mouse::getPosition(window))));
                }
                if(event.key.code == Keyboard::Space) {
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


    ///normalized view
    player.view = eVector2f((Vector2f)Mouse::getPosition(window) - player.getPos()).norm();
}

void Game::logic() {
    /*for (int i = 0; i < 20000; ++i) {
        lightScene.addEmitter(eVector2f(500, 300), eVector2f(0, 1), true, false);
        lightScene.setBehaviour(3, new EmitterBehaviour::Rotate(0.04));
        lightScene.deleteEmitter({500, 300});
    }*/

    lightScene.update();

    level.resetActive();
    lightScene.setActiveTiles();
    player.disableDynamicTiles();
    level.update();
    player.updateMovement();

    //dump.add("Velocity: (" + std::to_string(player.velocity.x) + " : " + std::to_string(player.velocity.y) + ")");
    //dump.add("View: (" + std::to_string(player.view.x) + " : " + std::to_string(player.view.y) + ")");


}

void Game::restart() {
    level.setField();
    lightScene.reset();
    player.reset();
    ///Must load form level in the future
    lightScene.addEmitter(eVector2f(1020, 750), eVector2f(0, 1), new EmitterBehaviour::Rotate(0.04));
    lightScene.addEmitter(eVector2f(900, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1100, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 460), eVector2f(0, 1), new EmitterBehaviour::Flicker(3));
    lightScene.addEmitter(eVector2f(1000, 300), eVector2f(0, 1), new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));
    lightScene.addEmitter(eVector2f(210, 615), eVector2f(-1, 0), new EmitterBehaviour::Flicker(9));
    lightScene.addEmitter(eVector2f(210, 495), eVector2f(-1, 0), new EmitterBehaviour::Flicker(6));

    lightScene.updateEmittersRayTracing();
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
