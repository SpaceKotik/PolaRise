#include "game.hpp"
#include "tile.hpp"
#include "level.hpp"
#include "hero.hpp"
#include <iostream>
#include <stdlib.h>

extern const int scale;
extern const int field_x;
extern const int field_y;
extern const float heroRadius;
extern const Color heroColor;
extern const float viewAngle;


Hero::Hero() {
	physForm.setRadius(heroRadius);
	physForm.setOrigin(Vector2f(heroRadius, heroRadius));
	physForm.setPosition(Vector2f(100, 100));
	//physForm.setFillColor(Color(102, 0, 204));
	physForm.setFillColor(heroColor);
	physForm.setOutlineColor(Color(20, 20, 20));
	physForm.setOutlineThickness(0);
	lineOfSight = viewAngle;
}
void Hero::move(Vector2f dir) {
	physForm.move(dir);
}

void Hero::setPos(Vector2f pos) {
	physForm.setPosition(pos);
}

Vector2f Hero::getPos() {
	return physForm.getPosition();
}
CircleShape* Hero::getPhysForm() {
	return &physForm;
}