#pragma once

#include "player.hpp"


//enum Input {UP, DOWN, LEFT, RIGHT, SPACE};


class StandState;
class OnAirState;


class PlayerState {
public:
    static StandState standing;
    static OnAirState onAir;

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