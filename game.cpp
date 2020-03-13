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


/*
static const std::string shaderCode = \
  "uniform vec2 frag_LightOrigin;"\
  "uniform vec3 frag_LightColor;"\
  "uniform float frag_LightAttenuation;"\
  "uniform vec2 frag_ScreenResolution;"\
  "void main(){"\
  " vec2 baseDistance =  gl_FragCoord.xy;"\
  " baseDistance.y = frag_ScreenResolution.y-baseDistance.y;"\
  " vec2 distance=frag_LightOrigin - baseDistance;"\
  " float linear_distance = length(distance);"\
  " float attenuation=1.0/( frag_LightAttenuation*linear_distance + frag_LightAttenuation*linear_distance);"\
  " vec4 lightColor = vec4(frag_LightColor, 1.0);"\
  " vec4 color = vec4(attenuation, attenuation, attenuation, 1.0) * lightColor; gl_FragColor=color;}";
*/

/*
static const std::string shaderCode = \
"uniform sampler2D image;"\
"uniform vec2 resolution;"\
"uniform vec2 dir;"\
"void main(){"\
"vec2 uv = vec2(gl_FragCoord.xy / resolution.xy);"\
"vec4 color = vec4(0.0);"\
"vec2 off1 = vec2(1.3846153846) * dir;"\
"vec2 off2 = vec2(3.2307692308) * dir;"\
"color += texture2D(image, uv) * 0.2270270270;"\
"color += texture2D(image, uv + (off1 / resolution)) * 0.3162162162;"\
"color += texture2D(image, uv - (off1 / resolution)) * 0.3162162162;"\
"color += texture2D(image, uv + (off2 / resolution)) * 0.0702702703;"\
"color += texture2D(image, uv - (off2 / resolution)) * 0.0702702703;"\
"gl_FragColor=color;}";*/

static const std::string shaderCode = \
"uniform sampler2D image;"\
"uniform vec2 resolution;"\
"uniform vec2 dir;"\
"void main(){"\
"vec2 uv = vec2(gl_FragCoord.xy / resolution.xy);"\
"vec4 color = vec4(0.0);"\
"vec2 off1 = vec2(1.411764705882353) * dir;"\
"vec2 off2 = vec2(3.2941176470588234) * dir;"\
"vec2 off3 = vec2(7.176470588235294) * dir;"\
"color += texture2D(image, uv) * 0.1964825501511404;"\
"color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;"\
"color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;"\
"color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;"\
"color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;"\
"color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;"\
"color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;"\
"gl_FragColor=color;}";





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


    window.create(VideoMode(field_x*scale, (field_y+0.0)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);
    window.setPosition(Vector2i(0, 0));
    window.setMouseCursorGrabbed(false);

    level.setField();

    rayTracing.convertTileMapToPolyMap(&level, getHandle());
    rayTracing.convertPolyMapToVertices();  


    myRenderTexture.create(30*40, 30*30);
    spriteWorld.setTexture(myRenderTexture.getTexture());
    spriteWorld.setOrigin(30*40 / 2.f, 30*30 / 2.f);
    spriteWorld.setPosition(30*40 / 2.f, 30*30 / 2.f);


    if(!shader.loadFromMemory(shaderCode, sf::Shader::Fragment)) {
        window.close();
    }
    shader.setParameter("resolution", sf::Vector2f(30*40, 30*30));



}



RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    FPScounter fpsCounter;

    while(window.isOpen()) {
        fpsCounter.update();
        fpsCounter.draw();

        input();
        logic();
        //level.update();
        draw(level, rayTracing);
    }
}




void Game::doRayTracing(RayTracing rayTracing, Vector2f pos, Vector2f view, float lineOfSight, std::mutex *rtLock) {
    
    rayTracing.update(&level, &window, pos, true, view, lineOfSight);
    rtLock->lock();
    window.setActive(true);
    myRenderTexture.draw(rayTracing.createMesh(), BlendAdd);
    //window.draw(temp, BlendAdd);
    //window.draw(rayTracing.createMesh(), BlendAdd);
    rtLock->unlock();
}


void Game::draw(Level level, RayTracing rayTracing) {

    RenderStates renderStates;
    renderStates.blendMode = BlendAdd;

    window.clear(backgorundColor);
    //window.clear();
    //window.clear(Color::Black);
    //myRenderTexture.clear(Color::Transparent);
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
    window.draw(bordersFade, BlendMultiply);





    Vector2f normView, normViewPerp;
    normView.x = 2*hero.view.x/sqrt(hero.view.x*hero.view.x + hero.view.y*hero.view.y);
    normView.y = 2*hero.view.y/sqrt(hero.view.x*hero.view.x + hero.view.y*hero.view.y);
    normViewPerp.x = -normView.y;
    normViewPerp.y = normView.x;
    //process light sources

    myRenderTexture.clear();
    std::mutex block;
    std::thread thr1(&Game::doRayTracing, this, rayTracing,  hero.getPos(), (mousePos - hero.getPos()), hero.lineOfSight, &block);
    std::thread thr2(&Game::doRayTracing, this, rayTracing,  (hero.getPos() + normView + normViewPerp), (mousePos - hero.getPos()), hero.lineOfSight, &block);
    std::thread thr3(&Game::doRayTracing, this, rayTracing,  (hero.getPos() + normView - normViewPerp), (mousePos - hero.getPos()), hero.lineOfSight, &block);
    std::thread thr4(&Game::doRayTracing, this, rayTracing,  (hero.getPos() - normView + normViewPerp), (mousePos - hero.getPos()), hero.lineOfSight, &block);
    std::thread thr5(&Game::doRayTracing, this, rayTracing,  (hero.getPos() - normView - normViewPerp), (mousePos - hero.getPos()), hero.lineOfSight, &block);

    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();
    myRenderTexture.display();
    //Sprite temp;
    //.setTexture(myRenderTexture.getTexture());

    for (int i = 0; i < 2; ++i) {
        shader.setParameter("dir", Vector2f(1, 0));
        shader.setParameter("image", myRenderTexture.getTexture());
        sf::RenderStates states;
        states.shader = &shader;
        //states.blendMode = sf::BlendAdd;
        myRenderTexture.draw(spriteWorld, states);
        myRenderTexture.display();


        shader.setParameter("dir", Vector2f(0, 1));
        shader.setParameter("image", myRenderTexture.getTexture());
        states.shader = &shader;
        myRenderTexture.draw(spriteWorld, states);
        myRenderTexture.display();
    }

    window.draw(spriteWorld, BlendAdd);

    //rayTracing.update(&level, getHandle(), hero.getPos(), true, (mousePos - hero.getPos()), hero.lineOfSight);
    //window.draw(rayTracing.createMesh(), renderStates);

    //draw start and finish
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
    window.draw(lightFade, BlendMultiply);

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

    //hero.view = hero.velocity;
    hero.view = (Vector2f)Mouse::getPosition(window) - hero.getPos();

    if(level.isOnFinish(hero.getPos())) {
        keys.isOnFinish = true;
        std::cout << "Finish\n";
    }
    else {
        keys.isOnFinish = false;///!!!
    }

}