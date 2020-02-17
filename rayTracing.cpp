
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
extern const float delay;
extern const int field_x;
extern const int field_y;

#define NO_INTERSECTION Vector2f(-10, -10);

Vector2f getRectPointPos(Tile rect, int point) {
    return rect.getRect().getTransform().transformPoint(rect.getRect().getPoint(point));
}



RayTracing::RayTracing() {
	/*this->rays = new Line;
	this->rays[0].startCoord = Vector2f(15.f, 90.f);
	this->rays[0].dirX = field_x*scale - rays[0].startCoord.x/sqrt((field_x*scale - rays[0].startCoord.x)*
																	(field_x*scale - rays[0].startCoord.x) +
																	(field_y*scale - rays[0].startCoord.y)*
																	(field_y*scale - rays[0].startCoord.y));
	this->rays[0].dirY = field_y*scale - rays[0].startCoord.y/sqrt((field_x*scale - rays[0].startCoord.x)*
																	(field_x*scale - rays[0].startCoord.x) +
																	(field_y*scale - rays[0].startCoord.y)*
																	(field_y*scale - rays[0].startCoord.y));
	this->rays[0].param = sqrt((field_x*scale - rays[0].startCoord.x)*
								(field_x*scale - rays[0].startCoord.x) +
								(field_y*scale - rays[0].startCoord.y)*
								(field_y*scale - rays[0].startCoord.y));*/
}




void RayTracing::update(Level *level, Window *window) {

	linesCount = level->getTileCount()*4;
	//find all vertices
	vertices = new Vector2f[linesCount + 4];
	Tile* allTiles =level->getField()->tile;
	for (int i = 0; i < level->getTileCount(); ++i) {
		for (int j = 0; j < 4; ++j) {
			vertices[i*4+j] = getRectPointPos(allTiles[i], j);
		}

	}
	vertices[linesCount] = Vector2f(0.f, 0.f);
	vertices[linesCount+1] = Vector2f(window->getSize().x, 0.f);
	vertices[linesCount+2] = Vector2f(0.f, window->getSize().y);
	vertices[linesCount+3] = Vector2f(window->getSize().x, window->getSize().y);




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

	


	//set raysVertex
	raysVertex = new Vertex* [linesCount + 4];
	for (int i = 0; i < linesCount + 4; ++i) {
		raysVertex[i] = new Vertex[2];
	}



	for (int i = 0; i < level->getTileCount()*4 + 4; ++i) {
		Vector2f mousePos;
		mousePos = (Vector2f)Mouse::getPosition(*window);

        if (Mouse::getPosition(*window).x > window->getSize().x)
            mousePos.x = window->getSize().x;
        if (Mouse::getPosition(*window).x < 0)
            mousePos.x = 0;
        if (Mouse::getPosition(*window).y > window->getSize().y)
            mousePos.y = window->getSize().y;
        if (Mouse::getPosition(*window).y < 0)
            mousePos.y = 0;


           raysVertex[i][0] = Vertex(mousePos, Color::Blue);
    }

    for (int i = 0; i < linesCount + 4; ++i) {
    	raysVertex[i][1] = Vertex(vertices[i], Color::Blue);
    }


    //set rays
	rays = new Line[linesCount + 4];

	for (int i = 0; i < linesCount+4; ++i) {
		for (int j = 0; j < 4; ++j) {

            rays[i].startCoord.x = raysVertex[i][0].position.x;
            rays[i].startCoord.y = raysVertex[i][0].position.y;

            rays[i].dirX = (vertices[i].x - rays[i].startCoord.x);
            rays[i].dirY = (vertices[i].y - rays[i].startCoord.y);

		}
	}







    

    //calculateIntersections();
    for (int i = 0; i < linesCount + 4; ++i) { 

		Line prevIntersection;
        prevIntersection.param = 10000;///!!!!
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
	for (int i = 0; i < linesCount + 4; ++i) {
		delete [] raysVertex[i];
	}
	delete [] raysVertex;

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
    if(T1<0) {
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
    //return Vector2f(25, 25);
    //return;






}

void RayTracing::calculateIntersections() {

	//for (int i = 0; i < linesCount + 4; ++i) {
	for (int i = 0; i < 1; ++i) {
		Line prevIntersection;
        prevIntersection.param = 10000;///!!!!
        raysVertex[i][1] = Vertex(vertices[i], Color::Blue);
		for (int j = 0; j < linesCount; ++j) {
	    	
			if (getPartIntersection(rays[i], lines[j]).startCoord.x != -10 && getPartIntersection(rays[i], lines[j]).startCoord.y != -10) {
				raysVertex[i][1] = Vertex(vertices[i], Color::Red);
		        if (getPartIntersection(rays[i], lines[j]).param < prevIntersection.param) {
		            raysVertex[i][1] = Vertex(getPartIntersection(rays[i], lines[j]).startCoord, Color::Red);
		            prevIntersection.param = getPartIntersection(rays[i], lines[j]).param;

		        }

		    }
		}
	}

}