#include "EightBallPool.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform3D.h"
#include "Ball.h"
#include "Player.h"

using namespace std;

bool isTopDown;
bool setWhiteBallPosition;
bool isStart;
float time;

// Players
Player *player1, *player2;

// Table
float xMax, zMax, zMin, xMin;

// cue
int cueMov;
float alpha = 0;

// White Ball
vector<Ball*> balls(15);
Ball *whiteBall;
float ballX, ballY, ballZ;
float speedBall = 2.5;
bool firstHit;

EightBallPool::EightBallPool()
{
}

EightBallPool::~EightBallPool()
{
}

bool allBallsStoped() {
	for (int i = 0; i < 15; i++)
	{
		if (balls[i]->moving == true && balls[i]->movingOut == true)
			return false;
	}
	return true;
}

void hitBall(float deltaTimeSeconds, Ball *ball) {
	float x, z;

	x = ball->center.x - ball->speed * sin(ball->angle) * deltaTimeSeconds;
	z = ball->center.z - ball->speed * cos(ball->angle) * deltaTimeSeconds;
	ball->speed -= deltaTimeSeconds * ball->speed;
	if (ball->speed < 0.1) {
		ball->moving = false;
		if (ball->color == 0) {
			ball->printStat = true;
		}
		return;
	}

	// if the ball hit a side of the table then the angle of direction is changed
	if (x > xMax && ((z + ball->radius / 2 > 0.055 || z - ball->radius / 2 < -0.055) && x >= xMax - 0.055)) {
		ball->angle = -ball->angle;
	}
	if (z > zMax) {
		ball->angle = M_PI - ball->angle;
	}

	if (x < xMin && ((z + ball->radius / 2 > 0.055 || z - ball->radius / 2 < -0.055) && x <= xMin + 0.055)) {
		ball->angle = -ball->angle;
	}
	if (z < zMin) {
		ball->angle = M_PI - ball->angle;
	}
	
	ball->center.x -= ball->speed * sin(ball->angle) * deltaTimeSeconds;
	ball->center.z -= ball->speed * cos(ball->angle) * deltaTimeSeconds;
	if (ball->color == 0) {
		ballX -= whiteBall->speed * sin(whiteBall->angle) * deltaTimeSeconds;
		ballZ -= whiteBall->speed * cos(whiteBall->angle) * deltaTimeSeconds;
	}
}

void takeOut(Ball* ball) {
	//holes are named in this order:
	//h3 h4
	//h2 h5
	//h1 h5
	float err = 0.005;
	float maxdistance = sqrt((ball->radius)*(ball->radius / 2));
	float distance;
	float x, z;

	// hole 1
	x = (xMin - ball->center.x);
	z = (zMax - ball->center.z);
	distance = sqrt(x*x + z*z);
	if (distance < maxdistance + err) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	// hole 2
	if (ball->center.x - ball->radius/2 <= -0.55) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	// hole 3
	x = (xMin - ball->center.x);
	z = (zMin - ball->center.z);
	distance = sqrt(x*x + z * z);
	if (distance < maxdistance + err) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	// hole 4
	x = (xMax - ball->center.x);
	z = (zMin - ball->center.z);
	distance = sqrt(x*x + z * z);
	if (distance < maxdistance + err) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	// hole 5
	if (ball->center.x + ball->radius/2 > 0.55) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	// hole 6
	x = (xMax - ball->center.x);
	z = (zMax - ball->center.z);
	distance = sqrt(x*x + z * z);
	if (distance < maxdistance + err) {
		ball->isOut = true;
		ball->moving = false;
		ball->movingOut = false;
	}

	if (ball->isOut == true) {
		player1->SetPoints(ball);
		player2->SetPoints(ball);
	}
}

