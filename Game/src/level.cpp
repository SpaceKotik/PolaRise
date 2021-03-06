#include "Game/include/level.hpp"

#include "Game/include/game.hpp"
#include <cmath>
#include "LightSystem/include/rayTracing.hpp"

using namespace consts;
using namespace sf;

Level::Level() = default;

void Level::setField() {
    for (int i = 0; i < fieldSize.x*fieldSize.y; ++i) {
        Vector2f pos = Vector2f ((int)(i % fieldSize.x)*scale , ((int)i/(int)fieldSize.x)*scale) + offset;
        level.emplace_back(pos, Void);
    }
    loadFromFile();
}

std::vector<Tile>* Level::getField() {
	return &level;
}

void Level::update() {
    for (auto &e : level) {
        if (e.type == Dynamic) {
            if (e.isActive && !e.underPlayer)
                e.physForm.setFillColor(activeTileColor);
            else {
                e.physForm.setFillColor(DEBUG ? inactiveTileColor : Color::Transparent);
            }
        }
        else if (e.type == Standart) {
            e.physForm.setFillColor(tileDefaultColor);
        }
        else if (e.type == Void)
            e.physForm.setFillColor(Color::Transparent);
    }
}

void Level::resetActive() {
    for (auto &e : level) {
        if (e.type == Dynamic) {
            e.isActive = false;
        }
    }
}

void Level::addTile(Vector2f pos, TileType type) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale,((int)pos.y/(int)scale)*scale);
    level.at(((int)pos.y / (int)scale) * fieldSize.x + (int)pos.x / (int)scale).type = type;
	loadToFile();
}

void Level::removeTile(Vector2f pos) {
	pos = Vector2f (((int)pos.x/(int)scale)*scale + 0*scale,((int)pos.y/(int)scale)*scale + 0*scale);
    level.at(((int)pos.y / (int)scale) * fieldSize.x + (int)pos.x / (int)scale).type = Void;
	loadToFile();
}

int Level::loadToFile() {
	std::ofstream levelFile(mediator->getLevelPath() + "level.txt");
    if(!levelFile.is_open())
        return -1;
    for (int i = 0; i < fieldSize.y; ++i) {
    	for (int j = 0; j < fieldSize.x; ++j) {
            switch (level.at(i * fieldSize.x + j).type) {
                case Standart:
                    levelFile << 1 << ' ';
                    break;
                case Dynamic:
                    levelFile << 2 << ' ';
                    break;
                case Deadly:
                    levelFile << 3 << ' ';
                    break;
                case Start:
                    levelFile << 4 << ' ';
                    break;
                case Finish:
                    levelFile << 5 << ' ';
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

    levelFile << startPos << std::endl;
    levelFile << finishPos << std::endl;

    levelFile.close();
    return 0;
}

int Level::loadFromFile() {
	std::ifstream levelFile(mediator->getLevelPath() + "level.txt");
    if(!levelFile.is_open())
        return -1;

    for (int i = 0; i < fieldSize.y; ++i) {
    	for (int j = 0; j < fieldSize.x; ++j) {
    		int currTile;
    		levelFile >> currTile;
    		switch(currTile) {
    		case 0:
                level[i * fieldSize.x + j].type = Void;
    			break;

    		case 1:
                level[i * fieldSize.x + j].type = Standart;
                level[i * fieldSize.x + j].physForm.setFillColor(tileDefaultColor);
    			break;
    		case 2:
                level[i * fieldSize.x + j].type = Dynamic;
    			break;
            case 3:
                level[i * fieldSize.x + j].type = Deadly;
                level[i * fieldSize.x + j].physForm.setFillColor(sf::Color::Red);
                break;
    		case 4:
                level[i * fieldSize.x + j].type = Start;
                level[i * fieldSize.x + j].physForm.setFillColor(Color::Green);
                level[i * fieldSize.x + j].physForm.setOutlineColor(Color::Black);
                level[i * fieldSize.x + j].physForm.setOutlineThickness(1);
    			break;
    		case 5:
                level[i * fieldSize.x + j].type = Finish;
                level[i * fieldSize.x + j].physForm.setFillColor(Color(100, 255, 100));
                level[i * fieldSize.x + j].physForm.setOutlineColor(Color::Black);
                level[i * fieldSize.x + j].physForm.setOutlineThickness(1);
    		
    		default:
	    		break;
	    	}
    	}
    }

    levelFile >> startPos.x;
    levelFile >> startPos.y;
    levelFile >> finishPos.x;
    levelFile >> finishPos.y;

    level.at(index(finishPos, fieldSize, scale)).type = Finish;

    levelFile.close();
    return 0;
}

bool Level::isOnTile(Vector2f pos) {
    int i = index(pos, fieldSize, scale);
    if(level.size() <= i || i < 0)
        return false;
	Tile currTile = level.at(i);
    return (currTile.isSolid() && !currTile.underPlayer);
}

bool Level::isOnDeadly(Vector2f pos) {
    int i = index(pos, fieldSize, scale);
    if(level.size() <= i || i < 0)
        return false;
    Tile currTile = level.at(i);
    return (currTile.type == Deadly);
}

bool Level::isOnFinish(Vector2f pos) {
    return level.at((index(pos, {fieldSize.x, fieldSize.y}, scale))).isFinish();
}

bool Level::isUnderPlayer(Vector2f pos) {
    return level.at((index(pos, {fieldSize.x, fieldSize.y}, scale))).isUnderPlayer();
}

// TODO: Maybe do something with indexes of tiles to make it look cleaner
void Level::setActiveTile(Vector2f pos) {
    ///check if index is valid
    int i = index(pos, fieldSize, scale);
    if(level.size() <= i || i < 0)
        return;
    Tile currTile = level.at(i);
    if (currTile.type == Dynamic) {
        level.at(i).isActive = true;
    }

}

void Level::setDynamicTiles(std::array<Vertex, 2> line) {
    ///representing mesh edge sa Line
    Line currLine;
    currLine.startCoord = line[0].position;
    currLine.dir = line[1].position - line[0].position;

    ///make direction of line
    float maxLen = std::sqrt(currLine.dir.x*currLine.dir.x + currLine.dir.y*currLine.dir.y); ///length of the line // TODO : test
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

void Level::setMediator(Game* _game) {
    mediator = _game;
}

eVector2f Level::getStartPos() {
    return startPos;
}



