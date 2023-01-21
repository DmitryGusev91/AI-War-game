#include "Ammo.h"

Ammo::Ammo()
{
	x = 0;
	y = 0;
	bullets = 0;
	grenades = 0;
	
}

Ammo::Ammo(int xx, int yy)
{
	x = xx;
	y = yy;
	bullets = MAX_BULLETS_IN_STOCK;
	grenades = MAX_GRENADES_IN_STOCK;
}

Ammo::~Ammo()
{
}
