#include "PauseGUI.h"
#include "Game/include/game.hpp"

void PauseGUI::setup(Game *game) {
    mediator = game;
    window = mediator->getWindow();
    gui = new tgui::Gui{*window};

    tgui::Button::Ptr buttonPause = tgui::Button::create();
    //buttonPause->getRenderer()->setBackgroundColor(Color::Black);
    //buttonPause->getRenderer()->setTextColor(Color::White);
    buttonPause->setSize(100, 50);
    buttonPause->setPosition("(&.width - width)/2", "(&.height - height)/2 - 80");
    buttonPause->setText("unpause");
    buttonPause->setTextSize(20);
    buttonPause->connect("pressed", [this](){
        this->mediator->changeState("Gameplay");
    });

    tgui::Button::Ptr buttonRestart = tgui::Button::create();
    buttonRestart->setSize(100, 50);
    buttonRestart->setPosition("(&.width - width)/2", tgui::bindBottom(buttonPause) + 10);
    buttonRestart->setText("restart");
    buttonRestart->setTextSize(20);
    buttonRestart->connect("pressed", [this](){
        this->mediator->changeState("Gameplay");
        this->mediator->restart();
    });

    tgui::Button::Ptr buttonMenu = tgui::Button::create();
    buttonMenu->setSize(100, 50);
    buttonMenu->setPosition("(&.width - width)/2", tgui::bindBottom(buttonRestart) + 10);
    buttonMenu->setText("menu");
    buttonMenu->setTextSize(20);
    buttonMenu->connect("pressed", [this](){
        this->mediator->changeState("Menu");
    });

    gui->add(buttonPause, "Pause");
    gui->add(buttonRestart, "Restart");
    gui->add(buttonMenu, "Menu");
}

void PauseGUI::draw() {
    gui->draw();
    window->display();
}
