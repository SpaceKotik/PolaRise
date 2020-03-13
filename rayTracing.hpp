#pragma once

#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <unistd.h>

using namespace sf;


struct Line {		// line is (x,y)  (x + dirX*param, y + dirY*param)
	Vector2f startCoord;
	float dirX;
	float dirY;
	float param;
};

struct Cell {
	int edgeId[4];
	bool edgeExist[4];
	bool exist = false;
};

class RayTracing {
private:
	std::vector<Line> edges;
	std::vector<Vector2f> vertices;
	std::vector<std::array<Vertex, 2>> raysVertex;
	Color lightColor;
	bool isRestricted = 0;

public:
	RayTracing();
	Line getPartIntersection(Line ray, Line line);
	void calculateIntersections();
	void update(Level *level, RenderWindow *window, Vector2f mousePos, bool _isRestricted = 0, Vector2f view = Vector2f(1, 1), float viewAngle = 200);
	VertexArray createMesh();
	VertexArray createVisibleBorders();
	void convertTileMapToPolyMap(Level *level, Window *window);
	void convertPolyMapToVertices();
	void changeLightColor();
	friend class Game;
};