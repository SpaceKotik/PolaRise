#include "Menu.h"
#include "game.hpp"

void Menu::setup() {
    window = mediator->getWindow();
    gui = new tgui::Gui{*window};

    tgui::Theme blackTheme{"GUIThemes/Black.txt"};

    tgui::Button::Ptr buttonPlay = tgui::Button::create();
    buttonPlay->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(800));
    buttonPlay->setSize(200, 120);
    buttonPlay->setPosition("(&.width - width)/2", "(&.height - height)/2 - 50");
    buttonPlay->setText("Play");
    buttonPlay->setTextSize(35);

    buttonPlay->connect("pressed", [this](){this->mediator->changeState("Gameplay");});
    buttonPlay->setFocusable(false);

    tgui::Button::Ptr buttonExit = tgui::Button::create();
    buttonExit->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(800));
    buttonExit->setSize(95, 40);
    buttonExit->setPosition(tgui::bindLeft(buttonPlay), tgui::bindBottom(buttonPlay) + 10);
    buttonExit->setText("quit");
    buttonExit->setTextSize(15);
    buttonExit->connect("pressed", [this](){this->window->close();});
    buttonExit->setFocusable(false);

    tgui::Button::Ptr buttonSettings = tgui::Button::create();
    buttonSettings->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(800));
    buttonSettings->setSize(95, 40);
    buttonSettings->setPosition(tgui::bindRight(buttonPlay) - buttonSettings->getSize().x, tgui::bindBottom(buttonPlay) + 10);
    buttonSettings->setText("Settings");
    buttonSettings->setTextSize(15);

    //buttonClose->connect("pressed", [this](){this->window.close();});


    gui->add(buttonExit, "buttonExit");
    gui->add(buttonPlay, "buttonPlay");
    gui->add(buttonSettings, "buttonSettings");
}

void Menu::input() {
    Event event{};
    while (window->pollEvent(event)) {
        gui->handleEvent(event);
        switch (event.type) {
            case Event::Closed:
                window->close();
                break;
            case Event::KeyPressed:
                if (event.key.code == Keyboard::Escape) {
                    window->close();
                }
            default:
                break;
        }
    }
}

void Menu::update() {

}

void Menu::draw() {
    window->clear();
    gui->draw();
    window->display();
}
