#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

using namespace std;

struct Point
{
	float x, y, z;
};

class Ball
{
public:

	Ball::Ball(float radius);
	Ball::~Ball();
	float Ball::distance(Ball *ball);

	bool printStat;
	Point center;
	float radius;
	int color;
	bool moving;
	bool movingOut;
	bool col;
	float colDistance;
	Ball* ball;
	float speed;
	float angle;
	float isOut;

	void SetPoint(float x, float y, float z);
	void SetColor(int color);
	
};

