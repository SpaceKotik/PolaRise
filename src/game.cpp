#include <SFML/Graphics.hpp>
#include <iostream>
#include <mutex>
#include <chrono>
#include <math.h>
#include <unistd.h>

#include "../SmartVector2/SmartVector2f.h"
#include "game.hpp"
#include "level.hpp"
#include "rayTracing.hpp"
#include "consts.h"

using namespace sf;
using namespace consts;

//#define NDEBUG true         ///show rays, obstacles, etc
//#define UPDATESHADERS false ///update changes in shaders in real time
//#define DUMP false

#define DOBLUR true
#define DOSHADOW true


#define NO_INTERSECTION Vector2f(-10, -10)

eVector2f view(10, 0);

class FPScounter {
    public:
        std::string update();
    private:
        sf::Clock delayTimer;
        sf::Clock fpsTimer;
        float fps = 0;
        int frameCount = 0;
};

std::string FPScounter::update() {
    frameCount++;

    if (delayTimer.getElapsedTime().asSeconds() > 0.2) {
        fps = frameCount / fpsTimer.restart().asSeconds();
        frameCount = 0;
        delayTimer.restart();

    }
    return " FPS: " + std::to_string((int)fps);
}

Game::Game() {
    gameState = Gameplay;

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(window_x, window_y), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(0, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();

    bufferTex.create(window_x, window_y);
    bufferTex.setSmooth(true);
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(window_x / 2.f, window_y / 2.f);
    bufferSprite.setPosition(window_x / 2.f, window_y / 2.f);

    if(!lightScene.setShaders(DOBLUR, DOSHADOW))
       window.close();
    lightScene.addEmitter(eVector2f(200, 100), eVector2f(1,1), true, false);
    //lightScene.addEmitter(eVector2f(900, 100), eVector2f(-1, 1), true, false);
    //lightScene.addEmitter(eVector2f(600, 500), eVector2f(-1, 1), true, false);




    lightScene.updateEmittersRayTracing(&level);
}

RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    FPScounter fpsCounter;

    while(window.isOpen()) {
        window.setTitle("Polarise" + fpsCounter.update());
        input();
        logic();
        draw();
    }
}

