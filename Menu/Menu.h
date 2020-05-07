#pragma once
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

class Game;

class Menu {
private:
    tgui::Gui* gui;
    sf::RenderWindow* window;
    Game* mediator;
public:
    Menu(Game *game) : mediator(game) {};
    void setup();
    void input();
    void update();
    void draw();
};
