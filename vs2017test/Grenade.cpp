#include "Grenade.h"



Grenade::Grenade(double x, double y)
{
	int i;
	
	double angle,teta = 2*3.14/NUM_BULLETS;
	for (i = 0, angle = 0; i < NUM_BULLETS; i++,angle += teta) 
	{
		bullets[i] = new Bullet(x, y,angle);
	}

	isExploded = false;
	isMoving = false;
}



Grenade::~Grenade()
{
}

void Grenade::setIsExploded(bool state)
{
	isExploded = state;
	for (int i = 0; i < NUM_BULLETS; i++)
		if(bullets[i]!=nullptr)
			bullets[i]->setIsFired(state);

}

void Grenade::explode(int maze[MSZ][MSZ])
{

	for (int i = 0; i < NUM_BULLETS; i++)
	{
		if (bullets[i] != nullptr)
		{
			bullets[i]->move(maze);
			if (bullets[i]->getIsFired() == false)
			bullets[i] = nullptr;
		}

	}
}

void Grenade::show()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		if(bullets[i]!=nullptr)
			bullets[i]->show();

}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage)
{
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->setIsFired(true);
		bullets[i]->SimulateFire( maze,  security_map,  damage);
	}
}

