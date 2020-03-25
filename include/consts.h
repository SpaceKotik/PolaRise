#include <SFML/Graphics.hpp>
#include "../SmartVector2/SmartVector2f.h"
#pragma once

namespace consts {
    using namespace sf;
    extern const int scale;
    extern const int field_x;
    extern const int field_y;
    extern const int window_x;
    extern const int window_y;
    extern const float heroRadius;
    extern const float heroHeight;
    extern const float heroWidth;
    extern const float heroAcceleration;
    extern const float maxVelocity;
    extern const float viewAngle;
    extern const eVector2f offset;

    extern const float jumpGravity;
    extern const float fallGravity;

    //extern const Color tileDefaultColor;
    extern const Color tileDefaultColor;
    extern const Color heroColor;

    //extern const Color backgroundColor;
    extern const Color backgroundColor;

    extern const Color lightColorBlue;	//Blue for 1 source
    extern const Color lightColorRed;    //Red for 1 source
}
