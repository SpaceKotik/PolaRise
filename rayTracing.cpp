#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include "game.hpp"
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"


extern const int scale;
extern const int field_x;
extern const int field_y;
extern const Vector2f lightSourceTextureCenter;

#define DOWN 0
#define LEFT 1
#define UP 2
#define RIGHT 3

#define NO_INTERSECTION Vector2f(-10, -10);

//const Color lightColor = Color(23*2, 23*2, 23*2);
//const Color lightColor = Color(210, 210, 160);
//const Color lightColor = Color(102, 178, 255);
//const Color lightColorBlue = Color(18, 32, 46);		//Blue for 5 sources


//const Color lightColorRed = Color(50, 10, 10);		//Red for 5 sources
//const Color lightColorBlue = Color(20, 20, 50);		//Blue for 5 sources

const Color lightColorBlue = Color(102, 102, 255);	//Blue for 1 source
const Color lightColorRed = Color(255, 51, 51);		//Red for 1 source
const float angle = 0.001;


Vector2f getRectPointPos(Tile rect, int point) {
    return rect.getRect().getTransform().transformPoint(rect.getRect().getPoint(point));
}

RayTracing::RayTracing() {
	lightColor = lightColorBlue;
}

void RayTracing::convertTileMapToPolyMap(Level *level, Window *window) {
	std::vector<Cell> processingCells;
	processingCells.clear();
	edges.clear();

	//Set borders
	Vector2f windowSize = (Vector2f)window->getSize();

	Line newBorder;
	newBorder.startCoord = Vector2f(0, 0);
	newBorder.dirX = windowSize.x;
	newBorder.dirY = 0;
	edges.push_back(newBorder);

	newBorder.startCoord = Vector2f(windowSize.x, 0);
	newBorder.dirX = 0;
	newBorder.dirY = windowSize.y;
	edges.push_back(newBorder);

	newBorder.startCoord = Vector2f(windowSize.x, windowSize.y);
	newBorder.dirX = -windowSize.x;
	newBorder.dirY = 0;
	edges.push_back(newBorder);

	newBorder.startCoord = Vector2f(0, windowSize.y);
	newBorder.dirX = 0;
	newBorder.dirY = -windowSize.y;
	edges.push_back(newBorder);

	//Reset edges information
	for (int i = 0; i < field_y; ++i) {
		for (int j = 0; j < field_x; ++j) {
			Cell tempCell;
			for (int k = 0; k < 4; ++k) {
				tempCell.edgeExist[k] = false;
				tempCell.edgeId[k] = -1;

				if ((level->getState() == Blue && level->getField()->tiles[i*field_x + j].checkIfBlue()) ||
					(level->getState() == Red && level->getField()->tiles[i*field_x + j].checkIfRed())) {
					if ((level->getState() == Red && level->getField()->tiles[i*field_x + j].checkIfSolidRed()) ||
					 (level->getState() == Blue && level->getField()->tiles[i*field_x + j].checkIfSolidBlue()))
						tempCell.exist = true;
				}
				else
					tempCell.exist = false;
			}
			processingCells.push_back(tempCell);
		}
	}

	//Do logic ad push edges to array
	for (int i = 0; i < field_y; ++i) {
		for (int j = 0; j < field_x; ++j) {

			//indexes for cells
			int self = i*field_x + j;
			int left = i*field_x + j - 1;
			int right = i*field_x + j + 1;
			int up = (i-1)*field_x + j;
			int down =(i+1)*field_x + j;

			if (processingCells[self].exist) {

				//check left neighbour
				if (j != 0 && !processingCells[left].exist) {

					//edge existing?
					if ( i != 0 && processingCells[up].edgeExist[LEFT]) {

						edges[processingCells[up].edgeId[LEFT]].dirY += scale;
						processingCells[self].edgeId[LEFT] = processingCells[up].edgeId[LEFT];
						processingCells[self].edgeExist[LEFT] = true;
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 0);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[self], 3).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[self], 3).y - newEdge.startCoord.y;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[LEFT] = edge_id;
						processingCells[self].edgeExist[LEFT] = true;
					}
				}

				//check right neighbour
				if (j != field_x-1 && !processingCells[right].exist) {

					//edge existing?
					if ( i != 0 && processingCells[up].edgeExist[RIGHT]) {

						edges[processingCells[up].edgeId[RIGHT]].dirY += scale;
						processingCells[self].edgeId[RIGHT] = processingCells[up].edgeId[RIGHT];
						processingCells[self].edgeExist[RIGHT] = true;
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 1);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[self], 2).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[self], 2).y - newEdge.startCoord.y;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[RIGHT] = edge_id;
						processingCells[self].edgeExist[RIGHT] = true;
					}
				}

				//check up neighbour
				if (i != 0 && !processingCells[up].exist) {


					//edge existing?
					if (j != 0 && processingCells[left].edgeExist[UP]) {

						edges[processingCells[left].edgeId[UP]].dirX += scale;
						processingCells[self].edgeId[UP] = processingCells[left].edgeId[UP];
						processingCells[self].edgeExist[UP] = true;
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 0);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[self], 1).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[self], 1).y - newEdge.startCoord.y;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[UP] = edge_id;
						processingCells[self].edgeExist[UP] = true;
					}
				}

				//check down neighbour
				if (j != field_x-1 && !processingCells[down].exist) {

					//edge existing?
					if (j != 0 && processingCells[left].edgeExist[DOWN]) {

						edges[processingCells[left].edgeId[DOWN]].dirX += scale;
						processingCells[self].edgeId[DOWN] = processingCells[left].edgeId[DOWN];
						processingCells[self].edgeExist[DOWN] = true;
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[self], 3);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[self], 2).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[self], 2).y - newEdge.startCoord.y;

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

