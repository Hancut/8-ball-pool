#include "Ball.h"
#include <include/glm.h>
#include <Core/Engine.h>
#include <math.h>

using namespace std;


Ball::Ball(float radius){
	this->radius = radius;
	this->moving = false;
	this->movingOut = false;
	this->angle = 0;
	this->isOut = false;
	this->col = false;
	this->printStat = false;
}

Ball::~Ball() {

}

void Ball::SetColor(int color) {
	this->color = color;
}

void Ball::SetPoint(float x, float y, float z) {
	this->center.x = x;
	this->center.y = y;
	this->center.z = z;
}

float Ball::distance(Ball *ball) {
	return sqrt((this->center.x - ball->center.x) * (this->center.x - ball->center.x) + (this->center.z - ball->center.z) * (this->center.z - ball->center.z));
}