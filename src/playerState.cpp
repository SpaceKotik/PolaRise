#include "playerState.h"
#include "player.hpp"
#include <iostream>

class StandState;
class OnAirState;

StandState::StandState() {

}

PlayerState* StandState::input(Player &player, Keyboard::Key input, TypeInput typeInput) {
    if(input == Keyboard::Key::Space && typeInput == Pressed) {
        return new OnAirState();
    }
    else
        return nullptr;
}

PlayerState* StandState::update(Player &player) {
    return nullptr;
}

OnAirState::OnAirState() {

}

PlayerState* OnAirState::input(Player &player, Keyboard::Key input, TypeInput typeInput) {
    return nullptr;
    if(input == Keyboard::Key::Space && typeInput == Pressed) {
        return new OnAirState();
    }
    else
        return nullptr;
}

PlayerState* OnAirState::update(Player &player) {
    return nullptr;
}