void Game::draw() {

    window.clear(backgroundColor);
    bufferTex.clear(backgroundColor);


    view.rotate(0.05);
//////////////////////
    window.setActive(false);
    lightScene.updateEmitter(0, eVector2f(hero.getPos()), hero.view, false);
    //lightScene.updateEmitter(1, eVector2f(Vector2f(900, 100)), eVector2f(1, 1), false);
    //lightScene.updateEmitter(2, eVector2f(600, 600), view, true);

    lightScene.draw();
    Sprite tempSprite;
    tempSprite.setTexture(lightScene.getTexture());
    tempSprite.setOrigin(window_x / 2.f, window_y / 2.f);
    tempSprite.setPosition(window_x / 2.f, window_y / 2.f);
    window.draw(tempSprite, BlendAdd);
//////////////////////

    ///pixelate everyrhing
    if (true) {
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
        if (level.getState() == Blue) {
            if (level.getField()->tiles[i].isBlue) {
                window.draw(level.getField()->tiles[i].physForm, BlendAdd);
            }
        }
        else if (level.getState() == Red) {
            if (level.getField()->tiles[i].isRed) {
                window.draw(level.getField()->tiles[i].physForm, BlendAdd);
            }
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
        rayTracing.update(hero.getPos(), true, hero.view, hero.lineOfSight);
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
    window.draw(*hero.getPhysForm());
    window.display();

}

MouseState Game::input() {
    MouseState mouseState;
    mouseState.pos = NO_INTERSECTION;
        
        Vector2f desPos;
        Event event;

        while (window.pollEvent(event)) {

            switch (event.type) {
            case Event::Closed:
                window.close();
                break;

            case Event::MouseButtonPressed:
                
                break;
            case Event::KeyPressed:             
                //if(Keyboard::isKeyPressed(Keyboard::Space)) {
                if(event.key.code == Keyboard::Space) {

                    level.changeState();

                    if (!level.isOnTile(hero.getPos() + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(hero.getPos() + Vector2f(heroRadius-.1, -heroRadius+.1))
                        && !level.isOnTile(hero.getPos() + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(hero.getPos() + Vector2f(-heroRadius+.1, -heroRadius+.1))) {

                        ///rayTracing.changeLightColor();
                        //updateObstacles(&level);

                            lightScene.updateEmittersRayTracing(&level);///////
                    }
                    else {
                        level.changeState();
                    }
                    
                }

                if(event.key.code == Keyboard::LShift) {
                    lightScene.addEmitter( eVector2f(Vector2f(Mouse::getPosition(window))), eVector2f(1, 1), false, true);
                    lightScene.updateEmittersRayTracing(&level);
                }
                if(event.key.code == Keyboard::LControl) {
                    lightScene.deleteEmitter(Vector2f(Vector2f(Mouse::getPosition(window))));
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

            default:
                break;                        
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }


        Vector2i mousePosition = Mouse::getPosition(window);
        mousePos = (Vector2f)Mouse::getPosition(window);

        if (mousePosition.x < 0 || mousePosition.x > window.getSize().x || mousePosition.y < 0 || mousePosition.y > window.getSize().y)
            return mouseState;

        if (Mouse::isButtonPressed(sf::Mouse::Left)) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.LeftButtonPressed = true;
                    level.addTile(mouseState.pos);
                    //updateObstacles(&level);
                        lightScene.updateEmittersRayTracing(&level);///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            mouseState.pos = (Vector2f)Mouse::getPosition(window);
            mouseState.RightButtonPressed = true;
            level.removeTile(mouseState.pos);
            //updateObstacles(&level);
                lightScene.updateEmittersRayTracing(&level);///////
        }

        if (Mouse::getPosition(window).x > window.getSize().x)
            mouseState.pos.x = window.getSize().x-2;
        if (Mouse::getPosition(window).x < 0)
            mouseState.pos.x = 2;
        if (Mouse::getPosition(window).y > window.getSize().y)
            mouseState.pos.y = window.getSize().y-2;
        if (Mouse::getPosition(window).y < 0)
            mouseState.pos.y = 2;

        return mouseState;
}

void Game::logic() {
    Vector2f desPos;

    //process input
    if (keys.right == true) 
        hero.velocity.x += heroAcceleration;
    if (keys.left == true) 
        hero.velocity.x -= heroAcceleration;
    if (keys.up == true)
        hero.velocity.y -= heroAcceleration;
    if (keys.down == true) 
        hero.velocity.y += heroAcceleration;
    

    //process right moving if player moves right
    desPos = hero.getPos() + Vector2f(hero.velocity.x , 0);
    //if no intersections, move onle horizontally
    if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1)) && hero.velocity.x >= 0) {
            
        hero.move(Vector2f(hero.velocity.x, 0));
    }
    //else move right to the obstacle and mirror speed
    else {
        if (hero.velocity.x > 0) {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale + 1)*scale - heroRadius , hero.getPos().y));
            hero.velocity.x = -hero.velocity.x * 0.2;
        }
    }


    //process left moving if player moves left
    desPos = hero.getPos() + Vector2f(hero.velocity.x , 0);


    if (!level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1)) && hero.velocity.x <= 0) {

        hero.move(Vector2f(hero.velocity.x, 0));
    }
    else {
        if (hero.velocity.x < 0) {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale)*scale + heroRadius , hero.getPos().y));
            hero.velocity.x = -hero.velocity.x * 0.2;
        }
    }

    
    //process up moving if player moves up
    desPos = hero.getPos() + Vector2f(0, hero.velocity.y);

    if ( !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1)) && hero.velocity.y <= 0) {

        hero.move(Vector2f(0, hero.velocity.y));
    }
    else {
        if (hero.velocity.y < 0) {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale)*scale + heroRadius ));
            hero.velocity.y = -hero.velocity.y * 0.2;
        }

    }


    //process down moving if player moves down
    desPos = hero.getPos() + Vector2f(0, hero.velocity.y);

    if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1))&& hero.velocity.y >= 0) {
        hero.move(Vector2f(0, hero.velocity.y));
    }
    else {
        if (hero.velocity.y > 0) {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale+1)*scale -heroRadius ));
            hero.velocity.y = -hero.velocity.y * 0.2;
        }
    }

    //decrease veloicty if no input
    if (keys.right == false && keys.left == false && keys.up == false && keys.down == false)
        hero.velocity = Vector2f(hero.velocity.x * 0.88, hero.velocity.y * 0.88);

    //restrict speed by normalizing velocity vector
    float absVelocity = sqrt(hero.velocity.x*hero.velocity.x + hero.velocity.y*hero.velocity.y);
    if (absVelocity > maxVelocity) {
        hero.velocity.x /= absVelocity;
        hero.velocity.x *= maxVelocity;
        hero.velocity.y /= absVelocity;
        hero.velocity.y *= maxVelocity;
    }
    if (absVelocity < 0.001) {
        hero.velocity = Vector2f(0, 0);
    }
    hero.view = (Vector2f)Mouse::getPosition(window) - hero.getPos();
    hero.view = Vector2f(hero.view.x/sqrt(hero.view.x*hero.view.x + hero.view.y*hero.view.y), hero.view.y/sqrt(hero.view.x*hero.view.x + hero.view.y*hero.view.y));

    dump.add("Velocity: (" + std::to_string(hero.velocity.x) + " : " + std::to_string(hero.velocity.y) + ")");    
    dump.add("View: (" + std::to_string(hero.view.x) + " : " + std::to_string(hero.view.y) + ")");

    if(level.isOnFinish(hero.getPos())) {
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

    shaderShadow.setUniform("frag_LightOrigin", hero.getPos());
    shaderShadow.setUniform("frag_ShadowParam1", param1);
    shaderShadow.setUniform("frag_ShadowParam2", param2);
    states.shader = &shaderShadow;

    return states;
}
 */