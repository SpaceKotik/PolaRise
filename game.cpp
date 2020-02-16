#include <SFML/Graphics.hpp>
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"

using namespace sf;


extern const int scale;
extern const float delay;
extern const int field_x;
extern const int field_y;
extern const int mines;


Game::Game() {
    gameState = Gameplay;
    //texture.loadFromFile("resourses\\textures.png");
    field_size_x = field_x;
    field_size_y = field_y;


    ContextSettings settings;
    settings.antialiasingLevel = 8;


    window.create(VideoMode(field_x*scale, (field_y+0.1)*scale), "Polarise",
                  Style::Titlebar | Style::Close, settings);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120);




    testLine[1][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[1][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[2][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[2][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[3][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[3][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[0][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[0][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);



    testLine[4][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[4][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[5][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[5][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[6][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[6][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

    testLine[7][0] = Vertex(sf::Vector2f(20.0f, window.getSize().y-20.f), Color::Blue);
    testLine[7][1] = Vertex(sf::Vector2f(window.getSize().x, 0.f), Color::Blue);

}





RenderWindow* Game::getHandle() {   
    return &window;
}

void Game::run() {
    Clock clock;
    //float time = 1;
    while(window.isOpen()) {

        input();
        draw();

        /*if (time < delay) {
            time += clock.restart().asSeconds();
            // /usleep(100);
            continue;
        }
        time = 0;*/

        testRect.rotate(1);

        testLine[0][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(0)), Color::Blue);
        testLine[1][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(1)), Color::Blue);             

        testLine[2][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(2)), Color::Blue);


        testLine[3][1] = Vertex(testRect.physForm.getTransform().transformPoint(testRect.physForm.getPoint(3)), Color::Blue);

        testLine[4][1] = Vertex(Vector2f(0.f, 0.f), Color::Blue);
        testLine[5][1] = Vertex(Vector2f(window.getSize().x, 0.f), Color::Blue);
        testLine[6][1] = Vertex(Vector2f(0.f, window.getSize().y), Color::Blue);
        testLine[7][1] = Vertex(Vector2f(window.getSize().x, window.getSize().y), Color::Blue);
    }
}

void Game::draw() {
    //update_field();
    window.clear(Color(230, 235, 230));
    window.draw(testRect.physForm);
    for (int i = 0; i < 8; ++i) {
        window.draw(testLine[i], 2, Lines);
    }

    /*for (int i = 0; i < field_size_y; i++) {
        for (int j = 0; j < field_size_x; j++) {
            window.draw(field[i][j].tile_v);
        }
    }*/
    //int total_points = get_total_points();
    //window.draw(texts.print_score(total_points));
    window.display();
}

void Game::input() {
        //static FloatRect prev_location = {-100, -100, scale, scale};
        Event event;

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