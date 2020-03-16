#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"

using namespace sf;

class Hero {
private:
	Vector2f pos;
	Vector2f view; // angle must be less than 180 
	float lineOfSight;
	float health;
	CircleShape physForm;
	Vector2f velocity = Vector2f(0, 0);

public:
	Hero();
	void move(Vector2f dir);
	void setPos(Vector2f pos);
	Vector2f getPos();
	CircleShape* getPhysForm();
	friend class Game;
};