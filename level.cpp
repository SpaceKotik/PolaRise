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
	tileCount = 0;
	for (int i = 0; i < field_x*field_y; ++i) {
		Vector2f pos = Vector2f ((int)(i % field_x)*scale , ((int)i/(int)field_x)*scale);
		field.tiles.push_back(Tile(pos, false, false));
	}
}

void Level::setField() {
	
	loadFromFile();
	//field.tiles[41].isBlue = true;
	//tileCount++;
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
	/*for (int i = 0; i < field_x*field_y; ++i) {
		if (field.tiles[i].isBlue)
			field.tiles[i].physForm.rotate(3);
	}*/
}

void Level::addTile(Vector2f pos) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale,((int)pos.y/(int)scale)*scale);
	//field.tiles.push_back(Tile(pos, false, false));
	if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue != true) {
	field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue = true;
	tileCount++;
	}

	//debug
	std::cout << "\033[2J\033[1;1H";
	std::cout << "Tiles on screen: " << tileCount << '\n';
	//debug
	
	loadToFile();
}

void Level::removeTile(Vector2f pos) {

	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
	if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue != false) {
		field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue = false;
		tileCount--;
	}

	//debug
	std::cout << "\033[2J\033[1;1H";
	std::cout << "Tiles on screen: " << tileCount << '\n';
	//debug

	loadToFile();
}

int Level::loadToFile() {
	std::ofstream levelFile("Levels//Level_01.txt");
    if(!levelFile.is_open())
        return -1;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		if (field.tiles.at(i*field_x + j).isBlue) {
    			levelFile << 1 << ' ';
    		}
    		else {
    			levelFile << 0 << ' ';
    		}
    	}
    	levelFile << std::endl;
    }
    levelFile.close();
    return 0;
}

int Level::loadFromFile() {
	std::ifstream levelFile("Levels//Level_01.txt");
    if(!levelFile.is_open())
        return -1;

    tileCount = 0;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		int currTile;
    		levelFile >> currTile;
    		if (currTile == 1) {
    			field.tiles[i*field_x + j].isBlue = true;
    			tileCount++;
    		}
    		else {
    			field.tiles[i*field_x + j].isBlue = false;
    		}
    	}
    }
    levelFile.close();
    return 0;
}