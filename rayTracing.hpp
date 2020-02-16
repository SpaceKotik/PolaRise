#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;



struct Line {
	Vector2f startCoord;
	float dirX;
	float dirY;
	float param;
	// line is (x,y)  (x + dirX*param, y + dirY*param)
};

class RayTracing {
private:
	Vector2f vertices;
	Line *lines;
	Line *rays;
public:
	RayTracing();
	//void setCanvas(Level level);
	Vector2f getPartIntersection(Line ray, Line line);
	Vector2f getIntersection(Line ray);
};