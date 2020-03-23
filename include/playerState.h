#pragma once

#include "player.hpp"



class PlayerState {
public:
    virtual ~PlayerState() {};
    virtual PlayerState* input(Player& player, Keyboard::Key input, TypeInput typeInput) = 0;
    virtual PlayerState* update(Player& player) = 0;

};

class StandState : public PlayerState {
public:
    StandState();
    PlayerState* input(Player& player, Keyboard::Key input, TypeInput typeInput) override;
    PlayerState* update(Player& player) override;
};

class OnAirState : public PlayerState {
private:
    bool spacePressed = false;
public:
    OnAirState();
    PlayerState* input(Player& player, Keyboard::Key input, TypeInput typeInput) override;
    PlayerState* update(Player& player) override;
};


enum Direction {Right, Left, None};

class WalkState : public PlayerState {
private:
    Direction direction;
public:
    WalkState(Direction _direction);
    PlayerState* input(Player& player, Keyboard::Key input, TypeInput typeInput) override;
    PlayerState* update(Player& player) override;
};