Ball* moveBall(Ball *ball, float distance, float deltaTimeSeconds) {
	float x, z;
	x = ball->center.x - distance * sin(ball->angle) * deltaTimeSeconds;
	z = ball->center.z - distance * cos(ball->angle) * deltaTimeSeconds;
	
	
	if (x > xMax && ((z + ball->radius / 2 > 0.055 || z - ball->radius / 2 < -0.055) && x >= xMax - 0.055)) {
		ball->angle = -ball->angle;
	}
	if (z > zMax) {
		ball->angle = M_PI - ball->angle;
	}

	if (x < xMin && ((z + ball->radius / 2 > 0.055 || z - ball->radius / 2 < -0.055) && x <= xMin + 0.055)) {
		ball->angle = -ball->angle;
	}
	if (z < zMin) {
		ball->angle = M_PI - ball->angle;
	}
	
	ball->center.x -= distance * sin(ball->angle) * deltaTimeSeconds;
	ball->center.z -= distance * cos(ball->angle) * deltaTimeSeconds;
	// for global variables of the white ball
	// are used for the camera, camera is moved by white ball movement
	if (ball->color == 0) {
		ballX -= distance * sin(ball->angle) * deltaTimeSeconds;
		ballZ -= distance * cos(ball->angle) * deltaTimeSeconds;
	}
	return ball;
}

bool isCollision(Ball *ball1, Ball *ball2) {
	float nx, ny, nz;
	float err = 0.002;
	nx = (ball1->center.x - ball2->center.x);
	nz = (ball1->center.z - ball2->center.z);
	float distance = sqrt(nx*nx + nz * nz);

	if (distance < ball1->radius) {
		return true;
	}
	return false;
}

bool ballsAndWhiteBallCollosion() {
	for (int i = 0; i < 15; i++)
	{
		if (isCollision(balls[i], whiteBall)) {
			return true;
		}
	}
	return false;
}

void moveBall1(Ball* ball1, Ball *ball2, float nx, float nz, float err, float deltaTimeSeconds) {
	// check collision of 2 balls and in case of each direction they are separated
	// B-----C   ab = 2 * radius;
	//	\    |   bc = |xA - xB|
	//    \ a|   ac = |zA - zB|
	//      \|
	//       A

	if ((nx < err) || (nz < err)) {
		ball1->angle = ball2->angle;
		ball2->moving = true;
		ball2->angle = -ball2->angle;
	}
	else {
		float cos = abs(ball1->center.z - ball2->center.z) / ball1->radius;
		float x, z, x1, z1;
		x = ball2->center.x;
		z = ball2->center.z;
		x1 = ball1->center.x;
		z1 = ball1->center.z;

		ball1->angle = acos(cos);

		if ((x < x1 && z < z1)) {
			ball1->angle += M_PI;
		} 
		else if (x < x1 && z > z1) {
			ball1->angle = -ball1->angle;
		}
		else if ((x > x1 && z < z1)) {
			ball1->angle = - ball1->angle + M_PI;
		}
		ball2->angle = ball1->angle + M_PI / 2;
		 
	}

	ball1->moving = true;
	ball2->moving = true;
	ball1->speed = ball2->speed * 0.95;
	ball2->speed = ball2->speed * 0.3;
	while (isCollision(ball1, ball2)) {
		Ball* ball = moveBall(ball1, ball1->radius, deltaTimeSeconds);
		if (ball1->distance(ball2) - ball->radius > 0) {
			ball1->ball = ball2;
			ball1->col = true;
			ball1->colDistance = ball1->distance(ball2) - ball->radius;
			ball1->movingOut = true;
		}
	}
}

void collision(Ball *ball1, Ball *ball2, float deltaTimeSeconds){
	// check collision between 2 balls
	float nx, ny, nz;
	float err = 0.00001;
	nx = abs(ball1->center.x - ball2->center.x);
	nz = abs(ball1->center.z - ball2->center.z);
	float distance = nx * nx + nz * nz;

	if (distance < ball1->radius * ball1->radius)
	{
		if (ball1->moving == false && ball2->moving == true) {
			moveBall1(ball1, ball2, nx, nz, err, deltaTimeSeconds);
		}
		else if (ball2->moving == false && ball1->moving == true) {
			moveBall1(ball2, ball1, nx, nz, err, deltaTimeSeconds);
		}
		else if (ball2->moving == true && ball1->moving == true) {
				if (ball1->speed > ball2->speed) {
					moveBall1(ball2, ball1, nx, nz, err, deltaTimeSeconds);
				}
				else {
					moveBall1(ball1, ball2, nx, nz, err, deltaTimeSeconds);
				} 
		}
	}
}

