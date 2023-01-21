#pragma once

const int MAX_HEALTH_IN_STOCK=20;
class Health
{
private:
	int x;
	int y;
	int health;

public:
	Health();
	Health(int xx,int yy);
	~Health();
	void setX(int xx) { x = xx; }
	void setY(int yy) { y = yy; }
	int getX() { return x; }
	int getY() { return y; }
	void setHealth(int h) { health=h; }
	int getHealth() { return health; }
};

