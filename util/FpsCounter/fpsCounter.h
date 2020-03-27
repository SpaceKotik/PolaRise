#pragma once

#include <SFML/Graphics.hpp>

class FPScounter {
public:
    std::string get();
private:
    sf::Clock delayTimer;
    sf::Clock fpsTimer;
    float fps = 0;
    int frameCount = 0;
};

