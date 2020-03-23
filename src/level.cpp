#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cmath>
#include "game.hpp"
#include "tile.hpp"
#include <iostream>
#include <stdlib.h>
#include "rayTracing.hpp"

#include "consts.h"
using namespace consts;

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
}

Field* Level::getField() {
	return &field;
}

void Level::update() {
    for (auto &e : field.tiles) {
        if (e.isActive)
            e.physForm.setFillColor(Color::Green);
        else
            e.physForm.setFillColor(Color::Red);

    }
}

void Level::resetActive() {
    for (auto &e : field.tiles) {
        e.isActive = false;

    }
}

void Level::addTile(Vector2f pos) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale,((int)pos.y/(int)scale)*scale);
	if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue != true && levelState == Blue) {
	field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue = true;
	field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isSolidBlue = true;
	tileCount++;
	}
	else if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isRed != true && levelState == Red) {
	field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isRed = true;
	field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isSolidRed = true;
	tileCount++;
	}
	loadToFile();
}

void Level::removeTile(Vector2f pos) {

	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
	if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue != false && levelState == Blue) {
		field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isBlue = false;
		field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isSolidBlue = false;
		tileCount--;
	}
	else if (field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isRed != false && levelState == Red) {
		field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isRed = false;
		field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale).isSolidRed = false;
		tileCount--;
	}
	loadToFile();
}

int Level::loadToFile() {
	std::ofstream levelFile("../Levels/Level_01.txt");
    if(!levelFile.is_open())
        return -1;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		if (field.tiles.at(i*field_x + j).isBlue) {
    			switch (field.tiles.at(i*field_x + j).typeBlue) {
    				case Standart:
    					levelFile << 1 << ' ';
    					break;
    				case Lava:
    					levelFile << 2 << ' ';
    					break;
    				case StartPos:
    					levelFile << 3 << ' ';
    					break;
    				case FinishPos:
    					levelFile << 4 << ' ';
    					break;
					default:
						break;
    			}
    		}
    		else {
    			levelFile << 0 << ' ';
    		}
    	}
    	levelFile << std::endl;
    }
    levelFile << std::endl;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		if (field.tiles.at(i*field_x + j).isRed) {
    			switch (field.tiles.at(i*field_x + j).typeRed) {
    				case Standart:
    					levelFile << 1 << ' ';
    					break;
    				case Lava:
    					levelFile << 2 << ' ';
    					break;
    				case StartPos:
    					levelFile << 3 << ' ';
    					break;
    				case FinishPos:
    					levelFile << 4 << ' ';
    					break;
					default:
						break;
    			}
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
	std::ifstream levelFile("../Levels//Level_01.txt");
    if(!levelFile.is_open())
        return -1;

    tileCount = 0;
    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		int currTile;
    		levelFile >> currTile;
    		switch(currTile) {
    		case 0:
    			field.tiles[i*field_x + j].isBlue = false;
    			field.tiles[i*field_x + j].isSolidBlue = false;
    			break;

    		case 1:
    			field.tiles[i*field_x + j].isBlue = true;
    			field.tiles[i*field_x + j].isSolidBlue = true;
    			field.tiles[i*field_x + j].typeBlue = Standart;
    			tileCount++;
    			break;
    		case 2:
    			break;
    		case 3:
    			field.tiles[i*field_x + j].isBlue = true;
    			field.tiles[i*field_x + j].isSolidBlue = false;
    			field.tiles[i*field_x + j].typeBlue = StartPos;
    			field.tiles[i*field_x + j].physForm.setFillColor(Color::Green);
    			field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
    			//tileCount++;
    			break;
    		case 4:
	    		field.tiles[i*field_x + j].isBlue = true;
    			field.tiles[i*field_x + j].isSolidBlue = false;
    			field.tiles[i*field_x + j].typeBlue = FinishPos;
    			field.tiles[i*field_x + j].physForm.setFillColor(Color::Red);
    			field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
    		
    		default:
	    		break;
	    	}
    	}
    }

    for (int i = 0; i < field_y; ++i) {
    	for (int j = 0; j < field_x; ++j) {
    		int currTile;
    		levelFile >> currTile;
    		switch(currTile) {
    		case 0:
    			field.tiles[i*field_x + j].isRed = false;
    			field.tiles[i*field_x + j].isSolidRed = false;
    			break;

    		case 1:
    			field.tiles[i*field_x + j].isRed = true;
    			field.tiles[i*field_x + j].isSolidRed = true;
    			field.tiles[i*field_x + j].typeRed = Standart;
    			tileCount++;
    			break;
    		case 2:
    			break;
    		case 3:
    			field.tiles[i*field_x + j].isRed = true;
    			field.tiles[i*field_x + j].isSolidRed = false;
    			field.tiles[i*field_x + j].typeRed = StartPos;
    			field.tiles[i*field_x + j].physForm.setFillColor(Color::Green);
    			field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
    			//tileCount++;
    			break;
    		case 4:
	    		field.tiles[i*field_x + j].isRed = true;
	    		field.tiles[i*field_x + j].isSolidRed = false;
	    		field.tiles[i*field_x + j].typeRed = FinishPos;
	    		field.tiles[i*field_x + j].physForm.setFillColor(Color::Red);
	    		field.tiles[i*field_x + j].physForm.setOutlineColor(Color::Black);
				field.tiles[i*field_x + j].physForm.setOutlineThickness(1);
	    		break;
	    	default:
	    		break;
	    	}
    	}
    }



    levelFile.close();
    return 0;
}

