#pragma once
#include "Definitions.h"

class Cell
{
private:
	int row, col;
	Cell* parent;
	int target_row, target_col;
	double h, g, f;
	double gColor;

public:
	Cell();
	Cell(int r,int c,Cell* p);
	Cell(int r, int c, Cell* p, double g, double h);
	Cell(const Cell& other);
	Cell(int r, int c, int tr, int tc, double g, Cell * p);
	~Cell();
	int getRow() { return row; }
	int getCol() { return col; }
	int getTargetRow() { return target_row; }
	int getTargetCol() { return target_col; }
	double getF() { return f; }
	double getG() { return g; }
	double getH() { return h; }
	bool operator == (const Cell &other) { return other.col == col && other.row == row; }
	void setG(double newG) { g = newG; }
	void setF(double newF) { f = newF; }
	void setH(double newH) { h = newH; }

	double getGColor() { return gColor; }
	void setGColor(double newG) { gColor = newG; }


	Cell* getParent() { return parent; }

};

