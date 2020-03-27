#include <cmath>

bool areEqual(float a, float b) {
    return std::fabs(a-b) <= 0.1;
}