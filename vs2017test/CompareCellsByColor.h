#pragma once
#include "Cell.h"

class CompareCellsByColor
{
public:
	CompareCellsByColor();
	~CompareCellsByColor();
	bool operator()( Cell &c1, Cell &c2) {
		return c1.getGColor() > c2.getGColor();
	}
};

