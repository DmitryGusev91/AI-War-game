#include "Health.h"

Health::Health()
{
	x = 0;
	y = 0;
	health = 0;
}

Health::Health(int xx, int yy)
{
	x = xx;
	y = yy;
	health = MAX_HEALTH_IN_STOCK;
}

Health::~Health()
{
}
