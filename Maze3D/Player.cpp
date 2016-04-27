#include "Player.h"

using namespace std;

Player::Player()
{
	row = 0;
	col = 1;
	direction = north;
}

void Player::move()
{
	cout << "direction " << direction << endl;
	switch (direction)
	{
	case north:
		row += 1;
		break;
	case east:
		col -= 1;
		break;
	case south:
		row -= 1;
		break;
	case west:
		col += 1;
		break;
	default:
		break;
	}
	cout << "row " << row << "col " << col << endl;
}

void Player::turnLeft()
{
	if(direction == 0){
		direction = 3;
	}
	else {
		direction -= 1;
	}
}

void Player::turnRight()
{
	direction += 1;
	direction %= 4;

}

int * Player::nextMove()
{
	int pos[2];
	switch (direction)
	{
	case north:
		pos[0] = row + 1;
		pos[1] = col;
		return pos;
	case east:
		pos[0] = row;
		pos[1] = col - 1;
		return pos;
	case south:
		pos[0] = row - 1;
		pos[1] = col;
		return pos;
	case west:
		pos[0] = row;
		pos[1] = col + 1;
		return pos;
	}

}