void RayTracing::convertPolyMapToVertices() {
	vertices.clear();
	for (auto e : edges) {

		if (std::find(vertices.begin(), vertices.end(), e.startCoord) == vertices.end())
			vertices.push_back(e.startCoord);

		Vector2f tempVertex = e.startCoord + Vector2f(e.dirX, e.dirY);
		if (std::find(vertices.begin(), vertices.end(), tempVertex) == vertices.end())
			vertices.push_back(tempVertex);
	}
	std::cout << "\033[2J\033[1;1H";
	std::cout << "Rays on screen: " << vertices.size()*3 << '\n';
}

void RayTracing::update(Level *level, Window *window, Vector2f mousePos) {
	raysVertex.clear();

	//set raysVertex for main rays
	for (int i = 0; i < vertices.size(); ++i) {
		std::array<Vertex, 2> currRay;
		currRay[0] = Vertex(mousePos, Color::Blue);
		currRay[1] = vertices.at(i);
		raysVertex.push_back(currRay);
    }
    //set raysVertex for assisting rays
    for (int i = 0; i < vertices.size(); ++i) {
    	std::array<Vertex, 2> currRay;
    	currRay[0] = Vertex(mousePos, Color::Blue);

    	Transform rotation1;
    	rotation1.rotate(-angle, mousePos);//.scale(500, 500, mousePos.x, mousePos.y);
    	//rotate
    	currRay[1] = Vertex(vertices[i], Color::Blue);
    	currRay[1].position = rotation1.transformPoint(currRay[1].position);
    	raysVertex.push_back(currRay);

    	Transform rotation2;
    	rotation2.rotate(angle, mousePos);//.scale(500, 500, mousePos.x, mousePos.y);
    	//rotate
    	currRay[1] = Vertex(vertices[i], Color::Blue);
    	currRay[1].position = rotation2.transformPoint(currRay[1].position);
    	raysVertex.push_back(currRay);
    }

	//important!
    calculateIntersections();
}

float getLen(Line line) {
	return sqrt((line.startCoord.x - line.dirX)*(line.startCoord.x - line.dirX) + (line.startCoord.y - line.dirY)*(line.startCoord.y - line.dirY));
}

