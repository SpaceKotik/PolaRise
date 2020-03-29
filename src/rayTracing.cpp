#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "game.hpp"
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "consts.h"

using namespace consts;

const float angle = 0.03;

#define NO_INTERSECTION Vector2f(-10, -10);

#define DOWN 0
#define LEFT 1
#define UP 2
#define RIGHT 3

Vector2f getRectPointPos(Tile rect, int point) {
    return rect.getRect().getTransform().transformPoint(rect.getRect().getPoint(point));
}

RayTracing::RayTracing() = default;

///convert tiles to edges
void RayTracing::convertTileMapToPolyMap(Level *level) {
    ///reset all information
	std::vector<Cell> processingCells;
	processingCells.clear();
	edges.clear();

	///Set borders
	Vector2f currWindowSize = Vector2f(windowSize.x, windowSize.y);

	Line newBorder;
	newBorder.startCoord = {0, 0};
	newBorder.dir = {currWindowSize.x, 0};

	edges.push_back(newBorder);

	newBorder.startCoord = {currWindowSize.x, 0};
    newBorder.dir = {0, currWindowSize.y};
	edges.push_back(newBorder);

	newBorder.startCoord = {currWindowSize.x, currWindowSize.y};
    newBorder.dir = {-currWindowSize.x , 0};
	edges.push_back(newBorder);

	newBorder.startCoord = {0, currWindowSize.y};
    newBorder.dir = {0, -currWindowSize.y};
	edges.push_back(newBorder);

	///Reset edges information
	for (int i = 0; i < fieldSize.y; ++i) {
		for (int j = 0; j < fieldSize.x; ++j) {
			Cell tempCell;
			for (int k = 0; k < 4; ++k) {
				tempCell.edgeExist[k] = false;
				tempCell.edgeId[k] = -1;
			}

            tempCell.exist = !(level->getField()->tiles[i * fieldSize.x + j].checkIfLightAbsorb());
			processingCells.push_back(tempCell);
		}
	}

	///Do logic ad push edges to array
	for (int i = 0; i < fieldSize.y; ++i) {
		for (int j = 0; j < fieldSize.x; ++j) {

			///indexes for cells
			int self = i*fieldSize.x + j;
			int left = i*fieldSize.x + j - 1;
			int right = i*fieldSize.x + j + 1;
			int up = (i-1)*fieldSize.x + j;
			int down =(i+1)*fieldSize.x + j;

			if (processingCells[self].exist) {

				///check left neighbour
				if (j != 0 && !processingCells[left].exist) {

					///edge existing?
					if ( i != 0 && processingCells[up].edgeExist[LEFT]) {

						edges[processingCells[up].edgeId[LEFT]].dir.y += scale;
						processingCells[self].edgeId[LEFT] = processingCells[up].edgeId[LEFT];
						processingCells[self].edgeExist[LEFT] = true;
					}
					///if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 0);
						newEdge.dir = getRectPointPos(level->getField()->tiles[self], 3) - newEdge.startCoord;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[LEFT] = edge_id;
						processingCells[self].edgeExist[LEFT] = true;
					}
				}

				///check right neighbour
				if (j != fieldSize.x-1 && !processingCells[right].exist) {

					///edge existing?
					if ( i != 0 && processingCells[up].edgeExist[RIGHT]) {

						edges[processingCells[up].edgeId[RIGHT]].dir.y += scale;
						processingCells[self].edgeId[RIGHT] = processingCells[up].edgeId[RIGHT];
						processingCells[self].edgeExist[RIGHT] = true;
					}
					///if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 1);
						newEdge.dir = getRectPointPos(level->getField()->tiles[self], 2) - newEdge.startCoord;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[RIGHT] = edge_id;
						processingCells[self].edgeExist[RIGHT] = true;
					}
				}

				///check up neighbour
				if (i != 0 && !processingCells[up].exist) {


					///edge existing?
					if (j != 0 && processingCells[left].edgeExist[UP]) {

						edges[processingCells[left].edgeId[UP]].dir.x += scale;
						processingCells[self].edgeId[UP] = processingCells[left].edgeId[UP];
						processingCells[self].edgeExist[UP] = true;
					}
					///if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 0);
						newEdge.dir = getRectPointPos(level->getField()->tiles[self], 1) - newEdge.startCoord;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[UP] = edge_id;
						processingCells[self].edgeExist[UP] = true;
					}
				}

				///check down neighbour
				if (j != fieldSize.x-1 && !processingCells[down].exist) {

					///edge existing?
					if (j != 0 && processingCells[left].edgeExist[DOWN]) {

						edges[processingCells[left].edgeId[DOWN]].dir.x += scale;
						processingCells[self].edgeId[DOWN] = processingCells[left].edgeId[DOWN];
						processingCells[self].edgeExist[DOWN] = true;
					}
					///if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 3);//
						newEdge.dir =  getRectPointPos(level->getField()->tiles[self], 2) - newEdge.startCoord;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[DOWN] = edge_id;
						processingCells[self].edgeExist[DOWN] = true;
					}
				}
			} 
		}
	}
}

