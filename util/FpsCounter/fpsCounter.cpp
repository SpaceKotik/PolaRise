#include <SFML/Graphics.hpp>
#include "fpsCounter.h"

std::string FPScounter::get() {
    frameCount++;
    if (delayTimer.getElapsedTime().asSeconds() > 0.2) {
        fps = frameCount / fpsTimer.restart().asSeconds();
        frameCount = 0;
        delayTimer.restart();

    }
    return " FPS: " + std::to_string((int)fps);
}