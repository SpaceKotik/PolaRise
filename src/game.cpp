#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <math.h>
#include <unistd.h>

#include "../SmartVector2/SmartVector2f.h"
#include "game.hpp"
#include "tile.hpp"
#include "level.hpp"
#include "rayTracing.hpp"

using namespace sf;

#define NDEBUG true   //show rays, obstacles, etc
#define UPDATESHADERS false //update changes in shaders in real time
#define DOBLUR true
#define DOSHADOW false
#define DUMP false

extern const int scale;
extern const int field_x;
extern const int field_y;

//const float lightSourceSize = 0.5;
extern const float heroRadius;
extern const float heroAcceleration; 
extern const float maxVelocity; 
extern const Color backgorundColor;
extern const Color lightColorRed;
extern const Color lightColorBlue;

#define NO_INTERSECTION Vector2f(-10, -10)
#define LIGHT_SOURCE_SCALE Vector2f(1.7, 1.7)
#define BORDERS_VISIBILITY_SCALE Vector2f(0.3, 0.3)
eVector2f view(10, 0);

class FPScounter {
    public:
        //FPSCounter();
        std::string update();
        //std::string draw();

    private:
        //sf::Text text;
        //sf::Font font;

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
    return "FPS: " + std::to_string((int)fps);
}

Game::Game() {
    gameState = Gameplay;

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(0, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();

    rayTracing.convertTileMapToPolyMap(&level);
    rayTracing.convertPolyMapToVertices();  

    bufferTex.create(field_x*scale, field_y*scale);
    bufferTex.setSmooth(true);
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(field_x*scale / 2.f, field_y*scale / 2.f);
    bufferSprite.setPosition(field_x*scale / 2.f, field_y*scale / 2.f);


    if(!setShaders()) {
        window.close();
    }

    srand(time(NULL));

    lightScene.setShaders(DOBLUR, DOSHADOW);
    lightScene.addEmitter(eVector2f(200, 100), eVector2f(1,1));
    lightScene.addEmitter(eVector2f(900, 100), eVector2f(-1, 1));
    lightScene.updateObstacles(&level);
    lightScene.updateEmittersRayTracing();



}

RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    FPScounter fpsCounter;

    while(window.isOpen()) {
        dump.add(fpsCounter.update());
        input();
        logic();
        draw(level, rayTracing);
        if(DUMP)
            dump.display();
    }
}

void Game::doRayTracing(RayTracing rayTracing, Vector2f pos, Vector2f view, float lineOfSight, Color color, std::mutex *rtLock) {
    
    rayTracing.update(pos, true, view, lineOfSight);
    rtLock->lock();
    //window.setActive(true);
    //bufferTex.setActive(true);
    bufferTex.draw(rayTracing.createMesh(color), BlendAdd);
    bufferTex.display();
    rtLock->unlock();
}

