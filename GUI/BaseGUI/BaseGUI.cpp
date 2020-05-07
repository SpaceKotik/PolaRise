#include "BaseGUI.h"

using namespace sf;

void BaseGUI::input() {
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