Line RayTracing::getPartIntersection(Line ray, Line line) {
	/*Line ray;
	ray.startCoord = raysVertex[0].position;
	ray.dirX = raysVertex[1].position.x - ray.startCoord.x;
	ray.dirY = raysVertex[1].position.y - ray.startCoord.y;*/


    // Are they parallel? If so, no intersect
    float r_mag = sqrt(ray.dirX*ray.dirX+ray.dirY*ray.dirY);
    float s_mag = sqrt(line.dirX*line.dirX+line.dirY*line.dirY);

    float distance1 = sqrt ((line.startCoord.x-ray.startCoord.x)*(line.startCoord.x-ray.startCoord.x) + (line.startCoord.y-ray.startCoord.y)*(line.startCoord.y-ray.startCoord.y));
    float distance2 = sqrt ((line.startCoord.x + line.dirX-ray.startCoord.x)*(line.startCoord.x + line.dirX-ray.startCoord.x)
     + (line.startCoord.y + line.dirY-ray.startCoord.y)*(line.startCoord.y + line.dirY-ray.startCoord.y));




    if(abs(ray.dirX/r_mag) == abs(line.dirX/s_mag) && abs(ray.dirY/r_mag) == abs(line.dirY/s_mag)){ // Directions are the same.
    	/*if (distance1 > distance2) {
    		Line tempLine;
		    tempLine.startCoord = Vector2f(line.startCoord.x + line.dirX, line.startCoord.y + line.dirY);
		    Line tempRay;
		    tempRay.startCoord = ray.startCoord;
		    tempRay.dirX = line.startCoord.x + line.dirX;
		    tempRay.dirY = line.startCoord.y + line.dirY;
		    tempLine.param = getLen(tempRay) / getLen(ray);
		    return tempLine;
    	}
    	else {
    		Line tempLine;
		    tempLine.startCoord = Vector2f(line.startCoord.x, line.startCoord.y);
		    Line tempRay;
		    tempRay.startCoord = ray.startCoord;
		    tempRay.dirX = line.startCoord.x + line.dirX;
		    tempRay.dirY = line.startCoord.y + line.dirY;
		    tempLine.param = getLen(tempRay) / getLen(ray);
		    return tempLine;
    	}*/



        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
       // tempLine.param = 100000;
        return tempLine;
	}

    // SOLVE FOR T1 & T2
    // ray.startCoord.x+ray.dirX*T1 = s_px+line.startCoord.x*T2 && ray.startCoord.y+ray.dirY*T1 = line.startCoord.y+line.startCoord.y*T2
    // ==> T1 = (s_px+line.startCoord.x*T2-ray.startCoord.x)/ray.dirX = (line.startCoord.y+line.startCoord.y*T2-ray.startCoord.y)/ray.dirY
    // ==> s_px*ray.dirY + line.startCoord.x*T2*ray.dirY - ray.startCoord.x*ray.dirY = line.startCoord.y*ray.dirX + line.startCoord.y*T2*ray.dirX - ray.startCoord.y*ray.dirX
    // ==> T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-s_px))/(line.startCoord.x*ray.dirY - line.startCoord.y*ray.dirX)
	float T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-line.startCoord.x))/(line.dirX*ray.dirY - line.dirY*ray.dirX);
    float T1 = (line.startCoord.x+line.dirX*T2-ray.startCoord.x)/ray.dirX;
    // Must be within parametic whatevers for RAY/SEGMENT
    if(T1<0) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        //tempLine.param = 100000;
        return tempLine;
    }
    if(T2<0 || T2>1) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        //tempLine.param = 100000;
        return tempLine;
    }
        

    // Return the POINT OF INTERSECTION
    Line tempLine;
    tempLine.startCoord = Vector2f(ray.startCoord.x+ray.dirX*T1, ray.startCoord.y+ray.dirY*T1);
    tempLine.param = T1;
    return tempLine;
}


//function coparing angles of rays
int cmp(const void *a, const void *b) {
		 if ((atan2(((Line*)a)->dirY, ((Line*)a)->dirX) - atan2(((Line*)b)->dirY, ((Line*)b)->dirX)) < 0) {
		 	return -1;
		 }
		 else {
		 	return 1;
		 }

}

