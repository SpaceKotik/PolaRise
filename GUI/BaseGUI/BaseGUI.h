#pragma once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

class Game;

class BaseGUI {
protected:
    tgui::Gui* gui;
    sf::RenderWindow* window;
    Game* mediator;
public:
    BaseGUI() = default;
    virtual void setup(Game *game) {};
    virtual void input();
    virtual void update() {};
    virtual void draw() {};
};