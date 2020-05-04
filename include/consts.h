#pragma once

#include <SFML/Graphics.hpp>
#include "../SmartVector2/SmartVector2f.h"

#define DEBUG false
#define DOPIXEL false
#define DOBLUR true
#define DOSHADOW true

namespace consts {
    using namespace sf;
    extern const int scale;
    extern const Vector2i fieldSize;
    extern const Vector2i windowSize;

    extern const float playerHeight;
    extern const float playerWidth;
    extern const float playerAcceleration;
    extern const float maxVelocity;
    extern const float viewAngle;
    extern const float jumpGravity;
    extern const float fallGravity;
    extern const float speedDecreaseCoef;
    extern const float shootableDuration;

    extern const eVector2f offset;
    extern const float playerOffset;

    extern const Color lightDefaultColor;
    extern const Color backgroundColor;
    extern const Color inactiveTileColor;
    extern const Color activeTileColor;
    extern const Color tileDefaultColor;
    extern const Color playerColor;
}