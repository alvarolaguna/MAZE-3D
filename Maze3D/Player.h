#include <iostream>

class Player {
private:
	float row, col;
	int direction;
	enum directions { north, east, south, west };

public:
	Player();
	void move();
	void turnLeft();
	void turnRight();
	int * nextMove();

};

