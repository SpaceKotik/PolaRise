#include <SFML/Graphics.hpp>
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "game.hpp"
#include "tile.hpp"
#include <iostream>
#include <stdlib.h>


extern const int scale;
extern const int field_x;
extern const int field_y;

using namespace sf;

Level::Level() {
	//setting tiles
}

//const int tilesCount = 4;

void Level::setField() {
	tileCount = 0;
	/*tileCount = tilesCount;
	//field.tile = new Tile[tilesCount];
	for (int i = 0; i < tilesCount; ++i) {
	//field.tile[i].physForm.setPosition(field_x*scale/2.f + i*250 - 300, field_y*scale/2.f + i*50-100);
	field.tile[i].physForm.setPosition(field_x*scale/2.f + i*scale*8 - 300, field_y*scale/2.f + i*scale*3));
	//field.tile[i].physForm.rotate(i*30);
	field.tile[i].physForm.setSize(Vector2f(scale*8, scale*3));
	//field.tile[i].physForm.setSize(Vector2f(20, 5));
	field.tile[i].physForm.setOrigin(Vector2f(field.tile[i].physForm.getSize().x/2.f, field.tile[i].physForm.getSize().y/2.f));
	field.tile[i].physForm.setFillColor(Color(70, 70, 70));
	//field.tile[i].physForm.setOutlineColor(Color::Black);
	field.tile[i].physForm.setOutlineThickness(0);
	}*/
}

Field* Level::getField() {
	return &field;
}

int Level::getTileCount() {
	return tileCount;
}

void Level::update() {
	for (int i = 0; i < tileCount; ++i) {
		//field.tiles[i].physForm.rotate(2);
	}
}

void Level::addTile(Vector2f pos) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
	field.tiles.push_back(Tile(pos, false, false));
	tileCount++;

	//debug
	std::cout << "\033[2J\033[1;1H";
	std::cout << "Tiles on screen: " << tileCount << '\n';
	//debug
}

void Level::removeTile(Vector2f pos) {
	//Tile tempTile(Vector2f)

	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
	for (int i = 0; i < tileCount; ++i) {
		if (field.tiles[i].physForm.getPosition() == pos) {
			field.tiles.erase(field.tiles.begin() + i);
			tileCount--;

			//debug
			std::cout << "\033[2J\033[1;1H";
			std::cout << "Tiles on screen: " << tileCount << '\n';
			//debug
			break;
		}
	}
}