#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>


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
#define LIGHT_SOURCE_SCALE Vector2f(0.7, 0.7)
#define BORDERS_VISIBILITY_SCALE Vector2f(0.3, 0.3)


class FPScounter {
    public:
        //FPSCounter();
        void update();
        void draw();

    private:
        //sf::Text text;
        //sf::Font font;

        sf::Clock delayTimer;
        sf::Clock fpsTimer;

        float fps = 0;

        int frameCount = 0;
};


void FPScounter::update() {
    frameCount++;

    if (delayTimer.getElapsedTime().asSeconds() > 0.2) {
        fps = frameCount / fpsTimer.restart().asSeconds();
        frameCount = 0;
        delayTimer.restart();
    }
}

//Draws the FPS display to the window
void FPScounter::draw() {
    //m_text.setString("FPS " + std::to_string((int)m_fps));
    //renderer.draw(m_text);
    std::cout << "\033[2J\033[1;1H";
    std::cout << "FPS : " << std::to_string((int)fps) << "\n";
}


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
    window.setPosition(Vector2i(0, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();

    rayTracing.convertTileMapToPolyMap(&level, getHandle());
    rayTracing.convertPolyMapToVertices();
}



RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    FPScounter fpsCounter;

    while(window.isOpen()) {
        //MouseState mouseState = input();
        //Vector2f mouseNotClicked = NO_INTERSECTION;
        fpsCounter.update();
        fpsCounter.draw();
        std::cout << atan2(0, -1) << "\n";

        input();
        logic();
        //level.update();
        draw(level, rayTracing);
    }
}




void doRayTracing(RayTracing rayTracing, Vector2f pos, Vector2f view, float lineOfSight, Level *level, RenderWindow* window, std::mutex *rtLock) {
    rayTracing.update(level, window, pos, true, view, lineOfSight);
    rtLock->lock();
    window->draw(rayTracing.createMesh(), BlendAdd);
    rtLock->unlock();
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

    //add light fade for borders
    Sprite bordersFade;
    bordersFade.setOrigin(lightSourceTextureCenter);
    bordersFade.setTexture(texture);
    bordersFade.setPosition(hero.getPos());
    bordersFade.setScale(BORDERS_VISIBILITY_SCALE);
    //window.draw(bordersFade, BlendMultiply);


    std::mutex block;
    std::thread thr1(doRayTracing, rayTracing, hero.getPos(), (mousePos - hero.getPos()), hero.lineOfSight, &level, &window, &block);
    /*std::thread thr2(doRayTracing, rayTracing, hero.getPos()+ Vector2f(2, 2), (mousePos - hero.getPos()), hero.lineOfSight, &level, &window, &block);
    std::thread thr3(doRayTracing, rayTracing, hero.getPos()+ Vector2f(2, -2), (mousePos - hero.getPos()), hero.lineOfSight, &level, &window, &block);
    std::thread thr4(doRayTracing, rayTracing, hero.getPos()+ Vector2f(-2, 2), (mousePos - hero.getPos()), hero.lineOfSight, &level, &window, &block);
    std::thread thr5(doRayTracing, rayTracing, hero.getPos()+ Vector2f(-2, -2), (mousePos - hero.getPos()), hero.lineOfSight, &level, &window, &block);*/
    //process light sources
    //rayTracing.update(&level, getHandle(), hero.getPos());
    //window.draw(rayTracing.createMesh(), renderStates);
    thr1.join();
    /*thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();*/

    //rayTracing.update(&level, getHandle(), hero.getPos(), true, (mousePos - hero.getPos()), hero.lineOfSight);
    //window.draw(rayTracing.createMesh(), renderStates);

    /*rayTracing.update(&level, getHandle(), hero.getPos() + Vector2f(2, 2));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), hero.getPos() + Vector2f(-2, 2));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), hero.getPos() + Vector2f(2, -2));
    window.draw(rayTracing.createMesh(), renderStates);

    rayTracing.update(&level, getHandle(), hero.getPos() + Vector2f(-2, -2));
    window.draw(rayTracing.createMesh(), renderStates);*/

    for (int i = 0; i < field_x*field_y; ++i) {
        if (level.getState() == Blue) {
            if (level.getField()->tiles[i].typeBlue == 1 || level.getField()->tiles[i].typeBlue == 2) 
                window.draw(level.getField()->tiles[i].physForm);
        }
        else if (level.getState() == Red) {
            if (level.getField()->tiles[i].typeRed == 1 || level.getField()->tiles[i].typeRed == 2) {
                window.draw(level.getField()->tiles[i].physForm);
            }
        }
    }

    

    window.draw(*hero.getPhysForm());


    //add light fade
    Sprite lightFade;
    lightFade.setOrigin(lightSourceTextureCenter);
    lightFade.setTexture(texture);
    lightFade.setPosition(hero.getPos());
    lightFade.setScale(LIGHT_SOURCE_SCALE);
    //window.draw(lightFade, BlendMultiply);


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
            currRay[0].color = Color::Red;
            currRay[1].color = Color::Red;
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
    ;




        
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

                        rayTracing.changeLightColor();
                        rayTracing.convertTileMapToPolyMap(&level, getHandle());
                        rayTracing.convertPolyMapToVertices();

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
                    rayTracing.convertTileMapToPolyMap(&level, getHandle());
                    rayTracing.convertPolyMapToVertices();
        }
        if (Mouse::isButtonPressed(sf::Mouse::Right)) {
            mouseState.pos = (Vector2f)Mouse::getPosition(window);
            mouseState.RightButtonPressed = true;
            level.removeTile(mouseState.pos);
            rayTracing.convertTileMapToPolyMap(&level, getHandle());
            rayTracing.convertPolyMapToVertices();
        }

        //calculate all edges and vertices
        /*rayTracing.convertTileMapToPolyMap(&level, getHandle());
        rayTracing.convertPolyMapToVertices();*/


        

        //mousePos = (Vector2f)Mouse::getPosition(window);
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

    //restrict speed by normalizing velocity vector
    float absVelocity = sqrt(hero.velocity.x*hero.velocity.x + hero.velocity.y*hero.velocity.y);
    if (absVelocity > maxVelocity) {
        hero.velocity.x /= absVelocity;
        hero.velocity.x *= maxVelocity;
        hero.velocity.y /= absVelocity;
        hero.velocity.y *= maxVelocity;
    }

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

    hero.view = hero.velocity;


    if(level.isOnFinish(hero.getPos())) {
        keys.isOnFinish = true;
        std::cout << "Finish\n";
    }
    else {
        keys.isOnFinish = false;///!!!
    }

}