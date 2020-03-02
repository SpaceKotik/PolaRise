#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

#include <math.h>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"
#include "level.hpp"
#include "rayTracing.hpp"

using namespace sf;

#define NDEBUG true

extern const int scale;
extern const int field_x;
extern const int field_y;
extern const Vector2f lightSourceTextureCenter;

const float lightSourceSize = 0.5;
extern const float heroRadius;
extern const float heroAcceleration; 
extern const float maxVelocity;
extern const float heroVelocity;
extern const Color backgorundColor;


#define NO_INTERSECTION Vector2f(-10, -10)
#define LIGHT_SOURCE_SCALE Vector2f(0.8, 0.8)
#define BORDERS_VISIBILITY_SCALE Vector2f(0.4, 0.4)


Game::Game() {
    gameState = Gameplay;
    texture.loadFromFile("Textures/lightMask.png");
    texture.setSmooth(true);

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y+0.1)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);

    mousePos = Vector2f(100, 100);


    level.setField();

    rayTracing.convertTileMapToPolyMap(&level, getHandle());
    rayTracing.convertPolyMapToVertices();
}



RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;

    while(window.isOpen()) {
        //MouseState mouseState = input();
        //Vector2f mouseNotClicked = NO_INTERSECTION;

        input();
        logic();
        //level.update();
        draw(level, rayTracing);
    }
}