///add ends of edges to vertex vector
void RayTracing::convertPolyMapToVertices() {
	vertices.clear();
	for (auto e : edges) {
        /// add only unique vertices
        /// first point of edge
		if (std::find(vertices.begin(), vertices.end(), e.startCoord) == vertices.end())
			vertices.push_back(e.startCoord);
		/// second of edge
		Vector2f tempVertex = e.startCoord + e.dir;
		if (std::find(vertices.begin(), vertices.end(), tempVertex) == vertices.end())
			vertices.push_back(tempVertex);
	}
}

///calculate intersections for current position and view
void RayTracing::update(Vector2f pos, bool _isRestricted, Vector2f view, float _viewAngle) {
    /// set if view angle is restricted or 2*PI
	isRestricted = _isRestricted;

	raysVertex.clear();

	///set raysVertex for main rays
	for (auto vertice : vertices) {
		std::array<Vertex, 2> currRay;
		currRay[0] = Vertex(pos, Color::Blue);
		currRay[1] = vertice;
		raysVertex.push_back(currRay);
    }

    ///set raysVertex for assisting rays
    for (auto vertice : vertices) {
    	std::array<Vertex, 2> currRay;
    	currRay[0] = Vertex(pos, Color::Blue);

        ///rotate to left
    	Transform rotation1;
    	rotation1.rotate(-angle, pos).scale(10000, 10000, pos.x, pos.y);  ///quite a weak point
    	currRay[1] = Vertex(vertice, Color::Blue);
    	currRay[1].position = rotation1.transformPoint(currRay[1].position);
    	raysVertex.push_back(currRay);

        ///rotate to right
    	Transform rotation2;
    	rotation2.rotate(angle, pos).scale(10000, 10000, pos.x, pos.y);  ///also
    	currRay[1] = Vertex(vertice, Color::Blue);
    	currRay[1].position = rotation2.transformPoint(currRay[1].position);
    	raysVertex.push_back(currRay);
    }

    ///making vector from player to it's view angle
	std::array<Vertex, 2> viewVec;
	viewVec[0] = pos;
	viewVec[1] = pos + view;

    ///erasing all rays that are not in line of sight (lambda function as bool expression)
	if(isRestricted) {
        raysVertex.erase(std::remove_if(raysVertex.begin(), raysVertex.end(),
                                        [viewVec, _viewAngle](std::array<Vertex, 2> const &a) -> bool {
                                            ///representing current ray and view vector as lines and checking if current ray is in line of sight
                                            Line curLine, viewLine;

                                            curLine.startCoord = a[0].position;
                                            curLine.dir = a[1].position - a[0].position;

                                            viewLine.startCoord = viewVec[0].position;
                                            viewLine.dir = viewVec[1].position - viewVec[0].position;


                                            float curLineLen = sqrt(
                                                    curLine.dir.x * curLine.dir.x + curLine.dir.y * curLine.dir.y);
                                            float viewLineLen = sqrt(
                                                    viewLine.dir.x * viewLine.dir.x + viewLine.dir.y * viewLine.dir.y);
                                            float cosine =
                                                    (curLine.dir.x * viewLine.dir.x + curLine.dir.y * viewLine.dir.y) /
                                                    (curLineLen * viewLineLen);

                                            return cosine < cos(_viewAngle / 2.f / 180.f * M_PI);

                                        }), raysVertex.end());
    }

	///adding restricting rays
	Transform rotationLeft;
	rotationLeft.rotate(-_viewAngle/2, pos).scale(10000, 10000, pos.x, pos.y);
	std::array<Vertex, 2> viewRayLeft;
	viewRayLeft[0] = pos;
	viewRayLeft[1] = rotationLeft.transformPoint(pos + view);
	raysVertex.push_back(viewRayLeft);

	Transform rotationRight;
    rotationRight.rotate(_viewAngle/2, pos).scale(10000, 10000, pos.x, pos.y);
    std::array<Vertex, 2> viewRayRight;
	viewRayRight[0] = pos;
	viewRayRight[1] = rotationRight.transformPoint(pos + view);
	raysVertex.push_back(viewRayRight);


	///calculating intersections before sorting
	calculateIntersections();
	
    ///Sorting array of rays to make light mesh;
    if (isRestricted) {
        std::sort(raysVertex.begin(), raysVertex.end(),
                  [viewRayRight](std::array<Vertex, 2> const &a, std::array<Vertex, 2> const &b) -> bool {

                      Line aLine, bLine, refLine;

                      aLine.startCoord = a[0].position;
                      aLine.dir = a[1].position - a[0].position;

                      bLine.startCoord = b[0].position;
                      bLine.dir = b[1].position - b[0].position;

                      refLine.startCoord = viewRayRight[0].position;
                      refLine.dir = viewRayRight[1].position - viewRayRight[0].position;

                      float aLineLen = sqrt(aLine.dir.x * aLine.dir.x + aLine.dir.y * aLine.dir.y);
                      float bLineLen = sqrt(bLine.dir.x * bLine.dir.x + bLine.dir.y * bLine.dir.y);
                      float refLineLen = sqrt(refLine.dir.x * refLine.dir.x + refLine.dir.y * refLine.dir.y);
                      float cos1 = (aLine.dir.x * refLine.dir.x + aLine.dir.y * refLine.dir.y) / (aLineLen * refLineLen);
                      float cos2 = (bLine.dir.x * refLine.dir.x + bLine.dir.y * refLine.dir.y) / (bLineLen * refLineLen);

                      if (cos1 - cos2 < 0) {
                          return true;
                      } else {
                          return false;
                      }
                  });
    }
    else {
        std::sort(raysVertex.begin(), raysVertex.end(), [](std::array<Vertex, 2> const &a, std::array<Vertex, 2> const &b) -> bool {
            Line aLine, bLine;
            aLine.startCoord = a[0].position;
            aLine.dir = a[1].position - a[0].position;

            bLine.startCoord = b[0].position;
            bLine.dir = b[1].position - b[0].position;

            return (atan2(aLine.dir.y, aLine.dir.x) - atan2(bLine.dir.y, bLine.dir.x)) < 0;
        });
    }
// TODO: make line extention constant
    ///may be the way to show walls better
	for (auto &e : raysVertex) {
        Transform transform;
        transform.scale(1.01, 1.01, pos.x, pos.y);
        e[1].position = transform.transformPoint(e[1].position);
	}
}


