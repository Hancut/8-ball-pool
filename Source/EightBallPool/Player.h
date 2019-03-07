#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include "Ball.h"

using namespace std;

class Player
{
public:

	Player::Player();
	Player::~Player();
	void Player::Print();
	void Player::PrintName();
	void Player::SetPoints(Ball* ball);
	void Player::PrintColor();
	void Player::firstHit(Ball* ball);

	int number;
	int faults;
	bool isFault;
	int pocketBalls;
	bool pocketABall;
	bool win;
	bool eightBall;
	int color;
	bool playing;
	bool playingNext;
	bool hitMinOneBall;
};

