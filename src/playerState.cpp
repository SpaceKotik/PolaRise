#include "playerState.h"
#include "player.hpp"
#include <iostream>
#include <math.h>
#include "consts.h"

using namespace consts;

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
    ///process down moving if player moves down
    //std::cout << "move" << std::endl;
    /*player.velocity.y += heroAcceleration;
    Vector2f desPos;
    desPos = player.getPos() + player.velocity;

    if (player.level->isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) || player.level->isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1))) {
        std::cout << "walking" << std::endl;
        return new WalkState(None);

    }*/

    return nullptr;
}

PlayerState* OnAirState::update(Player &player) {
    player.velocity.y += heroAcceleration;
    //std::cout << "move" << std::endl;
    Vector2f desPos;
    desPos = player.getPos() + player.velocity;

    if (player.level->isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) || player.level->isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1))) {
        std::cout << "walking" << std::endl;
        return new WalkState(None);

    }
    player.move(player.velocity);
    return nullptr;
}

WalkState::WalkState(Direction _direction) {
    direction = _direction;
}

PlayerState* WalkState::input(Player &player, Keyboard::Key input, TypeInput typeInput) {
    if(typeInput == Released)
        direction = None;
    else if (input == Keyboard::Key::A && typeInput == Pressed) {
        direction = Left;
    }
    else if (input == Keyboard::Key::D && typeInput == Pressed) {
        direction = Right;
    }
    else if(input == Keyboard::Key::Space ) {
        std::cout << "OnAir\n";
        player.velocity.y += 30;
        return new OnAirState();
        //return nullptr;
    }
    return nullptr;
}

PlayerState* WalkState::update(Player &player) {
    player.velocity.y = 0;
    if(direction == Right) {
        player.velocity.x += heroAcceleration;
    }
    if(direction == Left) {
        player.velocity.x -= heroAcceleration;
    }

    Vector2f desPos;
    ///process right moving if player moves right
    desPos = player.getPos() + Vector2f(player.velocity.x , 0);

    ///if no intersections, move onle horizontally
    if (!player.level->isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !player.level->isOnTile(desPos + Vector2f(heroRadius-.1, -heroRadius+.1))) {

        player.move(Vector2f(player.velocity.x, 0));
    }
        ///else move right to the obstacle and mirror speed
    else {
        if (player.velocity.x > 0) {
            player.setPos(Vector2f(((int)player.getPos().x / (int)scale + 1) * scale - heroRadius , player.getPos().y));
            player.velocity.x = -player.velocity.x * 0.2;
        }
    }

    ///process left moving if player moves left
    desPos = player.getPos() + Vector2f(player.velocity.x , 0);
    if (!player.level->isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1)) && !player.level->isOnTile(desPos + Vector2f(-heroRadius+.1, -heroRadius+.1))) {

        player.move(Vector2f(player.velocity.x, 0));
    }
    else {
        if (player.velocity.x < 0) {
            player.setPos(Vector2f(((int)player.getPos().x / (int)scale) * scale + heroRadius , player.getPos().y));
            player.velocity.x = -player.velocity.x * 0.2;
        }
    }


    if(direction == None) {
        player.velocity = Vector2f(player.velocity.x * 0.88, player.velocity.y);
    }

    if (abs(player.velocity.x) > maxVelocity) {
        player.velocity.x /= abs(player.velocity.x);
        player.velocity.x *= maxVelocity;
    }

    if (abs(player.velocity.x) < 0.001) {
        player.velocity = Vector2f(0, 0);
    }


    ///process down moving if player moves down
    desPos = player.getPos() + Vector2f(0, 5);
    if (!player.level->isOnTile(desPos + Vector2f(heroRadius-.1, heroRadius-.1)) && !player.level->isOnTile(desPos + Vector2f(-heroRadius+.1, heroRadius-.1))) {
        std::cout << "OnAir\n";
        return new OnAirState();
        //return nullptr;
    }

    return nullptr;
}



