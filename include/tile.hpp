#pragma once

#include <SFML/Graphics.hpp>
#include "consts.h"

using namespace sf;

enum TileType{
    Standart,
    Dynamic,
    Void,
    Deadly,
    StartPos,
    FinishPos
};

class Tile {
private:
	RectangleShape physForm;
    TileType type = Standart;
	bool isActive = false;
	bool isUnderPlayer = false;
public:
	Tile();
	Tile(Vector2f pos, TileType _type);
	RectangleShape getRect();

	bool checkIfSolid();
	bool checkIfLightAbsorb();
	bool checkIfDrawable();
	bool checkIfDeadly();

friend class Game;
friend class Level;
friend class RayTracing;
friend class Player;
};