void Game::draw(Level level, RayTracing rayTracing) {
    //load shaders every iteration to apply all changes in real time

    if (UPDATESHADERS)
        setShaders();

    window.clear(backgorundColor);    


    //process light sources in parallel
    Vector2f normViewPerp;
    normViewPerp.x = -hero.view.y/120;
    normViewPerp.y = hero.view.x/120;


    //view = (view - eVector2f(700, 200));
    view.rotate(0.05);
    bufferTex.clear(backgorundColor);
    std::mutex block;
    window.setActive(false);
    //bufferTex.setActive(false);
    std::thread thr1(&Game::doRayTracing, this, rayTracing,  hero.getPos(), (mousePos - hero.getPos()), hero.lineOfSight, lightColorBlue, &block);
    /*std::thread thr2(&Game::doRayTracing, this, rayTracing,  (hero.getPos() + hero.view + normViewPerp),  hero.view + normViewPerp,  hero.lineOfSight,lightColorBlue, &block);
    std::thread thr3(&Game::doRayTracing, this, rayTracing,  (hero.getPos() + hero.view - normViewPerp),  hero.view - normViewPerp, hero.lineOfSight, lightColorBlue, &block);
    std::thread thr4(&Game::doRayTracing, this, rayTracing,  (hero.getPos() - hero.view + normViewPerp),  hero.view + normViewPerp, hero.lineOfSight, lightColorBlue, &block);
    std::thread thr5(&Game::doRayTracing, this, rayTracing,  (hero.getPos() - hero.view - normViewPerp),  hero.view - normViewPerp, hero.lineOfSight, lightColorBlue, &block);
     */
    std::thread thr6(&Game::doRayTracing, this, rayTracing,  Vector2f(700, 200),  view, hero.lineOfSight/2, lightColorRed, &block);
    std::thread thr7(&Game::doRayTracing, this, rayTracing,  Vector2f(200, 200),  view, hero.lineOfSight/2, lightColorRed, &block);

    thr1.join();
    /*thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();*/
    thr6.join();
    thr7.join();
    //window.setActive(true);
    bufferTex.setActive(true);
    bufferTex.display();

    //blur all light
    for (int i = 0; i < 1; ++i) {

        bufferTex.draw(bufferSprite, getStatesBlur(Vector2f(1, 0), bufferTex.getTexture()));
        bufferTex.display();

        bufferTex.draw(bufferSprite, getStatesBlur(Vector2f(0, 1), bufferTex.getTexture()));
        bufferTex.display();

        bufferTex.draw(bufferSprite, getStatesBlur(Vector2f(1, 1), bufferTex.getTexture()));
        bufferTex.display();

        bufferTex.draw(bufferSprite, getStatesBlur(Vector2f(1, -1), bufferTex.getTexture()));
        bufferTex.display();
    }

    //make shadow
    //bufferTex.setActive(true);
    bufferTex.draw(bufferSprite, getStatesShadow(20000, 20000));
    bufferTex.setSmooth(true);
    bufferTex.display();

    //draw to screen from buffer
    //window.setActive(true);
    window.draw(bufferSprite);

//////////////////////
    window.setActive(false);
    lightScene.draw();
    Sprite tempSprite;
    tempSprite.setTexture(lightScene.getTexture());
    tempSprite.setOrigin(field_x*scale / 2.f, field_y*scale / 2.f);
    tempSprite.setPosition(field_x*scale / 2.f, field_y*scale / 2.f);
    window.draw(tempSprite, BlendAdd);
//////////////////////

    //draw all tiles
    RenderStates states = getStatesShadow(30000, 3000);
    states.blendMode = BlendAdd;
    for (int i = 0; i < field_x*field_y; ++i) {
        if (level.getState() == Blue) {
            if (level.getField()->tiles[i].isBlue) {
                window.draw(level.getField()->tiles[i].physForm, states);
            }
        }
        else if (level.getState() == Red) {
            if (level.getField()->tiles[i].isRed) {
                window.draw(level.getField()->tiles[i].physForm, states);
            }
        }
    }

    //draw start and finish
    for (int i = 0; i < field_x*field_y; ++i) {
        if (level.getState() == Blue) {
            if (level.getField()->tiles[i].typeBlue == 1 || level.getField()->tiles[i].typeBlue == 2) {
                window.draw(level.getField()->tiles[i].physForm, BlendMultiply);
            }
        }
        else if (level.getState() == Red) {
            if (level.getField()->tiles[i].typeRed == 1 || level.getField()->tiles[i].typeRed == 2) {
                window.draw(level.getField()->tiles[i].physForm, BlendMultiply);
            }
        }
    }

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

    }

    //draw player
    window.draw(*hero.getPhysForm());




    window.display();    
}

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

//Note: param1 must be about 20000-30000 to make smooth light
//      param2 is inverse to the size of the light circle
//This shader uses kind of gaussian distribution function with different sigmas
RenderStates Game::getStatesShadow(float param1, float param2) {
    RenderStates states;
    states.blendMode = BlendMultiply;
    if (!DOSHADOW)
        return states;

    shaderShadow.setParameter("frag_LightOrigin", hero.getPos());
    shaderShadow.setParameter("frag_ShadowParam1", param1);
    shaderShadow.setParameter("frag_ShadowParam2", param2);
    states.shader = &shaderShadow;
    
    return states;
}

RenderStates Game::getStatesBlur(Vector2f dir, Texture buffer) {
    RenderStates states;
    //states.blendMode = BlendMultiply;
    if (!DOBLUR)
        return states;

    shaderBlur.setParameter("dir", dir);
    shaderBlur.setParameter("image", buffer);
    states.shader = &shaderBlur;
    
    return states;
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
                        rayTracing.convertTileMapToPolyMap(&level);
                        rayTracing.convertPolyMapToVertices();
                            lightScene.updateObstacles(&level);//////////
                            lightScene.updateEmittersRayTracing();///////

                    }
                    else {
                        level.changeState();
                    }
                    
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
                    rayTracing.convertTileMapToPolyMap(&level);
                    rayTracing.convertPolyMapToVertices();
                        lightScene.updateObstacles(&level);//////////
                        lightScene.updateEmittersRayTracing();///////
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            mouseState.pos = (Vector2f)Mouse::getPosition(window);
            mouseState.RightButtonPressed = true;
            level.removeTile(mouseState.pos);
            rayTracing.convertTileMapToPolyMap(&level);
            rayTracing.convertPolyMapToVertices();
                lightScene.updateObstacles(&level);//////////
                lightScene.updateEmittersRayTracing();///////
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