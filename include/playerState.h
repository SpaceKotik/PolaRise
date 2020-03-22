#pragma once

#include "hero.hpp"

enum Input {UP, DOWN, LEFT, RIGHT, SPACE};


class PlayerState {
public:
    //virtual ~PlayerState();
    virtual void input(Hero& player, Input input) = 0;
    virtual void update(Hero& player) = 0;
};

class StandState : public PlayerState {
public:
    StandState();
    void input(Hero& player, Input input) override;
    void update(Hero& player) override;
};

class OnAirState : public PlayerState {
public:
    OnAirState();
    void input(Hero& player, Input input) override;
    void update(Hero& player) override;
};