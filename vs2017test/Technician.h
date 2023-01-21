#pragma once
#include "Cell.h"
#include "Definitions.h"
#include "math.h"
#include <queue>
#include <vector>
#include "CompareCells.h"
#include <iostream>
#include "Soldier.h"

using namespace std;

const int MAX_HP_CAPACITY = 20;
const int MAX_AMMO_CAPACITY = 20;


class Technician
{
private:
	int ammo;
	int hp;
	int color;
	int roomNum;
	bool foundSoldier;

	Cell* c = nullptr;
	Cell* target = nullptr;


public:
	~Technician();
	Technician();
	Technician(int color);

	int getAmmo() { return ammo; }
	int getHP() { return hp; }
	void setAmmo(int a) { ammo = a; }
	void setHP(int health) { hp = health; }
	int getColor() { return color; }
	void setColor(int col) { color = col; }
	int getRoom() { return roomNum; }
	void setRoom(int r) { roomNum = r; }
	bool getFoundSoldier() { return foundSoldier; }
	void setFountSoldier(bool status) { foundSoldier = status; }


	void setCurrentCell(Cell* cell) { c = cell; }
	void setCurrentTarget(Cell* cell) { target = cell; }
	Cell* getCurrentCell() { return c; }
	Cell* getTarget() { return target; }


	void search(int maze[MSZ][MSZ], int color);
	void RestorePath(Cell* pCurrent, int maze[MSZ][MSZ]);
	void CheckNeighbor(Cell* pCurrent, int nrow, int ncol, priority_queue <Cell, vector<Cell>, CompareCells>& pq, vector <Cell>& grays, vector <Cell>& blacks, int maze[MSZ][MSZ]);
	double ManhattanDistance(int x1, int y1, int x2, int y2);

	void restoreHealth(Soldier* s);
	void restoreAmmo(Soldier* s);
};