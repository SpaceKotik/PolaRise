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

using namespace sf;

Level::Level() {
	//setting tiles
}

const int tilesCount = 3;

void Level::setField() {
	tileCount = tilesCount;
	field.tile = new Tile[tilesCount];
	for (int i = 0; i < tilesCount; ++i) {
	field.tile[i].physForm.setPosition(field_x*scale/2.f + i*300 - 300, field_y*scale/2.f + i*100-100);
	field.tile[i].physForm.rotate(i*30);
	field.tile[i].physForm.setSize(Vector2f(scale*6, scale*3));
	field.tile[i].physForm.setOrigin(Vector2f(field.tile[i].physForm.getSize().x/2.f, field.tile[i].physForm.getSize().y/2.f));
	field.tile[i].physForm.setFillColor(Color::White);
	field.tile[i].physForm.setOutlineColor(Color::Black);
	field.tile[i].physForm.setOutlineThickness(0);
	}
}

Field* Level::getField() {
	return &field;
}

int Level::getTileCount() {
	return tileCount;
}

void Level::update() {
	for (int i = 0; i < tilesCount; ++i) {
		//field.tile[i].physForm.rotate(-2 + i*2);
	}
}