void Game::draw(Level level, RayTracing rayTracing) {

    RenderStates renderStates;
    renderStates.blendMode = BlendAdd;

    window.clear(backgorundColor);


    //draw all tiles

    for (int i = 0; i < field_x*field_y; ++i) {
        if (level.getState() == Blue) {
            if (level.getField()->tiles[i].isBlue) {
                window.draw(level.getField()->tiles[i].physForm);
            }
        }
        else if (level.getState() == Red) {
            if (level.getField()->tiles[i].isRed) {
                window.draw(level.getField()->tiles[i].physForm);
            }
        }
    }


    //Draw visible edges
    /*for (int i = 0; i < rayTracing.edges.size(); ++i) {
        Vertex currLine[2];
        currLine[0].position = rayTracing.edges.at(i).startCoord;
        currLine[1].position = rayTracing.edges.at(i).startCoord + Vector2f(rayTracing.edges.at(i).dirX, rayTracing.edges.at(i).dirY);
        currLine[0].color = Color::White;
        currLine[1].color = Color::White;
        window.draw(currLine, 2, Lines);
    }*/


     //add light fade for bor
    Sprite bordersFade;
    bordersFade.setOrigin(lightSourceTextureCenter);
    bordersFade.setTexture(texture);
    bordersFade.setPosition(hero.getPos());
    bordersFade.setScale(BORDERS_VISIBILITY_SCALE);
    //window.draw(bordersFade, BlendMultiply);


    //process light sources
    rayTracing.update(&level, getHandle(), hero.getPos());
    window.draw(rayTracing.createMesh(), renderStates);
    window.draw(*hero.getPhysForm());


    //add light fade
    Sprite lightFade;
    lightFade.setOrigin(lightSourceTextureCenter);
    lightFade.setTexture(texture);
    lightFade.setPosition(hero.getPos());
    lightFade.setScale(LIGHT_SOURCE_SCALE);
    ///window.draw(lightFade, BlendMultiply);


    /*rayTracing.update(&level, getHandle(), mousePos);
    window.draw(rayTracing.createMesh(), renderStates);*/


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
        for (int i = 0; i < rayTracing.raysVertex.size(); ++i) {
            Vertex currRay[2];
            currRay[0].position = rayTracing.raysVertex.at(i)[0].position;
            currRay[1].position = rayTracing.raysVertex.at(i)[1].position;
            currRay[0].color = Color::White;
            currRay[1].color = Color::White;
            window.draw(currRay, 2, Lines);
        }
        //Sprite borders1;
        //borders1.setTexture(borderTexture.getTexture());

        //RenderStates renderStatesBorder;
        //renderStatesBorder.texture = &borderTexture.getTexture();
        //renderStatesBorder.blendMode = BlendMode(BlendMode::DstColor, BlendMode::Zero);
        //renderStatesBorder.blendMode = BlendMode(BlendMode::One, BlendMode::Zero);
        //window.draw(rayTracing.createVisibleBorders());
        //window.draw(borders1, BlendMultiply);
    }

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
                /*if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.LeftButtonPressed = true;
                    level.addTile(mouseState.pos);
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.RightButtonPressed = true;
                    level.removeTile(mouseState.pos);
                }
                rayTracing.convertTileMapToPolyMap(&level, getHandle());
                rayTracing.convertPolyMapToVertices();*/
                break;
            case Event::KeyPressed:

                
                //if(Keyboard::isKeyPressed(Keyboard::Space)) {
                if(event.key.code == sf::Keyboard::Space) {

                    level.changeState();

                    if (!level.isOnTile(hero.getPos() + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(hero.getPos() + Vector2f(heroRadius-.1, -heroRadius+.1))
                        && !level.isOnTile(hero.getPos() + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(hero.getPos() + Vector2f(-heroRadius+.1, -heroRadius+.1))) {

                        rayTracing.changeLightColor();
                        rayTracing.convertTileMapToPolyMap(&level, getHandle());
                        rayTracing.convertPolyMapToVertices();


                    }
                    else {
                        level.changeState();
                    }
                    
                }


                if(event.key.code == sf::Keyboard::D) {
                    keys.right = 1;
                }

                if(event.key.code == sf::Keyboard::A) {
                    keys.left = 1;
                }

                if(event.key.code == sf::Keyboard::W) {
                    keys.up = 1;
                }

                if(event.key.code == sf::Keyboard::S) {
                    keys.down= 1;
                }
                
                break;

            case Event::KeyReleased:
                 if(event.key.code == sf::Keyboard::D) {
                    keys.right = 0;
                }

                if(event.key.code == sf::Keyboard::A) {
                    keys.left = 0;
                }

                if(event.key.code == sf::Keyboard::W) {
                    keys.up = 0;
                }

                if(event.key.code == sf::Keyboard::S) {
                    keys.down= 0;
                }

            default:
                break;                        
            }
        }



        if (Mouse::isButtonPressed(sf::Mouse::Left)) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.LeftButtonPressed = true;
                    level.addTile(mouseState.pos);
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            mouseState.pos = (Vector2f)Mouse::getPosition(window);
            mouseState.RightButtonPressed = true;
            level.removeTile(mouseState.pos);
        }
        rayTracing.convertTileMapToPolyMap(&level, getHandle());
        rayTracing.convertPolyMapToVertices();




        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }


        mousePos = (Vector2f)Mouse::getPosition(window);
        if (Mouse::getPosition(window).x > window.getSize().x)
            mousePos.x = window.getSize().x-2;
        if (Mouse::getPosition(window).x < 0)
            mousePos.x = 2;
        if (Mouse::getPosition(window).y > window.getSize().y)
            mousePos.y = window.getSize().y-2;
        if (Mouse::getPosition(window).y < 0)
            mousePos.y = 2;


        return mouseState;
}


