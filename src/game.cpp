#include <SFML/Graphics.hpp>
#include <iostream>
#include <mutex>
#include <cmath>

#include "../FpsCounter/fpsCounter.h"
#include "../SmartVector2/SmartVector2f.h"
#include "game.hpp"
#include "level.hpp"
#include "consts.h"
//#include "emitterBehaviour.h"


using namespace sf;
using namespace consts;

//#define NDEBUG true         ///show rays, obstacles, etc
//#define UPDATESHADERS false ///update changes in shaders in real time
//#define DUMP false

#define DOPIXEL false
#define DOBLUR true
#define DOSHADOW true

// TODO: solve edge cases for emitters, player, etc.
// TODO: fix freezes, maybe make movement fps invariant
// TODO: make lightScene more flexible (for rotating and moving emitters, like emitterRotate() and emitterMove())

Game::Game() {
    gameState = Gameplay;

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(window_x, window_y), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(600, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();
    player.setLevel(&level);
    bufferTex.create(window_x, window_y);
    bufferTex.setSmooth(true);
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(window_x / 2.f, window_y / 2.f);
    bufferSprite.setPosition(window_x / 2.f, window_y / 2.f);

    if(!lightScene.setShaders(DOBLUR, DOSHADOW))
       window.close();
    //lightScene.addEmitter(eVector2f(200, 100), eVector2f(1,1), true, false);
    lightScene.addEmitter(eVector2f(1000, 750), eVector2f(0, 1), true, false);
    lightScene.addEmitter(eVector2f(400, 100), eVector2f(0, 1), true, false);

    lightScene.setBehaviour(0, new EmitterBehaviour::Rotate(0.04));
    lightScene.setBehaviour(1, new EmitterBehaviour::MoveByPath({400, 100}, {800, 100}, 5));

    lightScene.updateEmittersRayTracing(&level);
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
    tempSprite.setOrigin(window_x / 2.f, window_y / 2.f);
    tempSprite.setPosition(window_x / 2.f, window_y / 2.f);
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
    for (int i = 0; i < field_x*field_y; ++i) {
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
    while (window.pollEvent(event)) {

        switch (event.type) {
        case Event::Closed:
            window.close();
            break;
        case Event::KeyPressed:
            if(event.key.code == Keyboard::LShift) {
                lightScene.addEmitter( eVector2f(Vector2f(Mouse::getPosition(window))), eVector2f(1, 1), false, true);
                lightScene.updateEmittersRayTracing(&level);
            }
            if(event.key.code == Keyboard::LControl) {
                lightScene.deleteEmitter(Vector2f(Vector2f(Mouse::getPosition(window))));
            }
            if(event.key.code == Keyboard::Space) {
                if (keys.jumpAble) {
                    player.velocity.y -= maxVelocity;
                    keys.jumpTime.restart();
                    keys.space = true;
                }
            }
            if(event.key.code == Keyboard::Escape) {
                window.close();
            }

            if(event.key.code == Keyboard::D) {
                keys.right = true;
            }
            if(event.key.code == Keyboard::A) {
                keys.left = true;
            }
            if(event.key.code == Keyboard::W) {
                keys.up = true;
            }
            if(event.key.code == Keyboard::S) {
                keys.down= true;
            }

            break;

        case Event::KeyReleased:
            if(event.key.code == Keyboard::D) {
                keys.right = false;
            }
            if(event.key.code == Keyboard::A) {
                keys.left = false;
            }
            if(event.key.code == Keyboard::W) {
                keys.up = false;
            }
            if(event.key.code == Keyboard::S) {
                keys.down= false;
            }
            if(event.key.code == Keyboard::Space) {
                keys.space = false;
                keys.jumpAble = false;
            }
        default:
            break;
        }
    }

    if (Mouse::isButtonPressed(sf::Mouse::Left)) {
                level.addTile(mousePos, Standart);
                lightScene.updateEmittersRayTracing(&level);///////
    }
    if (Mouse::isButtonPressed(sf::Mouse::Middle)) {
        level.addTile(mousePos, Dynamic);
        lightScene.updateEmittersRayTracing(&level);///////
    }
    if (Mouse::isButtonPressed(sf::Mouse::Right)) {
        level.removeTile(mousePos);
        lightScene.updateEmittersRayTracing(&level);///////
    }


    ///normalized view
    player.view = eVector2f((Vector2f)Mouse::getPosition(window) - player.getPos()).norm();
}

void Game::logicMovement() {
    // TODO: fix player jumping in air
    if (keys.jumpTime.getElapsedTime().asSeconds() > 0.22) {
        keys.space = false;
        keys.jumpAble = false;
        keys.jumpTime.restart();
    }

    ///process input
    if (keys.right)
        player.velocity.x += heroAcceleration;
    if (keys.left)
        player.velocity.x -= heroAcceleration;
    if (keys.space) {
        player.velocity.y += jumpGravity;
    }
    else {
        player.velocity.y += fallGravity;
    }

    ///set position that player tries to move to
    Vector2f desPos;
    ///first move by x, then apply restrictions
    player.move(Vector2f(player.velocity.x, 0));
    ///process right moving if player moves right
    desPos = player.getPos() + Vector2f(player.velocity.x, 0);
    ///if no intersections, move only horizontally
    if (level.isOnTile(desPos + Vector2f(heroWidth/2 - .1, heroHeight/2 - .1)) ||
        level.isOnTile(desPos + Vector2f(heroWidth/2 - .1, -heroHeight/2 + .1)) ||
        level.isOnTile(desPos + Vector2f(heroWidth/2 - .1, 0))) {

        ///else move right to the obstacle and mirror speed
        player.setPos(Vector2f(((int) player.getPos().x / (int) scale + 1) * scale - heroWidth/2, player.getPos().y));
        player.velocity.x = 0;
    }


    ///process left moving if player moves left
    desPos = player.getPos() + Vector2f(player.velocity.x, 0);

    if (level.isOnTile(desPos + Vector2f(-heroWidth/2 + .1, heroHeight/2 - .1)) ||
        level.isOnTile(desPos + Vector2f(-heroWidth/2 + .1, -heroHeight/2 + .1)) ||
        level.isOnTile(desPos + Vector2f(-heroWidth/2 + .1, 0))) {

        player.setPos(Vector2f(((int) player.getPos().x / (int) scale) * scale + heroWidth/2, player.getPos().y));
        player.velocity.x = 0;
    }

    ///first move by y, then apply restrictions
    player.move(Vector2f(0, player.velocity.y));
    ///process up moving if player moves up
    desPos = player.getPos() + Vector2f(0, player.velocity.y);

    if (level.isOnTile(desPos + Vector2f(heroWidth/2 - .1, -heroHeight/2 + .1)) ||
        level.isOnTile(desPos + Vector2f(-heroWidth/2 + .1, -heroHeight/2 + .1))) {

        keys.space = false;
        keys.jumpAble = false;

        player.setPos(Vector2f(player.getPos().x, ((int) player.getPos().y / (int) scale) * scale + heroHeight/2));
        player.velocity.y = -player.velocity.y * 0.2;
    }

    ///process down moving if player moves down
    desPos = player.getPos() + Vector2f(0, player.velocity.y);

    if (level.isOnTile(desPos + Vector2f(heroWidth/2 - .1, heroHeight/2 - .1)) ||
        level.isOnTile(desPos + Vector2f(-heroWidth/2 + .1, heroHeight/2 - .1)) ) {

        player.setPos(Vector2f(player.getPos().x, ((int) player.getPos().y / (int) scale + 1) * scale - heroHeight/2));
        player.velocity.y = 0;
    }
    if (player.velocity.y == 0)
        keys.jumpAble = true;///////////////


    ///decrease velocity if no input
    if (!keys.right && !keys.left)
        player.velocity = Vector2f(player.velocity.x * 0.88, player.velocity.y);

    ///restrict speed by x and y
    if(abs(player.velocity.x) > maxVelocity) {
        player.velocity.x /= abs(player.velocity.x);
        player.velocity.x *= maxVelocity;
    }

    if(abs(player.velocity.y) > 1.5*maxVelocity) {
        player.velocity.y /= abs(player.velocity.y);
        player.velocity.y *= 1.5*maxVelocity;
    }
}


void Game::logic() {
    lightScene.update();

    level.resetActive();
    lightScene.setActiveTiles(&level);
    level.update();

    //lightScene.updateEmitter(0, eVector2f(player.getPos()), player.view, false);

    logicMovement();

    dump.add("Velocity: (" + std::to_string(player.velocity.x) + " : " + std::to_string(player.velocity.y) + ")");
    dump.add("View: (" + std::to_string(player.view.x) + " : " + std::to_string(player.view.y) + ")");

    if(level.isOnFinish(player.getPos())) {
        keys.isOnFinish = true;
        std::cout << "Finish\n";
    }
    else {
        keys.isOnFinish = false;///!!!
    }



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
