#pragma once

#include "GUI/BaseGUI/BaseGUI.h"

class PauseGUI : public BaseGUI {
public:
    PauseGUI() = default;
    void setup(Game *game) override;
    void draw() override;
};
