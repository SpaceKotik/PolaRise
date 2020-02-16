
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

/*Vector2f RayTracing::getPartIntersection(Line ray, Line line) {


	//return;
}

Vector2f RayTracing::getIntersection(Line ray) {



	//return;
}*/	