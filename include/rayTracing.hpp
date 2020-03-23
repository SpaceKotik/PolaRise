#pragma once

#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <unistd.h>

using namespace sf;

///struct used for solving linear equations while intersections are calculated
struct Line {		// line is (x,y)  (x + dirX*param, y + dirY*param)
	Vector2f startCoord;
	float dirX;
	float dirY;
	float param;
};

///algorithm-specific structure for uniting edges
struct Cell {
	int edgeId[4];
	bool edgeExist[4];
	bool exist = false;
};

class RayTracing {
private:
	std::vector<Line> edges;                        ///edges of level
	std::vector<Vector2f> vertices;                 ///vertices of edges
	std::vector<std::array<Vertex, 2>> raysVertex;  ///array that is being converted to light mesh
	bool isRestricted = false;                      ///if false, line of sight is 2*PI
    Line getPartIntersection(Line ray, Line line);  ///functions used in update()
    void calculateIntersections();                  ///
    void convertTileMapToPolyMap(Level *level);     ///both used in updateObstacles()
    void convertPolyMapToVertices();                ///
public:
	RayTracing();
	void update(Vector2f mousePos, bool _isRestricted = true, Vector2f view = Vector2f(-1, 1), float viewAngle = 170);
	VertexArray createMesh(Color);                  ///get drawable polygon if light
	void updateObstacles(Level *level);             ///set new obstacles if level has changed

	void setActiveTiles(Level *level);
};