#pragma once

#include "Bullet.h"

const int NUM_BULLETS = 16;

class Grenade
{
private:
	Bullet* bullets[NUM_BULLETS];
	double x, y;
	bool isExploded,isMoving;
public:
	Grenade(double x, double y);
	~Grenade();
	int getNumOfShards() { return NUM_BULLETS; }
	void setIsExploded(bool state);
	bool getIsExploded() { return isExploded; }
	void explode(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ],double security_map[MSZ][MSZ],double damage);
	bool getIsMoving() { return isMoving; }
	void setIsMoving(bool status) { isMoving = status; }
	Bullet** getBullets() { return bullets; }
};

