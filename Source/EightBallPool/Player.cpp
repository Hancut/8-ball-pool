#include "Player.h"

#include <include/glm.h>
#include <Core/Engine.h>
#include <math.h>
#include <iostream>
#include "Ball.h"

using namespace std;


Player::Player(){
	this->color = 0;
	this->faults = 0;
	this->pocketBalls = 0;
	this->playingNext = false;
	this->isFault = false;
	this->pocketABall = false;
	this->win = false;
	this->eightBall = false;
}

Player::~Player() {

}

void Player::Print() {
	cout << "Player" << this->number << " has:\n\t* " << this->faults << " faults\n\t* " << this->pocketBalls << " pocketed balls\n";
}

void Player::PrintName() {
	cout << "Player" << this->number << " \n";
}

void Player::PrintColor() {
	switch (this->color)
	{
	case 0:
		cout << "\t*Color: None\n";
		return;
	case 1:
		cout << "\t*Color: Red\n";
		return;
	case 2:
		cout << "\t*Color: Blue\n";
		return;
	default:
		break;
	}
}

void Player::SetPoints(Ball* ball) {
	if (this->playing) {
		if (this->color == 0) {
			this->pocketBalls++;
			this->pocketABall = true;
			this->playingNext = true;
			this->color = ball->color;
			return;
		}
		
		if (ball->color == 3) {
			this->eightBall = true;

			if (this->pocketBalls == 7) {
				this->win = true;
			}
		}

		if (this->color == ball->color) {
			this->pocketBalls++;
			this->pocketABall = true;
			this->playingNext = true;
			return;
		}
		else if (this->color != ball->color) {
			this->isFault = true;
			this->playingNext = false;
			return;
		}
	}
	
}

void Player::firstHit(Ball* ball) {
	if (ball->color == 3) {
		this->playingNext = false;
		return;
	}

	if (this->playing && this->color != ball->color) {
		this->isFault = true;
		this->playingNext = false;
	}
}
