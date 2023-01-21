
#include "Technician.h"
#include <iostream>

using namespace std;

Technician::~Technician()
{
}

Technician::Technician()
{
	ammo = MAX_AMMO_CAPACITY;
	hp = MAX_HP_CAPACITY;;
	color = 0;
	foundSoldier = false;
}

Technician::Technician(int color)
{
	ammo = MAX_AMMO_CAPACITY;
	hp = MAX_HP_CAPACITY;
	this->color = color;
	foundSoldier = false;
}

void Technician::RestorePath(Cell* pCurrent, int maze[MSZ][MSZ])
{
	int techColor;
	if (color == TEAM1)
		techColor = TECH1;
	else
		techColor = TECH2;

	if (pCurrent->getParent() != nullptr)
	{
		if (maze[pCurrent->getParent()->getRow()][pCurrent->getParent()->getCol()] == techColor)
		{
			foundSoldier = true;
			return;
		}		
		while (pCurrent->getParent()->getParent() != nullptr)
		{
			pCurrent = pCurrent->getParent();
		}
	}
	Cell* pc = new Cell(pCurrent->getRow(), pCurrent->getCol(), nullptr);
	maze[pCurrent->getRow()][pCurrent->getCol()] = techColor;
	maze[pCurrent->getParent()->getRow()][pCurrent->getParent()->getCol()] = SPACE;
	setCurrentCell(pc);
}

void Technician::CheckNeighbor(Cell* pCurrent, int nrow, int ncol, priority_queue<Cell, vector<Cell>, CompareCells>& pq, vector<Cell>& grays, vector<Cell>& blacks, int maze[MSZ][MSZ])
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol , pCurrent);
	pn->setG(pCurrent->getG()+1);
	pn->setH(ManhattanDistance(c->getRow(),c->getCol(),target->getRow(),target->getCol()));
	pn->setF(pn->getG()+pn->getH());
	// check the color of this neighbor
	it_black = find(blacks.begin(), blacks.end(), *pn);
	if (it_black != blacks.end()) // it was found i.e. it is black
		return;
	// white
	it_gray = find(grays.begin(), grays.end(), *pn);
	if (it_gray == grays.end()) // it wasn't found => it is white
	{
		grays.push_back(*pn); // paint it gray
		pq.push(*pn);
	}
	else // it is gray
	{
		//   new F       ?      old F
		if (pn->getF() < it_gray->getF()) // then update it (F of neighbor)!!!
		{
			// we need toupdate it in two places:
			// 1. in vector grays
			it_gray->setG(pn->getG());
			it_gray->setF(pn->getF());
			// 2. in PQ
			// to update a Cell in pq we need to remove it from pq, to update it and to push it back
			vector<Cell> tmp;
			Cell tmpCell;
			do
			{
				if (pq.empty())
				{
					exit(1);
				}
				tmpCell = pq.top();
				pq.pop();
				if (!(tmpCell == (*pn))) // do not copy the Cell we were looking for to tmp!(see line 173) 
					tmp.push_back(tmpCell);
			} while (!(tmpCell == (*pn)));
			// now we are aout of do-while because we have found the neighbor in PQ. So change it to *pn.
			pq.push(*pn);
			// now push back all the elements that are in tmp
			while (!tmp.empty())
			{
				pq.push(tmp.back());
				tmp.pop_back();
			}
		}
	}
}

double Technician::ManhattanDistance(int x1, int y1, int x2, int y2)
{
		return (abs(x1 - x2) + abs(y1 - y2));
}

void Technician::restoreHealth(Soldier* s)
{
	int needed = MAX_HP - s->getHP();
	if (this->getHP() < needed)
	{
		s->setHP(s->getHP() + this->getHP());
		this->setHP(0);
	}
	else
	{
		s->setHP(s->getHP() + needed);
		this->setHP(this->getHP()-needed);
	}
}

void Technician::restoreAmmo(Soldier* s)
{
	int needed = MAX_AMMO - s->getAmmo();
	if (this->getAmmo() < needed)
	{
		s->setAmmo(s->getAmmo() + this->getAmmo());
		this->setAmmo(0);
	}
	else
	{
		s->setAmmo(s->getAmmo() + needed);
		this->setAmmo(this->getAmmo() - needed);
	}
}


void Technician::search(int maze[MSZ][MSZ], int color)
{
	int row, col;
	vector<Cell> grays;
	vector<Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCells> pq;
	Cell* pstart = new Cell(c->getRow(), c->getCol(), nullptr);

	Cell* pCurrent;
	bool targetFound = false;
	// initializes grays and pq
	grays.push_back(*pstart);
	pq.push(*pstart);
	vector<Cell>::iterator it_gray;


	while (!pq.empty())
	{
		pCurrent = new Cell(pq.top());
		pq.pop();
		// If current is actually a target then we stop A*
		if (pCurrent->getRow() == target->getRow() &&
			pCurrent->getCol() == target->getCol()) // then it is target
		{ //in this case there cannot be a better path to target!!!
			RestorePath(pCurrent,maze);
			return;
		}
		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent); // we have to define operator ==
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays
		// now check the neighbors of current
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		// try to go UP (row -1)
		if (maze[row - 1][col] != WALL ) // we can go UP
			CheckNeighbor(pCurrent, row - 1, col, pq, grays, blacks,maze);

		if (maze[row + 1][col] != WALL ) // DOWN
			CheckNeighbor(pCurrent, row + 1, col, pq, grays, blacks,maze);

		if (maze[row][col + 1] != WALL ) // RIGHT
			CheckNeighbor(pCurrent, row, col + 1, pq, grays, blacks,maze);

		if (maze[row][col - 1] != WALL )//LEFT
			CheckNeighbor(pCurrent, row, col - 1, pq, grays, blacks,maze);

	}
}


