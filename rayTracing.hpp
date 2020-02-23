#pragma once

#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <unistd.h>

using namespace sf;



struct Line {
	Vector2f startCoord;
	float dirX;
	float dirY;
	float param;
	// line is (x,y)  (x + dirX*param, y + dirY*param)
};


struct Cell {
	int edgeId[4];
	bool edgeExist[4];
	bool exist = false;
};

class RayTracing {
private:
	//Vector2f *vertices;
	Line *lines;


	std::vector<Line> edges;
	std::vector<Vector2f> vertices;

	//int linesCount;
	//Line *rays;
	//Vertex **raysVertex;
	std::vector<std::array<Vertex, 2>> raysVertex;
	Vector2f mousePos;
	//VertexArray *lightMesh;
	std::vector<Cell> processingCells; 

public:
	RayTracing();
	//void setCanvas(Level level);
	Line getPartIntersection(Line ray, Line line);
	void calculateIntersections();
	void update(Level *level, Window *window, Vector2f mousePos);
	VertexArray createMesh();
	//int getLineCount();
	Vertex** getRays();
	void convertTileMapToPolyMap(Level *level, Window *window);
	void convertPolyMapToVertices();
	void clear();
	friend class Game;
};