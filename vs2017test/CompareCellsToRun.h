#pragma once
#include "Cell.h"

class CompareCellsToRun
{
public:
	CompareCellsToRun();
	~CompareCellsToRun();
	bool operator()(Cell& c1, Cell& c2) {
		return c1.getF() < c2.getF();
	}
};