bool isNear(float a, float b) {
    return (a - b) < 0.1 && (a - b) > -0.1;
}

Line RayTracing::getPartIntersection(Line ray, Line line) {
    // FIXME: this part may cause glitches
    // FIXME: added offset for all level tiles as fast solution
/*
    // Are they parallel? If so, no intersect
    float rayLen = sqrt(ray.dir.x * ray.dir.x + ray.dir.y * ray.dir.y);
    float lineLen = sqrt(line.dir.x * line.dir.x + line.dir.y * line.dir.y);

    float distance1 = sqrt ((line.startCoord.x-ray.startCoord.x)*(line.startCoord.x-ray.startCoord.x) + (line.startCoord.y-ray.startCoord.y)*(line.startCoord.y-ray.startCoord.y));
    float distance2 = sqrt ((line.startCoord.x + line.dir.x-ray.startCoord.x)*(line.startCoord.x + line.dir.x-ray.startCoord.x)
     + (line.startCoord.y + line.dir.y-ray.startCoord.y)*(line.startCoord.y + line.dir.y-ray.startCoord.y));

    if(abs(ray.dir.x / rayLen) == abs(line.dir.x / lineLen) && abs(ray.dir.y / rayLen) == abs(line.dir.y / lineLen) && (ray.startCoord.x == line.startCoord.x || ray.startCoord.y == line.startCoord.y)){ /// Directions are the same.
    	if (distance1 > distance2) {
    		Line tempLine;
		    tempLine.startCoord = Vector2f(line.startCoord.x + line.dir.x, line.startCoord.y + line.dir.y);
		    tempLine.param = distance2/rayLen;
		    return tempLine;
    	}
    	else {
    		Line tempLine;
		    tempLine.startCoord = Vector2f(line.startCoord.x, line.startCoord.y);
            tempLine.param = distance1/rayLen;
		    return tempLine;
    	}
	}
*/
///Seems useless
/*
    eVector2f point1 = line.startCoord;
    eVector2f point2 = line.startCoord + line.dir;
    float rayLen = sqrt(ray.dir.x * ray.dir.x + ray.dir.y * ray.dir.y);
    float distance1 = sqrt ((point1.x-ray.startCoord.x)*(point1.x-ray.startCoord.x) + (point1.y-ray.startCoord.y)*(point1.y-ray.startCoord.y));
    float distance2 = sqrt ((point2.x-ray.startCoord.x)*(point2.x-ray.startCoord.x) + (point2.y-ray.startCoord.y)*(point2.y-ray.startCoord.y));
    if ((isNear(point1.x, ray.startCoord.x) && isNear(point2.x, ray.startCoord.x) && isNear(ray.dir.x, 0)) ||
            ( isNear(point1.y, ray.startCoord.y) && isNear(point2.y, ray.startCoord.y) && isNear(ray.dir.y, 0))) {
        if (distance1 > distance2) {
            Line tempLine;
            tempLine.startCoord = point2;
            tempLine.param = distance2/rayLen;
            return tempLine;
        }
        else {
            Line tempLine;
            tempLine.startCoord = point1;
            tempLine.param = distance1/rayLen;
            return tempLine;
        }

    }
*/


    /// SOLVE FOR T1 & T2
    /// ray.startCoord.x+ray.dirX*T1 = s_px+line.startCoord.x*T2 && ray.startCoord.y+ray.dirY*T1 = line.startCoord.y+line.startCoord.y*T2
    /// ==> T1 = (s_px+line.startCoord.x*T2-ray.startCoord.x)/ray.dirX = (line.startCoord.y+line.startCoord.y*T2-ray.startCoord.y)/ray.dirY
    /// ==> s_px*ray.dirY + line.startCoord.x*T2*ray.dirY - ray.startCoord.x*ray.dirY = line.startCoord.y*ray.dirX + line.startCoord.y*T2*ray.dirX - ray.startCoord.y*ray.dirX
    /// ==> T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-s_px))/(line.startCoord.x*ray.dirY - line.startCoord.y*ray.dirX)
	float T2 = (ray.dir.x*(line.startCoord.y-ray.startCoord.y) + ray.dir.y*(ray.startCoord.x-line.startCoord.x))/(line.dir.x*ray.dir.y - line.dir.y*ray.dir.x);
    float T1 = (line.startCoord.x+line.dir.x*T2-ray.startCoord.x)/ray.dir.x;
    /// Must be within parametic whatevers for RAY/SEGMENT
    if(T1<0) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        return tempLine;
    }
    if(T2 < 0 || T2 > 1) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        return tempLine;
    }

    /// Return the POINT OF INTERSECTION
    Line tempLine;
    tempLine.startCoord = Vector2f(ray.startCoord.x+ray.dir.x*T1, ray.startCoord.y+ray.dir.y*T1);
    tempLine.param = T1;
    return tempLine;
}