VertexArray RayTracing::createMesh() {
	//make orderArray
	VertexArray lightMesh(TriangleFan, raysVertex.size() + 2);
	int order[raysVertex.size()];
	for (int i = 0; i < raysVertex.size() ; ++i) {
		order[i] = i;
	}
	std::vector<Line> rays;
	for (auto e: raysVertex) {
		Line currRay;
		currRay.startCoord = e[0].position;
		currRay.dirX = e[1].position.x - e[0].position.x;
		currRay.dirY = e[1].position.y - e[0].position.y;
		rays.push_back(currRay);
	}

	//sort orderArray

	for (int i = 1; i < raysVertex.size() + 1; i++) {
		for (int j = 1; j < raysVertex.size() + 1-i; ++j)
		{
			if(atan2(rays[order[j-1]].dirY, rays[order[j-1]].dirX) > atan2(rays[order[j+1-1]].dirY, rays[order[j+1-1]].dirX)){
				//swap
				order[j-1] = order[j]+ order[j-1];
				order[j] = order[j-1] - order[j];
				order[j-1] =  order[j-1] - order[j];
				/*int temp = order[j-1];
				order[j-1] = order[j+1-1];
				order[j+1-1] = temp;*/
			}
		}
	}


		//MUST BE DONE!





		//qsort(order, linesCount*3+1, sizeof(int), cmp);
	
	lightMesh[0].position = raysVertex[0][0].position;
	lightMesh[0].texCoords = lightSourceTextureCenter; 
	lightMesh[0].color = lightColor;
	for (int i = 1; i < raysVertex.size() + 1; ++i) {
		(lightMesh)[i].position = raysVertex[order[i-1]][1].position;
		(lightMesh)[i].color = lightColor;

		(lightMesh)[i].texCoords = lightSourceTextureCenter + (lightMesh[0].position  - lightMesh[i].position);


	}

	(lightMesh)[raysVertex.size() + 1].position = raysVertex[order[0]][1].position;
	(lightMesh)[raysVertex.size() + 1].color = lightColor;
	(lightMesh)[raysVertex.size() + 1].texCoords = lightSourceTextureCenter + (lightMesh[0].position  - lightMesh[raysVertex.size() + 1].position);

	return lightMesh;
}

VertexArray RayTracing::createVisibleBorders() {

	VertexArray lightMesh(LineStrip, raysVertex.size() + 1);

	//make orderArray
	
	int order[raysVertex.size()];
	for (int i = 0; i < raysVertex.size() ; ++i) {
		order[i] = i;
	}
	std::vector<Line> rays;
	for (auto e: raysVertex) {
		Line currRay;
		currRay.startCoord = e[0].position;
		currRay.dirX = e[1].position.x - e[0].position.x;
		currRay.dirY = e[1].position.y - e[0].position.y;
		rays.push_back(currRay);
	}

	//sort orderArray

	for (int i = 1; i < raysVertex.size() + 1; i++) {
		for (int j = 1; j < raysVertex.size() + 1-i; ++j)
		{
			if(atan2(rays[order[j-1]].dirY, rays[order[j-1]].dirX) > atan2(rays[order[j+1-1]].dirY, rays[order[j+1-1]].dirX)){
				//swap
				order[j-1] = order[j]+ order[j-1];
				order[j] = order[j-1] - order[j];
				order[j-1] =  order[j-1] - order[j];
			}
		}
	}


	//MUST BE DONE!

	//qsort(order, linesCount*3+1, sizeof(int), cmp);


	for (int i = 0; i < raysVertex.size(); ++i) {
		(lightMesh)[i].position = raysVertex[order[i]][1].position;
		(lightMesh)[i].color = Color::White;

	}

	(lightMesh)[raysVertex.size()].position = raysVertex[order[1]][1].position;
	(lightMesh)[raysVertex.size()].color = Color::White;


	return lightMesh;
}


void RayTracing::calculateIntersections() {
	//std::cout << "\n NEW ITERATION \n";
	for (auto &e : raysVertex) {
		Line prevIntersection;
        prevIntersection.param = 100000;///!!!! may be the problem
        //raysVertex[i][1] = Vertex(vertices[i], Color::Blue);
        Line ray;
		ray.startCoord = e[0].position;
		ray.dirX = e[1].position.x - ray.startCoord.x;
		ray.dirY = e[1].position.y - ray.startCoord.y;
        //std::cout << "\n new ray: \n";
		for (auto k : edges) {

			Line tempLine = getPartIntersection(ray, k);
	    	
			if (tempLine.startCoord.x != -10 && tempLine.startCoord.y != -10) { 
				//std::cout << "temp: " << tempLine.param << "| ";
		        if (tempLine.param < prevIntersection.param) {
		            e[1] = Vertex(tempLine.startCoord, Color::Red);
		            prevIntersection.param = tempLine.param;


		        }


		    }
		    //std::cout << prevIntersection.param << ' '; 
		}
		//std::cout << '\n';
	}


}

void RayTracing::changeLightColor() {
	if (lightColor == lightColorBlue)
		lightColor = lightColorRed;
	else
		lightColor = lightColorBlue;
}
