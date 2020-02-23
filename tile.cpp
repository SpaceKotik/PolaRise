#include <SFML/Graphics.hpp>
//#include <windows.h>
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
	this->physForm.setPosition(field_x*scale/2.f, field_y*scale/2.f);
	//this->physForm.rotate(30);
	this->physForm.setSize(Vector2f(scale*6, scale*3));
	this->physForm.setOrigin(Vector2f(physForm.getSize().x/2.f, physForm.getSize().y/2.f));
	this->physForm.setFillColor(Color::White);
	this->physForm.setOutlineColor(Color::Black);
	this->physForm.setOutlineThickness(3);	

}

Tile::Tile(Vector2f pos, bool isRed, bool isBlue) {
	this->physForm.setPosition(pos);
	this->physForm.setSize(Vector2f(scale, scale));
	//this->physForm.setOrigin(Vector2f(physForm.getSize().x/2.f, physForm.getSize().y/2.f));
	//this->physForm.setFillColor(Color::Blue);
	this->physForm.setFillColor(Color::White);
	this->physForm.setOutlineColor(Color(190, 190, 190));
	this->physForm.setOutlineThickness(2);
	this->isRed = false;
	this->isBlue = false;
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