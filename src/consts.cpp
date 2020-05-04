#include <SFML/Graphics.hpp>
#include "consts.h"

namespace consts {
    using namespace sf;
    constexpr int scale = 30;

    const Vector2i fieldSize = {40, 30};
    const Vector2i windowSize = {fieldSize.x*scale, fieldSize.y*scale};

    constexpr float playerHeight = 40;
    constexpr float playerWidth = 20;

    constexpr float playerAcceleration = 1.0;
    constexpr float maxVelocity = 5.5;
    constexpr float viewAngle = 70;

    constexpr float jumpGravity = 0.1;
    constexpr float fallGravity = 0.8;
    constexpr float speedDecreaseCoef = 0.80; //0.83
    constexpr float shootableDuration = 2.5;

    const eVector2f offset = eVector2f(0.005, 0.005);
    const float playerOffset = 0.01;

    const Color tileDefaultColor = Color(200, 200, 200);
    const Color playerColor = Color(255, 110, 110);
    const Color backgroundColor = Color (20, 20, 20);
    const Color lightDefaultColor = Color(40, 100, 100);		///
    const Color inactiveTileColor = Color(100, 0, 80);		///Color for inactivate tiles
    const Color activeTileColor = Color(100, 250, 250);		    ///Color for activate tiles
}