void EightBallPool::Init()
{
	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2.5, 0), glm::vec3(0,0,0), glm::vec3(0, 0, -1));

	projectionMatrix = glm::perspective(RADIANS(80), window->props.aspectRatio, 0.01f, 200.0f);

	float x1 = - 0.05 + 0.001;
	float x2 = 0.05;
	float x3 = -0.025;
	float x4 = 0.025;
	float x5 = 0;
	float x6 = -0.020;
	float x7 = 0.020;
	float x8 = 0.04;
	float y1 = 0.01;
	float y2 = 0;
	float y3 = -0.02;
	float y4 =  0.05;
	float z1 = -0.1;
	float z2 = -0.06;
	float z3 = -0.075;
	float z4 = -0.05;
	float z5 = 0;
	float z6 = 0.025;
	float z7 = 0.035;

	float dim = 0.065;
	ballX = -0.5 + dim / 2 + ((1 - dim) / 2);
	ballY = 1 + dim / 2, ballZ = 0.7;

	setWhiteBallPosition = true;
	isTopDown = true;
	isStart = true;
	cueMov = 0;

	// Players
	player1 = new Player();
	player2 = new Player();
	player1->number = 1;
	player2->number = 2;
	player1->playing = true;
	player2->playing = false;

	// White ball
	whiteBall = new Ball(dim);
	whiteBall->color = 0;
	whiteBall->SetPoint(ballX, ballY, ballZ);
	whiteBall->speed = speedBall;
	whiteBall->moving = false;
	firstHit = true;

	
	float yMax = 1 + dim / 2;
	xMax = 0.5 - dim / 2;
	xMin = -0.5 + dim / 2;
	zMax = 1 - dim / 2;
	zMin = -1 + dim / 2;
	float startZ = -0.70;

	int index = 0;
	// arrange the balls
	// line 1
	float start = (1 - (dim * 5)) / 2 -0.5 + dim / 2;
	for (int i = 0; i < 5; i++) {
		Ball *aux = new Ball(dim);
		aux->SetPoint(start + (dim)* i, yMax, startZ);

		if (i == 0 || i == 2 || i == 4)
		{
			aux->SetColor(1); // Red
		}
		else {
			aux->SetColor(2); // Blue
		}
		balls[index] = new Ball(dim);
		balls[index] = aux;
		index++;
	}

	// line 2
	start = (1 - (dim * 4)) / 2 - 0.5 + dim / 2;
	for (int i = 0; i < 4; i++) {
		Ball *aux = new Ball(dim);
		aux->SetPoint(start + (dim)* i, yMax, startZ + 3 * dim / 4 + 0.01);
		
		if (i == 1 || i == 3)
		{
			aux->SetColor(1); // Red
		}
		else {
			aux->SetColor(2); // Blue
		}
		balls[index] = new Ball(dim);
		balls[index] = aux;
		index++;
	}

	// line 3
	start = (1 - (dim * 3)) / 2 - 0.5 + dim / 2;
	for (int i = 0; i < 3; i++) {
		Ball *aux = new Ball(dim);
		aux->SetPoint(start + (dim)* i, yMax, startZ +( 3 * dim / 4 + 0.01) * 2);
		
		if (i == 0)
		{
			aux->SetColor(1); // Red
		}
		else if (i == 2) {
			aux->SetColor(2); // Blue
		}
		else {
			aux->SetColor(3); // Black
		}
		balls[index] = new Ball(dim);
		balls[index] = aux;
		index++;
	}

	// line 4
	start = (1 - (dim * 2)) / 2 - 0.5 + dim / 2;
	for (int i = 0; i < 2; i++) {
		Ball *aux = new Ball(dim);
		aux->SetPoint(start + (dim)* i, yMax, startZ + (3 * dim / 4 + 0.01)*3);

		if (i == 1)
		{
			aux->SetColor(1); // Red
		}
		else {
			aux->SetColor(2); // Blue
		}
		balls[index] = new Ball(dim);
		balls[index] = aux;
		index++;
	}

	// line 5
	start = (1 - dim) / 2 - 0.5 + dim / 2;
	Ball *aux = new Ball(dim);
	aux->SetPoint(start, yMax, startZ + (3 * dim / 4  + 0.01)*4);
	
	aux->SetColor(2); // Red
	balls[index] = new Ball(dim);
	balls[index] = aux;
	index++;

	// Create corner hole
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(x1, y4,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // A = 0
			VertexFormat(glm::vec3(x1, y4,  z4), glm::vec3(1,1,1), glm::vec3(1,1,1)), // B  = 1
			VertexFormat(glm::vec3(x3, y4,  z3), glm::vec3(1,1,1), glm::vec3(1,1,1)), // C = 2
			VertexFormat(glm::vec3(x5, y4,  z1), glm::vec3(1,1,1), glm::vec3(1,1,1)), // D  = 3
			VertexFormat(glm::vec3(x4, y4,  z3), glm::vec3(1,1,1), glm::vec3(1,1,1)), // E = 4
			VertexFormat(glm::vec3(x2, y4,  z4), glm::vec3(1,1,1), glm::vec3(1,1,1)), // F  = 5
			VertexFormat(glm::vec3(x2, y4,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // G = 6

			VertexFormat(glm::vec3(x1, y1,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // H  = 7
			VertexFormat(glm::vec3(x2, y1,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // I = 8
			VertexFormat(glm::vec3(x6, y2,  z2), glm::vec3(0,0,1), glm::vec3(0,1,1)), // J  = 9
			VertexFormat(glm::vec3(x7, y2,  z2), glm::vec3(1,1,1), glm::vec3(1,1,1)), // K = 10
			VertexFormat(glm::vec3(x5, y3,  z1), glm::vec3(1,1,1), glm::vec3(1,1,1)), // L  = 11

			VertexFormat(glm::vec3(x1, y4,  z1)), // M = 12
			VertexFormat(glm::vec3(x2, y4, z1)), // N  = 13

			VertexFormat(glm::vec3(x8, y1, z6)), // Q = 14
			VertexFormat(glm::vec3(x5, y1, z7)), // P  = 15
			VertexFormat(glm::vec3(-x8, y1, z6)), // O = 16

			VertexFormat(glm::vec3(x1, -y4,  z1)), // R = 12
			VertexFormat(glm::vec3(x2, -y4, z1)), // S  = 13
		};

		vector<unsigned short> indices =
		{
			0,1,7,	7,1,9,
			1,2,9,	2,9,11,
			2,3,11,	3,4,11,
			11,10,4,4,5,10,
			5,6,10, 6,8,5,
			11,9,10,9,10,8,
			9,8,7,	6,8,10,
			// face
			7,8,16, 8,14,15,
			8,15,16, 
			// up
			1,12,2, 3,4,13,
			12,2,3,	13,4,5,
			//
			13,12,17,	13,17,18
		};

		CreateMesh("cube", vertices, indices);
	}
	
	// Create middle hole
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(x1, y4,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // A = 0
			VertexFormat(glm::vec3(x1, y4,  z4), glm::vec3(1,1,1), glm::vec3(1,1,1)), // B  = 1
			VertexFormat(glm::vec3(x3, y4,  z3), glm::vec3(1,1,1), glm::vec3(1,1,1)), // C = 2
			VertexFormat(glm::vec3(x5, y4,  z1), glm::vec3(1,1,1), glm::vec3(1,1,1)), // D  = 3
			VertexFormat(glm::vec3(x4, y4,  z3), glm::vec3(1,1,1), glm::vec3(1,1,1)), // E = 4
			VertexFormat(glm::vec3(x2, y4,  z4), glm::vec3(1,1,1), glm::vec3(1,1,1)), // F  = 5
			VertexFormat(glm::vec3(x2, y4,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // G = 6

			VertexFormat(glm::vec3(x1, y1,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // H  = 7
			VertexFormat(glm::vec3(x2, y1,  z5), glm::vec3(1,1,1), glm::vec3(1,1,1)), // I = 8
			VertexFormat(glm::vec3(x6, y1,  z2), glm::vec3(0,0,1), glm::vec3(0,1,1)), // J  = 9
			VertexFormat(glm::vec3(x7, y1,  z2), glm::vec3(1,1,1), glm::vec3(1,1,1)), // K = 10
			VertexFormat(glm::vec3(x5, y1,  z1), glm::vec3(1,1,1), glm::vec3(1,1,1)), // L  = 11

			VertexFormat(glm::vec3(x1, y4,  z1), glm::vec3(1,1,1)), // M = 12
			VertexFormat(glm::vec3(x2, y4, z1), glm::vec3(1,1,1)), // N  = 13

			VertexFormat(glm::vec3(x8, y1, z6), glm::vec3(1,1,1)), // Q = 14
			VertexFormat(glm::vec3(x5, y1, z7), glm::vec3(1,1,1)), // P  = 15
			VertexFormat(glm::vec3(-x8, y1, z6), glm::vec3(1,1,1)), // O = 16

			VertexFormat(glm::vec3(-0.123, y4, -0.075), glm::vec3(1,1,1)), // R = 17
			VertexFormat(glm::vec3(0.123, y4, -0.075), glm::vec3(1,1,1)), // S = 18

			VertexFormat(glm::vec3(x1, -y4, z1), glm::vec3(1,1,1)), // T = 19
			VertexFormat(glm::vec3(x2,-y4, z1), glm::vec3(1,1,1)), // W  = 20
			VertexFormat(glm::vec3(-0.123, -y4, -0.075), glm::vec3(1,1,1)), // X = 21
			VertexFormat(glm::vec3(0.123,-y4, -0.075), glm::vec3(1,1,1)), // Y = 22
		};

		vector<unsigned short> indices =
		{
			0,1,7,	7,1,9,
			1,2,9,	2,9,11,
			2,3,11,	3,4,11,
			11,10,4,4,5,10,
			5,6,10, 6,8,5,
			11,9,10,9,10,8,
			9,8,7,	6,8,10,
			// face
			7,8,16, 8,14,15,
			8,15,16,
			// up
			1,12,2, 3,4,13,
			12,2,3,	13,4,5,
			//
			12,17,0, 18,6,13,
			// rest
			12,17,21,	21,19,12,
			13,12,19,	13,19,20,
			18,13,20,	18,20,22,
		};

		CreateMesh("cube2", vertices, indices);
	}

	// Load a mesh from file into GPU memory
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Load a mesh from file into GPU memory
	{
		Mesh* mesh = new Mesh("floor");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Load a mesh from file into GPU memory
	{
		Mesh* mesh = new Mesh("ball");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 2, 0);
		materialShininess = 50;
		materialKd = 0.7;
		materialKs = 0.5;
	}
}

Mesh* EightBallPool::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices)
{
	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void EightBallPool::FrameStart()
{

}

void EightBallPool::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->props.resolution;

	// sets the clear color for the color buffer
	glClearColor(0.6, 0.8, 1, 1);

	// clears the color buffer (using the previously set color) and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
	glm::mat4 modelMatrix;

	if (isTopDown) {
		camera->Set(glm::vec3(0, 2.5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
		camera->RotateFirstPerson_OY(M_PI/2);
		projectionMatrix = glm::perspective(RADIANS(50), window->props.aspectRatio, 0.01f, 200.0f);
	}

	{
		// floor
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0, 0.5, 0);
		modelMatrix *= Transform3D::Scale(0.3, 0.2, 0.3);
		RenderMesh(meshes["floor"], shaders["Floor"], modelMatrix, glm::vec3(0, 0.4, 0));

		// mid h1
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(-0.5, 1, 0);
		modelMatrix *= Transform3D::RotateOY(RADIANS(90));
		RenderMesh(meshes["cube"], shaders["White"], modelMatrix, glm::vec3(0.9, 0.9, 0.9));

		// mid h2
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.5, 1, 0);
		modelMatrix *= Transform3D::RotateOY(RADIANS(270));
		RenderMesh(meshes["cube"], shaders["White"], modelMatrix, glm::vec3(1, 1, 1));

		// up left h
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(-0.5 + 0.035, 1, -1 + 0.035);
		modelMatrix *= Transform3D::RotateOY(RADIANS(45));
		RenderMesh(meshes["cube2"], shaders["White"], modelMatrix, glm::vec3(1, 1, 1));

		// up right h
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.5 - 0.035, 1, -1 + 0.035);
		modelMatrix *= Transform3D::RotateOY(RADIANS(-45));
		RenderMesh(meshes["cube2"], shaders["White"], modelMatrix, glm::vec3(1, 1, 1));

		// down left h
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(-0.5 + 0.035, 1, 1 - 0.035);
		modelMatrix *= Transform3D::RotateOY(RADIANS(135));
		RenderMesh(meshes["cube2"], shaders["White"], modelMatrix, glm::vec3(1, 1, 1));

		// down right h
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.5 - 0.035, 1, 1 - 0.035);
		modelMatrix *= Transform3D::RotateOY(RADIANS(-135));
		RenderMesh(meshes["cube2"], shaders["White"], modelMatrix, glm::vec3(1, 1, 1));

		// Table
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0, 1, 0);
		modelMatrix *= Transform3D::Scale(1, 0.01, 2);
		RenderMesh(meshes["box"], shaders["Red"], modelMatrix, glm::vec3(0.5, 0, 0));

		float xM = 0.1, yM = 0.1, zM = 0.88;

		// Manta laterala 1
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(-0.5 - (xM / 2), 1, 0.49);
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		// Manta laterala 2
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(-0.5 - (xM / 2), 1, -0.49);
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		// Manta laterala 3
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.5 + (xM / 2), 1, 0.49);
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		// Manta laterala 4
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.5 + (xM / 2), 1, -0.49);
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		// Manta jos 
		xM = 0.86; yM = 0.1; zM = 0.1;
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0, 1, 1 + (zM / 2));
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		// Manta sus
		xM = 0.86; yM = 0.1; zM = 0.1;
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0, 1, -1 - (zM / 2));
		modelMatrix *= Transform3D::Scale(xM, yM, zM);
		RenderMesh(meshes["box"], shaders["Brown"], modelMatrix, glm::vec3(0.1, 0.1, 0.1));

		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0, 0.75, 0);
		modelMatrix *= Transform3D::Scale(0.8, 0.5, 1.8);
		RenderMesh(meshes["box"], shaders["White"], modelMatrix, glm::vec3(0, 0, 0));
	}

	// ============= power ========
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y, camera->GetTargetPosition().z);
	
	modelMatrix *= Transform3D::RotateOY(alpha);
	modelMatrix *= Transform3D::Translate(-0.4, 0.3, 0);
	modelMatrix *= Transform3D::Scale(0.1 * sin(time) * 2, 0.05, 0.04);
	if (cueMov == 1)
		RenderMesh(meshes["box"], shaders["Power"], modelMatrix, glm::vec3(0.6, 0.298, 0));
	

	float dim = 0.065;
	float yMax = 1 + dim / 2;

	// ============================ cue =========================================
	float dimX = 0.01, dimY = 0.01, dimZ = 1;

	// coord for trans cue in ball
	float cueX1 = ballX + whiteBall->radius * sin(alpha) / 2;
	float cueY = ballY  + 0.09;
	float cueZ1 = ballZ + whiteBall->radius * cos(alpha) / 2;

	// coord for trans cue 
	float cueX2 = sin(alpha) *  dimZ / 2, cueZ2 = cos(alpha) * dimZ / 2;
	float cueZ3 = cos(-10) * (0.04), cueX3 = sin(-15) * (0.04);

	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(cueX2, 0, cueZ2);
	modelMatrix *= Transform3D::Translate(cueX1, cueY, cueZ1);
	modelMatrix *= Transform3D::RotateOY(alpha);
	modelMatrix *= Transform3D::RotateOX(RADIANS(-10));
	modelMatrix *= Transform3D::Scale(dimX, dimY, dimZ);
	if (!whiteBall->moving && !whiteBall->isOut) {
		if (cueMov == 1)
		{
			RenderMesh(meshes["box"], shaders["MovingCue"], modelMatrix, glm::vec3(0.6, 0.298, 0));
		}
		else {
			RenderMesh(meshes["box"], shaders["Cue"], modelMatrix, glm::vec3(0.6, 0.298, 0));
		}
	}
	// ============================================= White ball ==========================================
	
	if (whiteBall->moving && !whiteBall->isOut) {
		isTopDown = true;
		hitBall(deltaTimeSeconds, whiteBall);
	}
	else {
		whiteBall->angle = alpha;
		whiteBall->speed = speedBall;
	}
	
	takeOut(whiteBall);
	whiteBall->SetPoint(ballX, ballY, ballZ);
	if (!isTopDown) {
		camera->Set(glm::vec3(ballX, ballY, ballZ) + glm::vec3(0, 0.3, 0.5), glm::vec3(ballX, ballY, ballZ), glm::vec3(0, 1, 0));
		camera->RotateThirdPerson_OY(alpha);
	}
	
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(ballX, ballY, ballZ);
	modelMatrix *= Transform3D::Scale(dim, dim, dim);
	if (!whiteBall->isOut)
		RenderMesh(meshes["ball"], shaders["White"], modelMatrix, glm::vec3(1,1,1));
	whiteBall->SetPoint(ballX, ballY, ballZ);
	
	// fault
	if (whiteBall->isOut) {
		ballX = -0.5 + dim / 2 + ((1 - dim) / 2);
		ballY = 1 + dim / 2, ballZ = 0.7;
		whiteBall->SetPoint(ballX, ballY, ballZ);
		whiteBall->isOut = false;
		whiteBall->moving = false;
		if (player1->playing) {
			player1->isFault = true;
			player1->playingNext = false;
		}
		else {
			player2->isFault = true;
			player2->playingNext = false;
		}
	}
	// ========================== Balls ==============================
	for (int i = 0; i < 15; i++)
	{
		// if the ball is draw or not
		if (!balls[i]->isOut) {
			if (isCollision(balls[i], whiteBall) && firstHit) {
				if (player1->playing) {
					if(player1->color == balls[i]->color || player1->color == 0) 
						player1->hitMinOneBall = true;
				}
				else {
					if (player2->color == balls[i]->color || player2->color == 0)
						player2->hitMinOneBall = true;
				}
				firstHit = false;
			}
			collision(balls[i], whiteBall, deltaTimeSeconds);

			for (int j = 0; j < 15; j++) {
				if (i != j && !balls[j]->isOut) {
					collision(balls[i], balls[j], deltaTimeSeconds);
				}
			}
			
			if (balls[i]->moving) {
				hitBall(deltaTimeSeconds, balls[i]);
			}

			takeOut(balls[i]);

			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate(balls[i]->center.x, balls[i]->center.y, balls[i]->center.z);
			modelMatrix *= Transform3D::Scale(dim, dim, dim);

			switch (balls[i]->color)
			{
			case 0:
				RenderMesh(meshes["ball"], shaders["White"], modelMatrix, glm::vec3(1,1,1));
			case 1:
				RenderMesh(meshes["ball"], shaders["Red"], modelMatrix, glm::vec3(1, 0, 0));
			case 2:
				RenderMesh(meshes["ball"], shaders["Blue"], modelMatrix, glm::vec3(0, 0, 1));
			case 3:
				RenderMesh(meshes["ball"], shaders["Brown"], modelMatrix, glm::vec3(0, 0, 0));
			default:
				break;
			}
		}
		
	}

	// PRINT the statistic 
	if (allBallsStoped() && whiteBall->printStat) {
		whiteBall->printStat = false;
		cout << "===========================\nPlaying ";
		if (player1->playing) {
			player1->PrintName();
			if (!player1->hitMinOneBall) {
				player1->isFault = true;
				player1->playingNext = false;
			}
			if (player1->color != 0 && player2->color == 0) {
				switch (player1->color)
				{
				case 1:
					player2->color = 2;
					break;
				case 2:
					player2->color = 1;
				default:
					break;
				}
			}

			if (player1->isFault) {
				player1->faults++;
				player1->isFault = false;
				setWhiteBallPosition = true;
			}
		}
		else {
			player2->PrintName();
			if (!player2->hitMinOneBall) {
				player2->isFault = true;
				player2->playingNext = false;
			}
			if (player2->color != 0 && player1->color == 0) {
				switch (player2->color)
				{
				case 1:
					player1->color = 2;
					break;
				case 2:
					player1->color = 1;
				default:
					break;
				}
			}
			if (player2->isFault) {
				player2->faults++;
				player2->isFault = false;
				setWhiteBallPosition = true;
			}
		}

		player1->Print();
		player1->PrintColor();

		player2->Print();
		player2->PrintColor();

		if (player1->eightBall || player2->eightBall) {
			cout << "WINNER: ";
			if (player1->eightBall) {
				if (player1->win) {
					player1->PrintName();
				}
				else {
					player2->PrintName();
				}
			}
			else {
				if (player2->win) {
					player2->PrintName();
				}
				else {
					player1->PrintName();
				}
			}
			cout << "!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!\n";
			cout << "!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!\n";
			cout << "!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!~!~~!~~~!~~!\n";
			return;
		}
		// NextMove
		cout << "Next move for ";
		if (player1->playing) {
			if (player1->playingNext && player1->pocketABall) {
				player1->PrintName();
				cout << "\n";
			}
			else {
				player2->PrintName();
				player1->playing = false;
				player2->playing = true;
				cout << "\n";
			}
		}
		else {
			if (player2->playingNext && player2->pocketABall) {
				player2->PrintName();
				cout << "\n";
			}
			else {
				player1->PrintName();
				player2->playing = false;
				player1->playing = true;
				cout << "\n";
			}
		}
				
		cout << "===========================";
		player1->hitMinOneBall = false;
		player2->hitMinOneBall = false;
		player1->pocketABall = false;
		player2->pocketABall = false;
		firstHit = true;
	}
}

