#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"

#include "consts.h"

using namespace sf;
using namespace consts;

Tile::Tile() {
	physForm.setPosition(window_x/2.f, window_y/2.f);
	physForm.setSize(Vector2f(scale*6, scale*3));
	physForm.setOrigin(Vector2f(physForm.getSize().x/2.f, physForm.getSize().y/2.f));
	physForm.setFillColor(Color::White);
	physForm.setOutlineColor(Color::Black);
	physForm.setOutlineThickness(3);
	isDynamic = false;
}

Tile::Tile(Vector2f pos, bool isRed, bool isBlue) {
	physForm.setPosition(pos);
	physForm.setSize(Vector2f(scale, scale));
	physForm.setFillColor(tileDefaultColor);
	physForm.setOutlineColor(Color(190, 190, 190));
	physForm.setOutlineThickness(0);
	isRed = false;
	isBlue = false;
	isSolidBlue = false;
	isSolidRed = false;
	isDynamic = false;
}

void Tile::rotate(float deg) {
	physForm.rotate(deg);
}

RectangleShape Tile::getRect() {
	return physForm;
}

bool Tile::checkIfBlue() {
	return isBlue;
}

bool Tile::checkIfRed() {
	return isRed;
}

bool Tile::checkIfSolidRed() {
	return isSolidRed;
}

bool Tile::checkIfSolidBlue() {
	return isSolidBlue;
}