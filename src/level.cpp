#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include "game.hpp"
#include "tile.hpp"
#include "rayTracing.hpp"
#include "consts.h"

using namespace consts;
using namespace sf;

Level::Level() {
	tileCount = 0;
	for (int i = 0; i < field_x*field_y; ++i) {
		Vector2f pos = Vector2f ((int)(i % field_x)*scale , ((int)i/(int)field_x)*scale) + offset;
		field.tiles.push_back(Tile(pos, Void));
	}
}

void Level::setField() {
	loadFromFile();
}

Field* Level::getField() {
	return &field;
}

void Level::update() {
    for (auto &e : field.tiles) {
        if (e.type == Dynamic) {
            if (e.isActive)
                e.physForm.setFillColor(Color(255, 0, 220));
            else
                e.physForm.setFillColor(Color(100, 0, 80));
                //e.physForm.setFillColor(Color::Black);
        }
        else {
            e.physForm.setFillColor(Color(255, 255, 255));
        }

    }
}

void Level::resetActive() {
    for (auto &e : field.tiles) {
        if (e.type == Dynamic) {
            e.isActive = false;
        }

    }
}

void Level::addTile(Vector2f pos, TileType type) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale,((int)pos.y/(int)scale)*scale);
    field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).type = type;

	loadToFile();
}

void Level::removeTile(Vector2f pos) {

	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
    field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).type = Void;

	loadToFile();
}

int Level::loadToFile() {
	std::ofstream levelFile("../Levels/Level_01.txt");
    if(!levelFile.is_open())
        return -1;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
            switch (field.tiles.at(i*field_x + j).type) {
                case Standart:
                    levelFile << 1 << ' ';
                    break;
                case Dynamic:
                    levelFile << 2 << ' ';
                    break;
                case StartPos:
                    levelFile << 3 << ' ';
                    break;
                case FinishPos:
                    levelFile << 4 << ' ';
                    break;
                case Void:
                    levelFile << 0 << ' ';
                    break;
                default:
                    break;
            }
    	}
    	levelFile << std::endl;
    }
    levelFile.close();
    return 0;
}

int Level::loadFromFile() {
	std::ifstream levelFile("../Levels//Level_01.txt");
    if(!levelFile.is_open())
        return -1;

    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		int currTile;
    		levelFile >> currTile;
    		switch(currTile) {
    		case 0:
    			field.tiles[i*field_x + j].type = Void;
    			break;

    		case 1:
    			field.tiles[i*field_x + j].type = Standart;
    			break;
    		case 2:
                field.tiles[i*field_x + j].type = Dynamic;
    			break;
    		case 3:
    			field.tiles[i*field_x + j].type = StartPos;
    			field.tiles[i*field_x + j].physForm.setFillColor(Color::Green);
    			field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
    			break;
    		case 4:
    			field.tiles[i*field_x + j].type = FinishPos;
    			field.tiles[i*field_x + j].physForm.setFillColor(Color::Red);
    			field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
    		
    		default:
	    		break;
	    	}
    	}
    }

    levelFile.close();
    return 0;
}


bool Level::isOnTile(Vector2f pos) {
	Tile currTile = field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale);
    return currTile.checkIfSolid();
}

bool Level::isOnFinish(Vector2f pos) {
	Tile currTile = field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale);
	if (currTile.type == FinishPos)
		return true;
	else
		return false;
}

// TODO: Maybe do something with indexes of tiles to make it look cleaner
void Level::setActiveTile(Vector2f pos) {
    Tile currTile = field.tiles.at(((int) pos.y / (int) scale) * field_x + (int) pos.x / (int) scale);
    if (currTile.type == Dynamic) {
        field.tiles.at(((int) pos.y / (int) scale) * field_x + (int) pos.x / (int) scale).isActive = true;
    }

}

void Level::setDynamicTiles(std::array<Vertex, 2> line) {
    ///representing mesh edge sa Line
    Line currLine;
    currLine.startCoord = line[0].position;
    currLine.dir = line[1].position - line[0].position;

    ///make direction of line
    float maxLen = sqrt(currLine.dir.x*currLine.dir.x + currLine.dir.y*currLine.dir.y); ///length of the line
    float currLen = 0;  ///current distance from starting end of the line
    eVector2f dir = currLine.dir;
    dir = dir.norm();

    ///check the ends of the line first
    eVector2f currPos = line[1].position;
    setActiveTile(currPos);
    currPos = line[0].position;
    setActiveTile(currPos);

    // TODO: the length of the step must be varied depending on direction
    ///then step by 1 tile witdh every iteration
    while (currLen <= maxLen) {
        if(dir.x == 0 && dir.y == 0)
            break;
        setActiveTile(currPos);
        currPos = currPos + dir*scale;
        currLen = sqrt ((currLine.startCoord.x - currPos.x)*(currLine.startCoord.x - currPos.x) + (currLine.startCoord.y - currPos.y)*(currLine.startCoord.y - currPos.y));
    }
}
