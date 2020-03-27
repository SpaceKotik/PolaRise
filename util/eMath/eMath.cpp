#include <cmath>
#include <SFML/Window.hpp>

bool areEqual(float a, float b) {
    return std::fabs(a-b) <= 0.1;
}

int index(sf::Vector2f pos, sf::Vector2i fieldSize, int scale) {
    ///Basic index logic, first term is y position of the tile pos is lying on, second is x position
    ///First term is multiplied by field_x size as field is 1-dimentional array
    return ((int)pos.y/(int)scale)*fieldSize.x + (int)pos.x/(int)scale;
}