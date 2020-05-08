#include "Game/include/tile.hpp"
#include <iostream>
using namespace sf;
using namespace consts;

Tile::Tile() {
	physForm.setPosition(windowSize.x/2.f, windowSize.y/2.f);
	physForm.setSize(Vector2f(scale*6, scale*3));
	physForm.setOrigin(Vector2f(physForm.getSize().x/2.f, physForm.getSize().y/2.f));
	physForm.setFillColor(Color::White);
	physForm.setOutlineColor(Color::Black);
	physForm.setOutlineThickness(3);


}

Tile::Tile(Vector2f pos, TileType _type) {
	physForm.setPosition(pos);
	physForm.setSize(Vector2f(scale, scale));
	physForm.setFillColor(tileDefaultColor);
	physForm.setOutlineColor(Color(190, 190, 190));
	physForm.setOutlineThickness(0);
    type = _type;
	isActive = false;
}

RectangleShape Tile::getRect() {
	return physForm;
}

bool Tile::isSolid() {
    if (type == Finish)
        return false;
    return (type != Void) && !(type == Dynamic && !isActive);

}

bool Tile::isLightAbsorb() {
    if (type == Finish)
        return false;
    if (type == Dynamic && underPlayer)
        return false;
    return (type != Void && !underPlayer);
}

bool Tile::isDrawable() {
    if (type == Standart || type == Deadly || type == Finish)
        return true;
    if (!underPlayer && type == Dynamic && isActive)
        return true;
    return false;
}

bool Tile::isDeadly() {
    return type == Deadly;
}

bool Tile::isFinish() {
    return type == Finish;
}

bool Tile::isUnderPlayer() {
    return underPlayer;
}

