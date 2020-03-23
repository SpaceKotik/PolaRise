#include <SFML/Graphics.hpp>
#include "consts.h"
namespace consts {
    using namespace sf;
    constexpr int scale = 30;
    constexpr int field_x = 40;
    constexpr int field_y = 30;
    constexpr int window_x = field_x*scale;
    constexpr int window_y = field_y*scale;
    constexpr float heroRadius = 10;
    constexpr float heroHeight = 40;
    constexpr float heroWidth = 20;

    constexpr float heroAcceleration = 0.5;
    constexpr float maxVelocity = 5.5;
    constexpr float viewAngle = 70;

    constexpr float jumpGravity = 0.1;
    constexpr float fallGravity = 0.8;

    //extern const Color tileDefaultColor = Color(100, 100, 100);
    const Color tileDefaultColor = Color::White;
    const Color heroColor = Color(150, 150, 150);

    //extern const Color backgroundColor = Color (50, 50, 50);
    const Color backgroundColor = Color (0, 0, 0);

    //const Color lightColorBlue = Color(102, 102, 255);	//Blue for 1 source
    //const Color lightColorBlue = Color(250, 236, 130);	//Blue for 1 source
    const Color lightColorRed = Color(255, 51, 51);		//Red for 1 source
}


