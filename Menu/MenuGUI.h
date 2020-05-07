#pragma once
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "GUI/BaseGUI.h"

class Game;

class MenuGUI : public BaseGUI {
public:
    MenuGUI() = default;
    void setup(Game *game) override;
    void draw() override;
};
