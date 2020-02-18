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


extern const int scale;
extern const float delay;
extern const int field_x;
extern const int field_y;
const float speed = 10;
const float lightSourceSize = 2;


#define NO_INTERSECTION Vector2f(-10, -10);

Game::Game() {
    gameState = Gameplay;
    //texture.loadFromFile("resourses\\textures.png");

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y+0.1)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(true);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);

    mousePos = Vector2f(100, 100);
}



RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    //float time = 1;


    Level level;
    level.setField();
    /*level.addTile(Vector2f(200, 200));
    level.addTile(Vector2f(400, 400));
    level.addTile(Vector2f(600, 600));
    level.addTile(Vector2f(700, 200));*/
    RayTracing rayTracing;
    //rayTracing.update(&level, getHandle());


    while(window.isOpen()) {
        //rayTracing.clear();
        //MouseState mouseState = input();
        MouseState mouseState = input();
        Vector2f mouseNotClicked = NO_INTERSECTION;
        
        if (mouseState.pos != mouseNotClicked) {
            if (mouseState.LeftButtonPressed) {
                level.addTile(mouseState.pos);
            }
            if (mouseState.RightButtonPressed) {
                level.removeTile(mouseState.pos);
            }
        }
        level.update();
        //rayTracing.update(&level, getHandle(), mousePos);
        // /rayTracing.createMesh();

        draw(level, rayTracing);
        //rayTracing.clear();
    }


}

void Game::draw(Level level, RayTracing rayTracing) {
    //update_field();
    //window.clear(Color(230, 235, 230));
    window.clear(Color(10, 10, 10));
    /*window.draw(testRect.physForm);
    for (int i = 0; i < 8; ++i) {
        window.draw(testLine[i], 2, Lines);
    }*/
    for (int i = 0; i < level.getTileCount(); ++i) {
        window.draw(level.getField()->tiles[i].physForm);
    }
   // window.draw(rayTracing.createMesh());


    //BlendMode blendmode;


    //main source
    rayTracing.update(&level, getHandle(), mousePos);
    window.draw(rayTracing.createMesh(), BlendAdd);
    /*for (int i = 0; i < rayTracing.getLineCount()*3; ++i) {
        window.draw(rayTracing.getRays()[i], 2, Lines);
    }*/
    rayTracing.clear();






    rayTracing.update(&level, getHandle(), mousePos + Vector2f(0, lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(0, -lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, 0));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, 0));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();



    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, -lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, -lightSourceSize));
    window.draw(rayTracing.createMesh(), BlendAdd);
    rayTracing.clear();

    //main source moved here
    /*rayTracing.update(&level, getHandle(), mousePos);
    window.draw(rayTracing.createMesh(), BlendAdd);
    for (int i = 0; i < rayTracing.getLineCount()*3; ++i) {
        //window.draw(rayTracing.getRays()[i], 2, Lines);
    }
    rayTracing.clear();*/
    //window.draw(rayTracing.createMesh());
    
    //window.draw(*rayTracing.getMesh());
    

    window.display();
}

MouseState Game::input() {
        MouseState mouseState;
        mouseState.pos = NO_INTERSECTION;

        //Vector2f mouseClickedPos = NO_INTERSECTION;
        Event event;

        while (window.pollEvent(event)) {

            switch (event.type) {
            case Event::Closed:
                window.close();
                break;

            case Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.LeftButtonPressed = true;
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    mouseState.pos = (Vector2f)Mouse::getPosition(window);
                    mouseState.RightButtonPressed = true;
                }
                break;
            default:
                break;



            /*case Event::KeyPressed:

            if(Keyboard::isKeyPressed(Keyboard::Left)) {
                mousePos = mousePos + Vector2f(-speed, 0);
            }
            if(Keyboard::isKeyPressed(Keyboard::Right)) {
                mousePos = mousePos + Vector2f(speed, 0);
            }
            if(Keyboard::isKeyPressed(Keyboard::Up)) {
                mousePos = mousePos + Vector2f(0, speed);
            }
            if(Keyboard::isKeyPressed(Keyboard::Down)) {
                mousePos = mousePos + Vector2f(0, -speed);
            }
            break;*/
            

            
            }
        }
        mousePos = (Vector2f)Mouse::getPosition(window);
        if (Mouse::getPosition(window).x > window.getSize().x)
            mousePos.x = window.getSize().x;
        if (Mouse::getPosition(window).x < 0)
            mousePos.x = 0;
        if (Mouse::getPosition(window).y > window.getSize().y)
            mousePos.y = window.getSize().y;
        if (Mouse::getPosition(window).y < 0)
            mousePos.y = 0;


        return mouseState;
}