void EightBallPool::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix, glm::vec3 color)
{
	if (!mesh || !shader || !shader->program)
		return;
	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	GLint loc_lightPosition = glGetUniformLocation(shader->program, "light_position");
	glUniform3fv(loc_lightPosition, 1, glm::value_ptr(lightPosition));

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = camera->position;
	GLint loc_eyePosition = glGetUniformLocation(shader->program, "eye_position");
	glUniform3fv(loc_eyePosition, 1, glm::value_ptr(eyePosition));


	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	GLint loc_materialShininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(loc_materialShininess, materialShininess);

	GLint loc_materialKd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(loc_materialKd, materialKd);

	GLint loc_materialKs = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(loc_materialKs, materialKs);

	glm::vec3 objectColor = color;
	GLint loc_objectColor = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(loc_objectColor, 1, glm::value_ptr(objectColor));

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	time = Engine::GetElapsedTime();
	int location = glGetUniformLocation(shader->GetProgramID(), "Time");
	glUniform1f(location, time);

	location = glGetUniformLocation(shader->GetProgramID(), "Alpha");
	glUniform1f(location, alpha);
	mesh->Render();
}

void EightBallPool::FrameEnd()
{
	//DrawCoordinatSystem();
}

void EightBallPool::OnInputUpdate(float deltaTime, int mods)
{
	float dim = whiteBall->radius;
	// White Ball
	if (setWhiteBallPosition == true)
	{
		whiteBall->center.x -= deltaTime;
		if (window->KeyHold(GLFW_KEY_W) && ballX - deltaTime > -0.5 + dim / 2 
			&& !ballsAndWhiteBallCollosion()) {
			ballX -= deltaTime;
		}
		else {
			whiteBall->center.x += deltaTime;
		}

		whiteBall->center.x += deltaTime;
		if (window->KeyHold(GLFW_KEY_S) && ballX + deltaTime < 0.5 - dim / 2 
			&& !ballsAndWhiteBallCollosion()) {
			ballX += deltaTime;
		}
		else {
			whiteBall->center.x -= deltaTime;
		}

		whiteBall->center.z -= deltaTime;
		if (isStart) {
			if (window->KeyHold(GLFW_KEY_D) && ballZ - deltaTime > 0.7 
				&& !ballsAndWhiteBallCollosion()) {
				ballZ -= deltaTime;
			}
			else {
				whiteBall->center.z += deltaTime;
			}
		} else if (window->KeyHold(GLFW_KEY_D) && ballZ + deltaTime > -1 + dim / 2 + 0.05 
			&& !ballsAndWhiteBallCollosion()) {
			ballZ -= deltaTime;
		}
		else {
			whiteBall->center.z += deltaTime;
		}

		whiteBall->center.z += deltaTime;
		if (window->KeyHold(GLFW_KEY_A) && ballZ + deltaTime < 1 - dim / 2 
			&& !ballsAndWhiteBallCollosion()) {
			ballZ += deltaTime;
		}
		else {
			whiteBall->center.z -= deltaTime;
		}
	}
	whiteBall->SetPoint(ballX, ballY, ballZ);
};

void EightBallPool::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_SPACE) {
		if (isTopDown)
		{
			camera->Set(glm::vec3(0, 1.3, 1.5), glm::vec3(ballX, ballY, ballZ), glm::vec3(0, 1, 0));
			projectionMatrix = glm::perspective(RADIANS(80), window->props.aspectRatio, 0.01f, 200.0f);
			isTopDown = false;
		}
		setWhiteBallPosition = false;
		isStart = false;
	}

};

void EightBallPool::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void EightBallPool::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	float sensivityOX = 0.001f;
	float sensivityOY = 0.001f;

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		alpha += sensivityOX * (-deltaX);
		whiteBall->angle = alpha;
		if (window->GetSpecialKeyState() == 0) {
			camera->RotateThirdPerson_OY(sensivityOX * (-deltaX));
		}
	}
};

void EightBallPool::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
	{
		cueMov = 1;
	}
};

void EightBallPool::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
	{
		cueMov = 0;
		whiteBall->moving = true;
		whiteBall->speed = speedBall * abs(sin(time));
	}
}

void EightBallPool::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void EightBallPool::OnWindowResize(int width, int height)
{
	// treat window resize event
}
