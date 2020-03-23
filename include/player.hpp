#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "level.hpp"
//#include "playerState.h"
enum TypeInput {Pressed, Released};
using namespace sf;

class PlayerState;

class Player {
private:
	Vector2f pos;
	Vector2f view; // angle must be less than 180 
	float lineOfSight;
	float health;
	//CircleShape physForm;
	RectangleShape physForm;
	Vector2f velocity = Vector2f(0, 0);
    Level* level;

public:
	Player();
	void move(Vector2f dir);
	void setPos(Vector2f pos);
	Vector2f getPos();
	RectangleShape* getPhysForm();
    void setLevel(Level* _level);

    //PlayerState* state;
	//virtual void input(Keyboard::Key _input, TypeInput typeInput);
	//virtual void update();

	friend class PlayerState;
    friend class WalkState;
    friend class OnAirState;
	friend class Game;

};