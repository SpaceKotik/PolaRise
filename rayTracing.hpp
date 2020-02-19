#pragma once

#include <SFML/Graphics.hpp>
#include "level.hpp"

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
	Vector2f *vertices;
	Line *lines;
	int linesCount;
	Line *rays;
	Vertex **raysVertex;
	Vector2f mousePos;
	//VertexArray *lightMesh;

public:
	RayTracing();
	//void setCanvas(Level level);
	Line getPartIntersection(Line ray, Line line);
	void calculateIntersections();
	void update(Level *level, Window *window, Vector2f mousePos);
	VertexArray createMesh();
	int getLineCount();
	Vertex** getRays();
	void clear();
};