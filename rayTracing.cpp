
#include <SFML/Graphics.hpp>
//#include <windows.h>
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

const float angle = 0.001;



//const Color lightColor = Color(23*2, 23*2, 23*2);
//const Color lightColor = Color(210, 210, 160);
//const Color lightColor = Color(102, 178, 255);
//const Color lightColor = Color(18, 32, 46);
const Color lightColor = Color(18*5, 32*5, 46*5);

#define NO_INTERSECTION Vector2f(-10, -10);

Vector2f getRectPointPos(Tile rect, int point) {
    return rect.getRect().getTransform().transformPoint(rect.getRect().getPoint(point));
}

void RayTracing::convertTileMapToPolyMap(Level *level) {
	edges.clear();



	for (int i = 0; i < field_y; ++i) {
		for (int j = 0; j < field_x; ++j) {
			Cell tempCell;
			for (int k = 0; k < 4; ++k) {
				tempCell.edgeExist[k] = false;
				tempCell.edgeId[k] = -1;
				if (level->getField()->tiles[i*field_x + j].checkIfBlue())
					tempCell.exist = true;
				else
					tempCell.exist = false;
			}
			processingCells.push_back(tempCell);
		}
	}

	for (int i = 0; i < field_y; ++i) {
		for (int j = 0; j < field_x; ++j) {
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
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[i], 0);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[i], 3).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[i], 3).y - newEdge.startCoord.y;

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
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[i], 1);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[i], 2).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[i], 2).y - newEdge.startCoord.y;

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
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[i], 0);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[i], 1).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[i], 1).y - newEdge.startCoord.y;

						int edge_id = edges.size();

						edges.push_back(newEdge);

						processingCells[self].edgeId[UP] = edge_id;
						processingCells[self].edgeExist[UP] = true;
					}
				}


				//check down neighbour
				if (j != field_y-1 && !processingCells[down].exist) {

					//edge existing?
					if (j != 0 && processingCells[left].edgeExist[DOWN]) {

						edges[processingCells[left].edgeId[DOWN]].dirX += scale;
						processingCells[self].edgeId[DOWN] = processingCells[up].edgeId[LEFT];
					}
					//if not, make one
					else {
						Line newEdge;
						newEdge.startCoord = getRectPointPos(level->getField()->tiles[i], 1);//
						newEdge.dirX = getRectPointPos(level->getField()->tiles[i], 2).x - newEdge.startCoord.x;
						newEdge.dirY = getRectPointPos(level->getField()->tiles[i], 2).y - newEdge.startCoord.y;

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











RayTracing::RayTracing() {
	mousePos = Vector2f(100, 100);
}




void RayTracing::update(Level *level, Window *window, Vector2f mousePos) {
	linesCount = level->getTileCount()*4 + 4;
	//find all vertices
	vertices = new Vector2f[linesCount];
	std::vector<Tile> tempField = level->getField()->tiles;
	std::vector<Tile> allTiles;
	//allTiles.clear();
	for (int i = 0; i < field_x*field_y; ++i) {
		if (tempField[i].checkIfBlue()) {
			allTiles.push_back(tempField[i]);
		}
	}




	for (int i = 0; i < level->getTileCount(); ++i) {
		for (int j = 0; j < 4; ++j) {
			vertices[i*4+j] = getRectPointPos(allTiles[i], j);
		}

	}
	vertices[linesCount-4] = Vector2f(0.f, 0.f);
	vertices[linesCount-4+1] = Vector2f(window->getSize().x, 0.f);
	vertices[linesCount-4+2] = Vector2f(0.f, window->getSize().y);
	vertices[linesCount-4+3] = Vector2f(window->getSize().x, window->getSize().y);


	//find all lines
	lines = new Line[linesCount];
	for (int i = 0; i < level->getTileCount(); ++i) {
		for (int j = 0; j < 4; ++j) {

            lines[i*4+j].startCoord.x = getRectPointPos(allTiles[i], j).x;
            lines[i*4+j].startCoord.y = getRectPointPos(allTiles[i], j).y;

            lines[i*4+j].dirX = (getRectPointPos(allTiles[i], (j+1) % 4).x - lines[i*4+j].startCoord.x);
            lines[i*4+j].dirY = (getRectPointPos(allTiles[i], (j+1) % 4).y - lines[i*4+j].startCoord.y);

		}
	}
	//set borders

    int tempX = window->getSize().x;
	int tempY = window->getSize().y;

	lines[linesCount-4].startCoord.x = 0;
    lines[linesCount-4].startCoord.y = 0;

    lines[linesCount-4].dirX = window->getSize().x;
    lines[linesCount-4].dirY = 0;

    lines[linesCount-4+1].startCoord.x = window->getSize().x;
    lines[linesCount-4+1].startCoord.y = 0;

    lines[linesCount-4+1].dirX = 0;
    lines[linesCount-4+1].dirY = window->getSize().y;

    lines[linesCount-4+2].startCoord.x = window->getSize().x;
    lines[linesCount-4+2].startCoord.y = window->getSize().y;

    lines[linesCount-4+2].dirX = (-tempX);
    lines[linesCount-4+2].dirY = 0;

    lines[linesCount-4+3].startCoord.x = 0;
    lines[linesCount-4+3].startCoord.y = window->getSize().y;

    lines[linesCount-4+3].dirX = 0;
    lines[linesCount-4+3].dirY = -tempY;


	//set raysVertex
	raysVertex = new Vertex* [linesCount*3];
	for (int i = 0; i < linesCount*3; ++i) {
		raysVertex[i] = new Vertex[2];
	}

	//set cursor position
	//Vector2f mousePos;
	for (int i = 0; i < linesCount*3; ++i) {

         raysVertex[i][0] = Vertex(mousePos, Color::Blue);
    }
    //set 2nd dot of main raysVertex
    for (int i = 0; i < linesCount; ++i) {
    	raysVertex[i][1] = Vertex(vertices[i], Color::Blue);

    }
    //set 2nd dot of assistinng raysVertex
    for (int i = linesCount; i < linesCount*3; i += 2) {

    	Transform rotation1;
    	rotation1.rotate(-angle, mousePos);//  .scale(1, 1, mousePos.x, mousePos.y);
    	//rotate
    	raysVertex[i][1] = Vertex(vertices[(i-linesCount)/2], Color::Blue);
    	raysVertex[i][1].position = rotation1.transformPoint(raysVertex[i][1].position);

    	Transform rotation2;
    	rotation2.rotate(2*angle, mousePos);
    	//rotate
    	raysVertex[i+1][1] = Vertex(vertices[(i-linesCount)/2], Color::Blue);
    	raysVertex[i+1][1].position = rotation2.transformPoint(raysVertex[i][1].position);
    }


    //set main rays
	rays = new Line[linesCount*3];

	for (int i = 0; i < linesCount; ++i) {
		for (int j = 0; j < 4; ++j) {

            rays[i].startCoord.x = raysVertex[i][0].position.x;
            rays[i].startCoord.y = raysVertex[i][0].position.y;

            rays[i].dirX = (vertices[i].x - rays[i].startCoord.x);
            rays[i].dirY = (vertices[i].y - rays[i].startCoord.y);

		}
	}
	//set assisting rays
	for (int i = linesCount; i < linesCount*3; ++i) {

		rays[i].startCoord.x = raysVertex[i][0].position.x;
        rays[i].startCoord.y = raysVertex[i][0].position.y;

        rays[i].dirX = (raysVertex[i][1].position.x - rays[i].startCoord.x);
        rays[i].dirY = (raysVertex[i][1].position.y - rays[i].startCoord.y);
	}

	//important!
    calculateIntersections();


}

int RayTracing::getLineCount() {
	return linesCount;
}

Vertex** RayTracing::getRays() {
	return raysVertex;
}

void RayTracing::clear() {
	delete [] vertices;
	delete [] lines;
	delete [] rays;
	for (int i = 0; i < linesCount*3; ++i) {
		delete [] raysVertex[i];
	}
	delete [] raysVertex;
	//delete [] lightMesh;
}


Line RayTracing::getPartIntersection(Line ray, Line line) {

    // Are they parallel? If so, no intersect
    float r_mag = sqrt(ray.dirX*ray.dirX+ray.dirY*ray.dirY);
    float s_mag = sqrt(line.dirX*line.dirX+ line.dirY*line.dirY);
    if(ray.dirX/r_mag == line.dirX/s_mag && ray.dirY/r_mag == line.dirY/s_mag){ // Directions are the same.
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
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
    if(T1<-0.0001) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
        return tempLine;
    }
    if(T2<0 || T2>1) {
        Line tempLine;
        tempLine.startCoord = NO_INTERSECTION;
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
	VertexArray lightMesh(TriangleFan, linesCount*3 + 2);
	int order[linesCount*3];
	for (int i = 0; i < linesCount*3 + 1; ++i) {
		order[i] = i;
	}

	//sort orderArray

	for (int i = 1; i < linesCount*3 + 1; i++) {
		for (int j = 1; j < linesCount*3 + 1-i; ++j)
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
	for (int i = 1; i < linesCount*3 + 1; ++i) {
		(lightMesh)[i].position = raysVertex[order[i-1]][1].position;
		(lightMesh)[i].color = lightColor;

		(lightMesh)[i].texCoords = lightSourceTextureCenter + (lightMesh[0].position  - lightMesh[i].position);


	}

	(lightMesh)[linesCount*3 + 1].position = raysVertex[order[0]][1].position;
	(lightMesh)[linesCount*3 + 1].color = lightColor;
	(lightMesh)[linesCount*3 + 1].texCoords = lightSourceTextureCenter + (lightMesh[0].position  - lightMesh[linesCount*3 + 1].position);

	return lightMesh;
}


void RayTracing::calculateIntersections() {

	for (int i = 0; i < linesCount*3; ++i) { 

		Line prevIntersection;
        prevIntersection.param = 10000;///!!!! may be the problem
        //raysVertex[i][1] = Vertex(vertices[i], Color::Blue);
		for (int j = 0; j < linesCount; ++j) {

			Line tempLine = getPartIntersection(rays[i], lines[j]);
	    	
			if (tempLine.startCoord.x != -10 && tempLine.startCoord.y != -10) { 
		        if (tempLine.param < prevIntersection.param) {
		            raysVertex[i][1] = Vertex(tempLine.startCoord, Color::Red);
		            prevIntersection.param = tempLine.param;

		        }

		    }
		}
	}

}
