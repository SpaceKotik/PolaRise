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



#define NO_INTERSECTION Vector2f(-10, -10)
#define LIGHT_SOURCE_SCALE Vector2f(1.5, 1.5)

Game::Game() {
    gameState = Gameplay;
    texture.loadFromFile("Textures/lightMask.png");
    texture.setSmooth(true);

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y+0.1)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(true);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    mousePos = Vector2f(100, 100);
}



RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;

    Level level;
    level.setField();

    RayTracing rayTracing;
    rayTracing.convertTileMapToPolyMap(&level, getHandle());
    rayTracing.convertPolyMapToVertices();

    while(window.isOpen()) {

        MouseState mouseState = input();
        Vector2f mouseNotClicked = NO_INTERSECTION;
        
        if (mouseState.pos != mouseNotClicked) {
            if (mouseState.LeftButtonPressed) {
                level.addTile(mouseState.pos);
                
            }
            if (mouseState.RightButtonPressed) {
                level.removeTile(mouseState.pos);
            }
            rayTracing.convertTileMapToPolyMap(&level, getHandle());
            rayTracing.convertPolyMapToVertices();
        }
        level.update();

        draw(level, rayTracing);
    }
}

void Game::draw(Level level, RayTracing rayTracing) {

    RenderStates renderStates;
    renderStates.blendMode = BlendAdd;

    window.clear(Color(10, 10, 10));

    //process light sources
    rayTracing.update(&level, getHandle(), mousePos);
    window.draw(rayTracing.createMesh(), renderStates);

    /*rayTracing.update(&level, getHandle(), mousePos + Vector2f(0, lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(0, -lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, 0));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, 0));
    window.draw(rayTracing.createMesh(), renderStates);*/

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(-lightSourceSize, -lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), mousePos + Vector2f(lightSourceSize, -lightSourceSize));
    window.draw(rayTracing.createMesh(), renderStates);

    //add light fade
    Sprite lightFade;
    lightFade.setOrigin(lightSourceTextureCenter);
    lightFade.setTexture(texture);
    lightFade.setPosition(mousePos);
    lightFade.setScale(LIGHT_SOURCE_SCALE);
    window.draw(lightFade, BlendMultiply);

    //draw all tiles

    for (int i = 0; i < field_x*field_y; ++i) {
        if (level.getField()->tiles[i].isBlue) {
            window.draw(level.getField()->tiles[i].physForm);
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
        for (int i = 0; i < rayTracing.raysVertex.size(); ++i) {
            Vertex currRay[2];
            currRay[0].position = rayTracing.raysVertex.at(i)[0].position;
            currRay[1].position = rayTracing.raysVertex.at(i)[1].position;
            currRay[0].color = Color::White;
            currRay[1].color = Color::White;
            window.draw(currRay, 2, Lines);
        }
    }

    window.display();    
}

MouseState Game::input() {
        MouseState mouseState;
        mouseState.pos = NO_INTERSECTION;

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