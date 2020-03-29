#include "tile.hpp"

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

bool Tile::checkIfSolid() {
    return (type != Void) && !(type == Dynamic && !isActive);

}

bool Tile::checkIfLightAbsorb() {
    return (type != Void) ;
}
