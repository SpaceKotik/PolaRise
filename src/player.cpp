#include "game.hpp"
#include "tile.hpp"
#include "level.hpp"
#include "player.hpp"
#include <iostream>
#include <stdlib.h>
#include "consts.h"
#include "playerState.h"
#include "level.hpp"
using namespace consts;

Player::Player() {
	//physForm.setRadius(heroRadius);
	physForm.setSize({heroWidth, heroHeight});
	physForm.setOrigin(Vector2f(heroWidth/2, heroHeight/2));
	physForm.setPosition(Vector2f(100, 100));
	//physForm.setFillColor(Color(102, 0, 204));
	physForm.setFillColor(heroColor);
	physForm.setOutlineColor(Color(20, 20, 20));
	physForm.setOutlineThickness(0);
	lineOfSight = viewAngle;
	//state = new WalkState(None);
}

void Player::setLevel(Level* _level) {
    level = _level;
}

void Player::move(Vector2f dir) {
	physForm.move(dir);
}

void Player::setPos(Vector2f pos) {
	physForm.setPosition(pos);
}

Vector2f Player::getPos() {
	return physForm.getPosition();
}

RectangleShape* Player::getPhysForm() {
	return &physForm;
}
/*
void Player::input(Keyboard::Key input, TypeInput typeInput) {
    PlayerState* _state = state->input(*this, input, typeInput);
    if (_state != nullptr) {
        delete state;
        state = _state;
    }
}

void Player::update() {
    PlayerState* _state = state->update(*this);
    if (_state != nullptr) {
        delete state;
        state = _state;
    }
}
 */