void Game::logic() {
    
    /*if (heroMoves.right == true) {
        desPos = hero.getPos() + hero.velocity + Vector2f(heroAcceleration, 0);

        if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1))
            && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1))) {
                //hero.move(Vector2f(heroSpeed, 0)); 
                hero.velocity += Vector2f(heroAcceleration, 0);
                if (sqrt((hero.velocity.x*hero.velocity.x) + (hero.velocity.y*hero.velocity.y) > maxVelocity))
                    hero.velocity -= Vector2f(heroAcceleration, 0);
                hero.move(hero.velocity);
        }
        else {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale + 1)*scale - heroRadius , hero.getPos().y));
            hero.velocity.x = 0;
        }
    }

    if (heroMoves.left == true) {
        desPos = hero.getPos() + hero.velocity + Vector2f(-heroAcceleration, 0);

        if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1))
            && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1))) {
                //hero.move(Vector2f(-heroSpeed, 0));
                hero.velocity += Vector2f(-heroAcceleration, 0);
                if (sqrt((hero.velocity.x*hero.velocity.x) + (hero.velocity.y*hero.velocity.y) > maxVelocity))
                    hero.velocity -= Vector2f(-heroAcceleration, 0);
                hero.move(hero.velocity);
        }
        else {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale)*scale + heroRadius , hero.getPos().y));
            hero.velocity.x = 0;
        }
    }

    if (heroMoves.up == true) {
        desPos = hero.getPos() + hero.velocity + Vector2f(0, -heroAcceleration);

        if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1))
            && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1))) {


            hero.velocity += Vector2f(0, -heroAcceleration);
            if (sqrt((hero.velocity.x*hero.velocity.x) + (hero.velocity.y*hero.velocity.y) > maxVelocity))
                hero.velocity -= Vector2f(0, -heroAcceleration);
            hero.move(hero.velocity);
        }
        else {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale)*scale + heroRadius ));
            hero.velocity.y = 0;

        }
    }

    if (heroMoves.down == true) {
         desPos = hero.getPos() + hero.velocity + Vector2f(0, heroAcceleration);

        if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1))
            && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1))) {
            //hero.move(Vector2f(0, heroSpeed));

            hero.velocity += Vector2f(0, heroAcceleration);
            if (sqrt((hero.velocity.x*hero.velocity.x) + (hero.velocity.y*hero.velocity.y) > maxVelocity))
                hero.velocity -= Vector2f(0, heroAcceleration);
            hero.move(hero.velocity);

        }
        else {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale+1)*scale -heroRadius ));
            hero.velocity.y = 0;
        }
    }*/

    Vector2f desPos;
    Vector2f heroVel = Vector2f(0, 0);






    if (keys.right == true) 
        hero.velocity.x += heroAcceleration;
    desPos = hero.getPos() + Vector2f(hero.velocity.x , 0);

    if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1)) && hero.velocity.x >= 0) {
            
        hero.move(Vector2f(hero.velocity.x, 0));
        //heroVel += Vector2f(heroVelocity, 0);
    }
    else {
        if (hero.velocity.x >= 0) {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale + 1)*scale - heroRadius , hero.getPos().y));
            hero.velocity.x = 0;
        }
    }



    

    if (keys.left == true) 
        hero.velocity.x -= heroAcceleration;
    desPos = hero.getPos() + Vector2f(hero.velocity.x , 0);


    if (!level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1)) && hero.velocity.x <= 0) {

        hero.move(Vector2f(hero.velocity.x, 0));
        //heroVel += Vector2f(-heroVelocity, 0);
    }
    else {
        if (hero.velocity.x <= 0) {
            hero.setPos(Vector2f(((int)hero.getPos().x/(int)scale)*scale + heroRadius , hero.getPos().y));
            hero.velocity.x = 0;
        }
    }





    if (keys.up == true)
        hero.velocity.y -= heroAcceleration;
    desPos = hero.getPos() + Vector2f(0, hero.velocity.y);

    if ( !level.isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1)) && hero.velocity.y <= 0) {

        hero.move(Vector2f(0, hero.velocity.y));
        //heroVel += Vector2f(0, -heroVelocity);
    }
    else {
        if (hero.velocity.y <= 0) {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale)*scale + heroRadius ));
            hero.velocity.y = 0;
        }

    }



    

    if (keys.down == true) 
        hero.velocity.y += heroAcceleration;
    desPos = hero.getPos() + Vector2f(0, hero.velocity.y);

    if (!level.isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !level.isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1))&& hero.velocity.y >= 0) {

        hero.move(Vector2f(0, hero.velocity.y));
        //heroVel += Vector2f(0, heroVelocity);

    }
    else {
        if (hero.velocity.y >= 0) {
            hero.setPos(Vector2f(hero.getPos().x, ((int)hero.getPos().y/(int)scale+1)*scale -heroRadius ));
            hero.velocity.y = 0;
        }
    }
    

    //restrict speed
    /*float absVelocity = sqrt(heroVel.x*heroVel.x + heroVel.y*heroVel.y);
    if (absVelocity > heroVelocity+1) {
        heroVel.x /= absVelocity;
        heroVel.x *= heroVelocity;
        heroVel.y /= absVelocity;
        heroVel.y *= heroVelocity;
    }
    hero.move(heroVel);*/

}