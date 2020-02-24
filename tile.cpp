#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"


extern const int scale;
extern const int field_x;
extern const int field_y;

using namespace sf;

Tile::Tile() {
	physForm.setPosition(field_x*scale/2.f, field_y*scale/2.f);
	physForm.setSize(Vector2f(scale*6, scale*3));
	physForm.setOrigin(Vector2f(physForm.getSize().x/2.f, physForm.getSize().y/2.f));
	physForm.setFillColor(Color::White);
	physForm.setOutlineColor(Color::Black);
	physForm.setOutlineThickness(3);	
}

Tile::Tile(Vector2f pos, bool isRed, bool isBlue) {
	physForm.setPosition(pos);
	physForm.setSize(Vector2f(scale, scale));
	physForm.setFillColor(Color::White);
	physForm.setOutlineColor(Color(190, 190, 190));
	physForm.setOutlineThickness(2);
	isRed = false;
	isBlue = false;
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