void Level::changeState() {
	if (levelState == Blue)
		levelState = Red;
	else
		levelState = Blue;
}

LevelState Level::getState() {
	return levelState;
}

bool Level::isOnTile(Vector2f pos) {
	Tile currTile = field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale);
	if (currTile.isBlue && levelState == Blue && currTile.isSolidBlue) {
		return true;
	}
	else if (currTile.isRed  && levelState == Red && currTile.isSolidRed) {
		return true;
	}
	else {
		return false;
	}


	/////
	if (currTile.isSolid)
        return true;
	else
        return false;
	/////
}

bool Level::isOnFinish(Vector2f pos) {
	Tile currTile = field.tiles.at(((int)pos.y/(int)scale)*field_x + (int)pos.x/(int)scale);
	if (currTile.isBlue && levelState == Blue && currTile.typeBlue == FinishPos) {
		return true;
	}
	else if (currTile.isRed  && levelState == Red && currTile.typeRed == FinishPos) {
		return true;
	}
	else {
		return false;
	}
}



void Level::setActiveTile(Vector2f pos) {
    Tile currTile = field.tiles.at(((int) pos.y / (int) scale) * field_x + (int) pos.x / (int) scale);
    if (currTile.isSolid && currTile.isDynamic)
        field.tiles.at(((int) pos.y / (int) scale) * field_x + (int) pos.x / (int) scale).isActive = true;

}

void Level::setDynamicTiles(std::array<Vertex, 2> line) {
    struct Line currLine;

    currLine.startCoord = line[0].position;
    currLine.dirX = line[1].position.x - line[0].position.x;
    currLine.dirY = line[1].position.y - line[0].position.y;


    eVector2f currPos = line[1].position;
    setActiveTile(currPos);
    currPos = line[0].position;
    setActiveTile(currPos);

    ///make direction of line
    float maxLen = sqrt(currLine.dirX*currLine.dirX + currLine.dirY*currLine.dirY);
    //float currLen = sqrt ((currLine.startCoord.x - currPos.x)*(currLine.startCoord.x - currPos.x) + (currLine.startCoord.y - currPos.y)*(currLine.startCoord.y - currPos.y));
    float currLen = 0;////////////
    eVector2f dir = {currLine.dirX, currLine.dirY};
    dir = dir.norm();


    while (currLen <= maxLen) {
        if(dir.x == 0 && dir.y == 0)
            break;
        setActiveTile(currPos);
        currPos = currPos + dir*scale;
        currLen = sqrt ((currLine.startCoord.x - currPos.x)*(currLine.startCoord.x - currPos.x) + (currLine.startCoord.y - currPos.y)*(currLine.startCoord.y - currPos.y));

    }


}







