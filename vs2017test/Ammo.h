#pragma once
#include"Cell.h"

const int MAX_BULLETS_IN_STOCK = 100;
const int MAX_GRENADES_IN_STOCK = 10;

class Ammo
{
private:
	int x;
	int y;
	int bullets;
	int grenades;


public:
	Ammo();
	Ammo(int xx,int yy);
	~Ammo();
	void setX(int xx) { x = xx; }
	void setY(int yy) { y = yy; }
	int getX() { return x; }
	int getY() { return y; }
	void setBullets(int b) { bullets=b; }
	void setGrenades(int g) { grenades=g; }
	int getBullets() { return bullets; }
	int getGrenades() { return grenades; }

};




