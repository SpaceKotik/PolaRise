
#include <SFML/Graphics.hpp>
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include "game.hpp"
#include "tile.hpp"
#include "rayTracing.hpp"


extern const int scale;
extern const float delay;
extern const int field_x;
extern const int field_y;
extern const int mines;

#define NO_INTERSECTION Vector2f(-1, -1);


RayTracing::RayTracing() {
	this->rays = new Line;
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
								(field_y*scale - rays[0].startCoord.y));
}

Vector2f RayTracing::getPartIntersection(Line ray, Line line) {

	// Are they parallel? If so, no intersect
	float r_mag = sqrt(ray.dirX*ray.dirX+ray.dirY*ray.dirY);
	float s_mag = sqrt(line.dirX*line.dirX+ line.dirY*line.dirY);
	if(ray.dirX/r_mag == line.dirX/s_mag && ray.dirY/r_mag == line.dirY/s_mag){ // Directions are the same.
		return NO_INTERSECTION;
	}

	// SOLVE FOR T1 & T2
	// ray.startCoord.x+ray.dirX*T1 = s_px+line.startCoord.x*T2 && ray.startCoord.y+ray.dirY*T1 = line.startCoord.y+line.startCoord.y*T2
	// ==> T1 = (s_px+line.startCoord.x*T2-ray.startCoord.x)/ray.dirX = (line.startCoord.y+line.startCoord.y*T2-ray.startCoord.y)/ray.dirY
	// ==> s_px*ray.dirY + line.startCoord.x*T2*ray.dirY - ray.startCoord.x*ray.dirY = line.startCoord.y*ray.dirX + line.startCoord.y*T2*ray.dirX - ray.startCoord.y*ray.dirX
	// ==> T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-s_px))/(line.startCoord.x*ray.dirY - line.startCoord.y*ray.dirX)
	float T2 = (ray.dirX*(line.startCoord.y-ray.startCoord.y) + ray.dirY*(ray.startCoord.x-line.startCoord.x))/(line.startCoord.x*ray.dirY - line.startCoord.y*ray.dirX);
	float T1 = (line.startCoord.x+line.startCoord.x*T2-ray.startCoord.x)/ray.dirX;

	// Must be within parametic whatevers for RAY/SEGMENT
	if(T1<0)
		return NO_INTERSECTION;
	if(T2<0 || T2>1)
		return NO_INTERSECTION;

	// Return the POINT OF INTERSECTION
	return Vector2f(ray.startCoord.x+ray.dirX*T1, ray.startCoord.y+ray.dirY*T1);
	//return;
}

/*Vector2f RayTracing::getIntersection(Line ray) {



	//return;
}*/	