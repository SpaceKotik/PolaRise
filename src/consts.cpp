#include <SFML/Graphics.hpp>
#include "consts.h"


namespace consts {
    using namespace sf;
    constexpr int scale = 30;

    const Vector2i fieldSize = {40, 30};
    const Vector2i windowSize = {fieldSize.x*scale, fieldSize.y*scale};

    constexpr float heroHeight = 40;
    constexpr float heroWidth = 20;

    constexpr float heroAcceleration = 0.5;
    constexpr float maxVelocity = 5.5;
    constexpr float viewAngle = 70;

    constexpr float jumpGravity = 0.1;
    constexpr float fallGravity = 0.8;

    const eVector2f offset = eVector2f(0.005, 0.005);
    const float playerOffset = 0.01;
    const Color tileDefaultColor = Color(200, 200, 200);
    //const Color tileDefaultColor = Color(200, 100, 200);
    //const Color heroColor = Color::Red;
    const Color heroColor = Color(255, 110, 110);
    const Color backgroundColor = Color (20, 20, 20);
    const Color lightColorDefault = Color(255, 51, 51);		///
    const Color inactiveTileColor = Color(100, 0, 80);		///Color for inactivate tiles
    //const Color activeTileColor = Color(250, 168, 8);		    ///Color for activate tiles
    //const Color activeTileColor = Color(80, 200, 200);		    ///Color for activate tiles
    const Color activeTileColor = Color(100, 250, 250);		    ///Color for activate tiles
    //const Color activeTileColor = Color(250, 250, 0);		    ///Color for activate tiles
}


