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


#define NO_INTERSECTION Vector2f(-10, -10);

Game::Game() {
    gameState = Gameplay;
    //texture.loadFromFile("resourses\\textures.png");

    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y+0.1)*scale), "PolaRise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120);
}


    //Starting position of rays
    /*for (int i = 0; i < 8; ++i) {
        testLine[i][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
        testLine[i][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);
    }/*

}


/*Line getPartIntersection(Line ray, Line line) {

    // Are they parallel? If so, no intersect
    /*float r_mag = sqrt(ray.dirX*ray.dirX+ray.dirY*ray.dirY);
    float s_mag = sqrt(line.dirX*line.dirX+ line.dirY*line.dirY);
    if(ray.dirX/r_mag == line.dirX/s_mag && ray.dirY/r_mag == line.dirY/s_mag){ // Directions are the same.
        return NO_INTERSECTION;
    }

    // SOLVE FOR T1 & T2
    // ray.startCoord.x+ray.dirX*T1 = s_px+line.startCoord.x*T2 && ray.startCoord.y+ray.dirY*T1 = line.startCoord.y+line.startCoord.y*T2
    // ==> T1 = (s_px+line.startCoord.x*T2-ray.startCoord.x)/ray.dirX = (line.startCoord.y+line.startCoord.y*T2-ray.startCoord.y)/ray.dirY
    // ==> s_px*ray.dirY + line.startCoord.x*T2*ray.dirY - ray.startCoord.x*ray.dirY = line.startCoord.y*ray.dirX + line.startCoord.y*T2*ray.dirX - ray.startCoord.y*ray.dirX
    // ==> T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-s_px))/(line.startCoord.x*ray.dirY - line.startCoord.y*ray.dirX)
    float T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-line.startCoord.x))/(line.dirX*ray.dirY - line.dirY*ray.dirX);
    float T1 = (line.startCoord.x+line.dirX*T2-ray.startCoord.x)/ray.dirX;

    // Must be within parametic whatevers for RAY/SEGMENT
    if(T1<0) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        return tempLine;
    }
    if(T2<0 || T2>1) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        return tempLine;
    }
        

    // Return the POINT OF INTERSECTION
    Line tempLine;
    tempLine.startCoord = Vector2f(ray.startCoord.x+ray.dirX*T1, ray.startCoord.y+ray.dirY*T1);
    tempLine.param = T1;
    return tempLine;
    //return Vector2f(25, 25);
    //return;
}*/


RenderWindow* Game::getHandle() {   
    return &window;
}
///
/*Vector2f getRectPointPos(Tile rect, int point) {
    return rect.getRect().getTransform().transformPoint(rect.getRect().getPoint(point));
}*/
///
void Game::run() {
    Clock clock;
    //float time = 1;


    Level level;
    level.setField();
    RayTracing rayTracing;
    //rayTracing.update(&level, getHandle());


    while(window.isOpen()) {
        //rayTracing.clear();
        input();
        rayTracing.update(&level, getHandle());
        level.update();
        draw(level, rayTracing);
        rayTracing.clear();

        /*if (time < delay) {
            time += clock.restart().asSeconds();
            // /usleep(100);
            continue;
        }
        time = 0;*/
        /*
        /* testRect.rotate(1);

        testLine[0][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(0)), Color::Blue);
        testLine[1][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(1)), Color::Blue);             
        testLine[2][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(2)), Color::Blue);
        testLine[3][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(3)), Color::Blue);


        testLine[4][1] = Vertex(Vector2f(0.f, 0.f), Color::Blue);
        testLine[5][1] = Vertex(Vector2f(window.getSize().x, 0.f), Color::Blue);
        testLine[6][1] = Vertex(Vector2f(0.f, window.getSize().y), Color::Blue);
        testLine[7][1] = Vertex(Vector2f(window.getSize().x, window.getSize().y), Color::Blue);

        //Set ray sorce
        for (int i = 0; i < 8; ++i) {
            testLine[i][0] = Vertex(mousePos, Color::Blue);
        }   
        // check intersections
        for (int i = 0; i < 8; ++i) {
            Line tempLine, tempRay;



            tempRay.startCoord.x = testLine[i][0].position.x;
            tempRay.startCoord.y = testLine[i][0].position.y;

            tempRay.dirX = (testLine[i][1].position.x - tempRay.startCoord.x);
            tempRay.dirY = (testLine[i][1].position.y - tempRay.startCoord.y);
            /*tempRay.param = sqrt((testLine[i][0].position.x - testLine[i][1].position.x)*
                                                                        (testLine[i][0].position.x- testLine[i][1].position.x) +
                                                                        (testLine[i][0].position.y - testLine[i][1].position.y)*
                                                                        (testLine[i][0].position.y - testLine[i][1].position.y));*/



        /*
            Line prevIntersection;
            prevIntersection.param = 10000;///!!!!
            for (int j = 0; j < 4; ++j) {
            //getRectPointPos(testRect, (i+1) % 4).y;
                //getRectPointPos(testRect, j);

                tempLine.startCoord.x = getRectPointPos(testRect, j).x;
                tempLine.startCoord.y = getRectPointPos(testRect, j).y;

                tempLine.dirX = ((getRectPointPos(testRect, (j+1) % 4).x - tempLine.startCoord.x));
                tempLine.dirY = (getRectPointPos(testRect, (j+1) % 4).y - tempLine.startCoord.y);


                /*tempLine.param = sqrt((tempLine.startCoord.x - getRectPointPos(testRect, (j+1) % 4).x)*
                                                                            (tempLine.startCoord.x - getRectPointPos(testRect, (j+1) % 4).x) +
                                                                            (tempLine.startCoord.y - getRectPointPos(testRect, (j+1) % 4).y)*
                                                                            (tempLine.startCoord.y - getRectPointPos(testRect, (j+1) % 4).y));
            //}

                if (getPartIntersection(tempRay, tempLine).startCoord.x != -10 && getPartIntersection(tempRay, tempLine).startCoord.y != -10) {
                    if (getPartIntersection(tempRay, tempLine).param < prevIntersection.param) {
                        testLine[i][1] = Vertex(getPartIntersection(tempRay, tempLine).startCoord, Color::Red);
                        prevIntersection.param = getPartIntersection(tempRay, tempLine).param; 
                    }

                }


                
            }
        }*/

        //draw(level, rayTracing);
    }


}

