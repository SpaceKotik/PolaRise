#pragma once

#include <util/SmartVector2/SmartVector2f.h>

class Lantern {
private:
    Texture tex;
    eVector2f pos;
    bool isPicked = false;
public:
    Lantern();

    friend class Game;

    Sprite sprite;
};
