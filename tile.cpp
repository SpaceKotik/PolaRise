#include <SFML/Graphics.hpp>
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"


extern const int scale;
extern const float delay;
extern const int field_x;
extern const int field_y;
extern const int mines;

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

void Tile::rotate(float deg) {
	physForm.rotate(deg);

}

RectangleShape Tile::getRect() {
	return physForm;
}