void Game::draw(Level level, RayTracing rayTracing) {
    //update_field();
    window.clear(Color(230, 235, 230));
    /*window.draw(testRect.physForm);
    for (int i = 0; i < 8; ++i) {
        window.draw(testLine[i], 2, Lines);
    }*/
    for (int i = 0; i < level.getTileCount(); ++i) {
        window.draw(level.getField()->tile[i].physForm);
    }
    for (int i = 0; i < rayTracing.getLineCount() + 4; ++i) {
        window.draw(rayTracing.getRays()[i], 2, Lines);
    }

    window.display();
}

void Game::input() {
        //static FloatRect prev_location = {-100, -100, scale, scale};
        Event event;


        // controlling the position of the ray source
        mousePos = (Vector2f)Mouse::getPosition(window);

        if (Mouse::getPosition(window).x > window.getSize().x)
            mousePos.x = window.getSize().x;
        if (Mouse::getPosition(window).x < 0)
            mousePos.x = 0;
        if (Mouse::getPosition(window).y > window.getSize().y)
            mousePos.y = window.getSize().y;
        if (Mouse::getPosition(window).y < 0)
            mousePos.y = 0;
        ///

        while (window.pollEvent(event)) {

            switch (event.type) {
            case Event::Closed:
                window.close();
                break;


            
            //case Event::MouseButtonPressed:
                /*if (event.mouseButton.button == Mouse::Left) {
                    Tile* curr_tile;
                    if ((curr_tile = find_mouse_clicked()) != NULL && curr_tile->mark == None && !curr_tile->isOpened) {
                        curr_tile->press();
                        curr_tile->update();
                        prev_location = curr_tile->tile_v.getGlobalBounds();
                    }
                }
                else if (event.mouseButton.button == Mouse::Right) {
                    Tile* curr_tile;
                    if ((curr_tile = find_mouse_clicked()) != NULL) {
                        if (!curr_tile->isOpened){
                            if(curr_tile->mark == None)
                                curr_tile->mark = Flag;
                            else if(curr_tile->mark == Flag)
                                curr_tile->mark = Question;
                            else if(curr_tile->mark == Question)
                                curr_tile->mark = None;
                        }
                    }
                }
                else if(event.mouseButton.button == Mouse::Middle){
                   Tile* curr_tile;
                        if ((curr_tile = find_mouse_clicked()) != NULL) {
                            open_obvious(curr_tile->x, curr_tile->y);
                        }
                }
                break;*/


            /*case Event::MouseButtonReleased:
                if (find_mouse_clicked() != NULL) {
                    if (prev_location.left == find_mouse_clicked()->tile_v.getGlobalBounds().left
                        && prev_location.top == find_mouse_clicked()->tile_v.getGlobalBounds().top){
                        if (event.mouseButton.button == Mouse::Left) {

                            Tile* curr_tile;

                            if ((curr_tile = find_mouse_clicked()) != NULL) {
                                open_tile(curr_tile->x, curr_tile->y);
                            }
                        }
                    }
                    else if (prev_location.top != -100 && prev_location.left != -100) {
                        field[(int)(prev_location.top/scale)][(int)(prev_location.left/scale)].press();
                        prev_location.top = -100;
                        prev_location.left = -100;
                    }
                }
                else {
                    field[(int)(prev_location.top/scale)][(int)(prev_location.left/scale)].press();
                }
                break;
            */
            default:
                break;
            }
        }
}