VertexArray RayTracing::createMesh(Color color) {
	///make orderArray
	int meshSize;
	if (isRestricted)
		meshSize = raysVertex.size() + 1;
	else 
		meshSize = raysVertex.size() + 2;

	VertexArray lightMesh(TriangleFan, meshSize);

	///add central point
	lightMesh[0].position = raysVertex[0][0].position; 
	lightMesh[0].color = color;
	///add points clockwise/counter-clockwise
	for (int i = 1; i < raysVertex.size() + 1; ++i) {
		(lightMesh)[i].position = raysVertex[i-1][1].position;
		(lightMesh)[i].color = color;

	}
	///add closing vertex if not restricted
	if (!isRestricted) {
		(lightMesh)[raysVertex.size() + 1].position = raysVertex[0][1].position;
		(lightMesh)[raysVertex.size() + 1].color = color;
	}
	
	return lightMesh;
}

void RayTracing::calculateIntersections() {
    ///for each ray
	for (auto &e : raysVertex) {
		Line prevIntersection;
        prevIntersection.param = 1; ///set to -10 if some problems with rays length occur
        Line ray;
		ray.startCoord = e[0].position;
		ray.dir = e[1].position - ray.startCoord;
		///for each edge
		for (auto k : edges) {
			Line tempLine = getPartIntersection(ray, k);
	    	
			if (tempLine.startCoord.x != -10 && tempLine.startCoord.y != -10) { 
		        if (tempLine.param < prevIntersection.param || prevIntersection.param == -10) {
		            e[1] = Vertex(tempLine.startCoord, Color::Red);
		            prevIntersection.param = tempLine.param;
		        }
		    }
		}
	}
}

///set new obstacles if level changed
void RayTracing::updateObstacles(Level *level) {
    convertTileMapToPolyMap(level);
    convertPolyMapToVertices();
}

///Pass all lines of light mesh to Level function to set all intersecting tiles as lighted (active)
void RayTracing::setActiveTiles(Level *level) {
    std::array<Vertex, 2> currLine;
    /// iterate through all lines in mesh
    for(int i = 0; i < raysVertex.size()-1; ++i) {
        currLine[0] = raysVertex.at(i)[1];
        currLine[1] = raysVertex.at(i+1)[1];
        /// check only horizontal / vertical lines
        if (abs(currLine[0].position.x - currLine[1].position.x) < 1. || abs(currLine[0].position.y - currLine[1].position.y) < 1.)
            level->setDynamicTiles(currLine);
    }
    /// check closing line if view angle is 2*PI
    if(!isRestricted) {
        currLine[0] = raysVertex.at(0)[1];
        currLine[1] = raysVertex.at(raysVertex.size() - 1)[1];
        level->setDynamicTiles(currLine);
    }
}