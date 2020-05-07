#include "Game/include/lantern.h"
#include "Game/include/consts.h"
using namespace consts;

Lantern::Lantern() {
    tex.loadFromFile("../Textures/flashLight.png");
    tex.setSmooth(true);
    sprite.setPosition(windowSize.x/2.f, windowSize.y/2.f);
    sprite.setOrigin(64, 64);
    sprite.setTexture(tex);
    sprite.scale(0.25, 0.25);
}
