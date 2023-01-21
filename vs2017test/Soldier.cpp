#include "Soldier.h"
#include <iostream>

Soldier::Soldier()
{
	x = 0;
	y = 0;
	ammo = MAX_AMMO;
	grenades = MAX_GRENADES;
	healthPoints = MAX_HP;
	color = SPACE;
	isInSameRoom = false;
	isHidden = false;
}

Soldier::Soldier(int xx, int yy,int c)
{
	x = xx;
	y = yy;
	ammo = MAX_AMMO;
	grenades = MAX_GRENADES;
	healthPoints = MAX_HP;
	color = c;
	isInSameRoom = false;
	isHidden = false;
}

Soldier::~Soldier()
{
}



void Soldier::deleteSoldier()
{
	c=nullptr;
	target=nullptr;
	grenade = nullptr;
	bullet = nullptr;
}

int Soldier::AuclidicDistance(Cell* c)
{
	return sqrt(pow((this->getY()-c->getRow()),2)+pow((this->getX()-c->getCol()),2));
}

//pick a target for soldier
void Soldier::SearchForEnemy(Soldier* enemy1, Soldier* enemy2)
{
	if (enemy1 == nullptr && enemy2 == nullptr)
		cout << "YOUR TEAM HAS WON THE GAME !!!!" << endl;
	else
	{
		if (enemy1 == nullptr)
			this->setCurrentTarget(enemy2->getCurrentCell());
		else
		{
			if (enemy2 == nullptr)
				this->setCurrentTarget(enemy1->getCurrentCell());
			else
			{
				if (enemy1->getRoomNum() == enemy2->getRoomNum() && enemy1->getRoomNum() == this->getRoomNum())	//if both enemies are in the same room with the soldier then target the one with less hp
				{
					if (enemy1->getHP() > enemy2->getHP())
						this->setCurrentTarget(enemy2->getCurrentCell());
					else
						this->setCurrentTarget(enemy1->getCurrentCell());
				}
				else
				{
					if (AuclidicDistance(enemy1->getCurrentCell()) >= AuclidicDistance(enemy2->getCurrentCell()))//else (both not in the same room with soldier) pick the nearest one
						this->setCurrentTarget(enemy1->getCurrentCell());
					else
						this->setCurrentTarget(enemy2->getCurrentCell());
				}
			}
		}
	}
}


Soldier* Soldier::findEnemyByTarget(Soldier* e1, Soldier* e2)
{
	if (e1 == nullptr)
		return e2;
	else if (e2 == nullptr)
		return e1;
	else
	{
		if (e1->getCurrentCell()->getRow() == this->getTarget()->getRow() && e1->getCurrentCell()->getCol() == this->getTarget()->getCol())
			return e1;
		else
			return e2;
	}
}

double Soldier::ManhattanDistance(int x1, int y1, int x2, int y2)
{
	return (abs(x1 - x2) + abs(y1 - y2));
}

void Soldier::RestorePath(Cell* pCurrent, int maze[MSZ][MSZ])
{
	if (pCurrent->getParent() != nullptr)
	{
		if (maze[pCurrent->getParent()->getRow()][pCurrent->getParent()->getCol()] == color)
		{
			return;
		}
		while (pCurrent->getParent()->getParent() != nullptr)
		{
			pCurrent = pCurrent->getParent();
		}
	}
	Cell* pc = new Cell(pCurrent->getRow(), pCurrent->getCol(), nullptr);
	maze[pCurrent->getRow()][pCurrent->getCol()] = color;
	maze[pCurrent->getParent()->getRow()][pCurrent->getParent()->getCol()] = SPACE;
	setCurrentCell(pc);
}

void Soldier::CheckNeighbor(Cell* pCurrent, int nrow, int ncol, priority_queue<Cell, vector<Cell>, CompareCellsToRun>& pq1, vector<Cell>& grays, vector<Cell>& blacks, int maze[MSZ][MSZ])
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol, pCurrent);
	pn->setG(pCurrent->getG() + 1);
	pn->setH(ManhattanDistance(c->getRow(), c->getCol(), target->getRow(), target->getCol()));
	pn->setF(pn->getG() + pn->getH());
	// check the color of this neighbor
	it_black = find(blacks.begin(), blacks.end(), *pn);
	if (it_black != blacks.end()) // it was found i.e. it is black
		return;
	// white
	it_gray = find(grays.begin(), grays.end(), *pn);
	if (it_gray == grays.end()) // it wasn't found => it is white
	{
		grays.push_back(*pn); // paint it gray
		pq1.push(*pn);
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
				if (pq1.empty())
				{
					exit(1);
				}
				tmpCell = pq1.top();
				pq1.pop();
				if (!(tmpCell == (*pn))) // do not copy the Cell we were looking for to tmp!(see line 173) 
					tmp.push_back(tmpCell);
			} while (!(tmpCell == (*pn)));
			// now we are aout of do-while because we have found the neighbor in PQ. So change it to *pn.
			pq1.push(*pn);
			// now push back all the elements that are in tmp
			while (!tmp.empty())
			{
				pq1.push(tmp.back());
				tmp.pop_back();
			}
		}
	}
}


void Soldier::searchWhereToRun(int maze[MSZ][MSZ], int color)
{
	int row, col;
	vector<Cell> grays;
	vector<Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCellsToRun> pq1;
	Cell* pstart = new Cell(c->getRow(), c->getCol(), nullptr);

	Cell* pCurrent;
	bool targetFound = false;
	// initializes grays and pq
	grays.push_back(*pstart);
	pq1.push(*pstart);
	vector<Cell>::iterator it_gray;


	while (!pq1.empty())
	{
		pCurrent = new Cell(pq1.top());
		pq1.pop();
		// If current is actually a target then we stop A*
		if (pCurrent->getRow() == target->getRow() &&
			pCurrent->getCol() == target->getCol()) // then it is target
		{ //in this case there cannot be a better path to target!!!
			RestorePath(pCurrent, maze);
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
		if (maze[row - 1][col] != WALL) // we can go UP
			CheckNeighbor(pCurrent, row - 1, col, pq1, grays, blacks, maze);

		if (maze[row + 1][col] != WALL) // DOWN
			CheckNeighbor(pCurrent, row + 1, col, pq1, grays, blacks, maze);

		if (maze[row][col + 1] != WALL) // RIGHT
			CheckNeighbor(pCurrent, row, col + 1, pq1, grays, blacks, maze);

		if (maze[row][col - 1] != WALL)//LEFT
			CheckNeighbor(pCurrent, row, col - 1, pq1, grays, blacks, maze);

	}
}




//checks if the soldier is in the same room as his target
void Soldier::isInSameRoomWithTarget(Room** rooms)
{
	int leftW, rightW, topH, lowH;
	leftW = rooms[roomNum]->getCenterCol() - rooms[roomNum]->getW() / 2;
	rightW = rooms[roomNum]->getCenterCol() + rooms[roomNum]->getW() / 2;
	topH = rooms[roomNum]->getCenterRow() + rooms[roomNum]->getH() / 2;
	lowH = rooms[roomNum]->getCenterRow() - rooms[roomNum]->getH() / 2;

	if (target->getRow() > lowH && target->getRow() < topH && target->getCol() > leftW && target->getCol() < rightW)
		isInSameRoom = true;
}

//check if the soldier is in one of the rooms and update the room num, if the soldier is in a corridor then update room to -1
void Soldier::updateSoldiersRoom(Room** rooms)
{
	int leftW, rightW, topH, lowH;
	setRoomNum(-1);
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		leftW = rooms[i]->getCenterCol() - rooms[i]->getW() / 2;
		rightW = rooms[i]->getCenterCol() + rooms[i]->getW() / 2;
		topH = rooms[i]->getCenterRow() + rooms[i]->getH() / 2;
		lowH = rooms[i]->getCenterRow() - rooms[i]->getH() / 2;
		if (c->getRow() >= lowH && c->getRow() <= topH && c->getCol() >= leftW && c->getCol() <= rightW)
		{
			setRoomNum(i);
			break;
		}

	}

}

void Soldier::shoot(Room** rooms, int maze[MSZ][MSZ],Soldier* enemy)
{
	if (bullet != nullptr)
		cout << bullet->getX() << "	" << bullet->gety() << endl;

	if (healthPoints <= 0)
		return;

	if (bullet == nullptr) // Initialize bullet
		InitBullet(rooms);

	else if (bullet->getIsFired())
		bulletMovement(maze,enemy);

	else // Bullet hit wall or enemy
		bullet = nullptr;

}

void Soldier::InitBullet(Room** rooms)
{
	double xx, yy, angle,x,y;
	isInSameRoomWithTarget(rooms);
	if (isInSameRoom == true)
	{
		if (ammo > 0)
		{
			yy = target->getRow() - c->getRow();		//y for angle
			xx = target->getCol() - c->getCol();		//x for angle
			angle = atan2((double)yy, (double)xx);
			x = c->getCol()+0.5;							//x for starting point
			y = c->getRow()+0.5;							//y for starting point
			bullet = new Bullet(x, y, angle);
			bullet->setIsFired(true);
			ammo--;
		}
	}
}


void Soldier::bulletMovement(int maze[MSZ][MSZ],Soldier* enemy)
{

	Soldier* e=enemy;

	if (maze[(int)bullet->gety()][(int)bullet->getX()] == enemy->getColor())
	{
		e->updateHP(-BULLET_DAMAGE);

		if (e->getHP() < 0)
			e->setHP(0);

		bullet->setIsFired(false);
		bullet = nullptr;
	}
		
	if(bullet!=nullptr)
		bullet->move(maze);
}

void Soldier::throwGrenade(Room** rooms, int maze[MSZ][MSZ], Soldier* enemy1,Soldier* enemy2)
{
	if (healthPoints <= 0)
		return;

	if (grenade == nullptr) // Initialize grenade
		InitGrenade(rooms, maze);

	else if (grenade->getIsExploded())
	{
		bullet = nullptr;
		grenadeMovement(maze, enemy1, enemy2);
		
	}	
	else if (bullet!= nullptr && bullet->getIsFired())
	{
		bulletBeforeGrenade(maze);
	}
	else
		grenade = nullptr;
		

}

void Soldier::InitGrenade(Room** rooms, int maze[MSZ][MSZ])
{
	double xx, yy, angle, x, y,tx,ty;
	isInSameRoomWithTarget(rooms);
	if (isInSameRoom == true)
	{
		if (grenades > 0)
		{
			targetForThrow(&tx,&ty,maze);
			grenade = new Grenade(tx, ty);
			yy =ty - c->getRow();		
			xx =tx - c->getCol();
			angle = atan2((double)yy, (double)xx);
			x = c->getCol() + 0.5;							//x for starting point
			y = c->getRow() + 0.5;							//y for starting point
			
			bullet = new Bullet(x, y, angle);
			bullet->setIsFired(true);
			grenades--;
		}
	}
}

void Soldier::grenadeMovement(int maze[MSZ][MSZ], Soldier* enemy1,Soldier* enemy2)
{
	int counter = 0;
	Soldier* e;

	for (int i = 0; i < NUM_BULLETS; i++)
	{
		if (grenade->getBullets()[i] != nullptr)
		{

			if ((enemy1!=nullptr && maze[(int)grenade->getBullets()[i]->gety()][(int)grenade->getBullets()[i]->getX()] == enemy1->getColor()) || (enemy2!=nullptr && maze[(int)grenade->getBullets()[i]->gety()][(int)grenade->getBullets()[i]->getX()] == enemy2->getColor()))
			{
				if (enemy1 != nullptr && maze[(int)grenade->getBullets()[i]->gety()][(int)grenade->getBullets()[i]->getX()] == enemy1->getColor())
					e = enemy1;
				else
					e = enemy2;

				e->updateHP(-GRENADE_DAMAGE);

				if (e->getHP() < 0)
					e->setHP(0);

				grenade->getBullets()[i]->setIsFired(false);
				grenade->getBullets()[i] = nullptr;
				counter++;
			}
		}
		else
			counter++;
	}

	if (counter == getGrenade()->getNumOfShards())
		grenade->setIsExploded(false);

	if (grenade != nullptr && grenade->getIsExploded())
		grenade->explode(maze);
}



void Soldier::targetForThrow(double* xx, double* yy, int maze[MSZ][MSZ])
{
	*xx = target->getCol();
	*yy = target->getRow();

	// Check Up
	if (maze[target->getRow() + 1][target->getCol()] == SPACE)
		*yy += 1;
	// Check Right
	else if (maze[target->getRow()][target->getCol() + 1] == SPACE)
		*xx += 1;
	// Check Left
	else if (maze[target->getRow()][target->getCol() - 1] == SPACE)
		*xx -= 1;
	// Check Down
	else if (maze[target->getRow() - 1][target->getCol()] == SPACE)
		*yy -= 1;
	// Check Up - Right
	else if (maze[target->getRow() + 1][target->getCol() + 1] == SPACE)
	{
		*yy += 1;
		*xx += 1;
	}
	// Check Up - Left
	else if (maze[target->getRow() + 1][target->getCol() - 1] == SPACE)
	{
		*xx -= 1;
		*yy += 1;
	}
	// Check Down - Left
	else if (maze[target->getRow() - 1][target->getCol() - 1] == SPACE)
	{
		*xx -= 1;
		*yy -= 1;
	}
	// Check Down - Right
	else if (maze[target->getRow() - 1][target->getCol() + 1] == SPACE)
	{
		*xx += 1;
		*yy += 1;
	}
		
}

void Soldier::bulletBeforeGrenade(int maze[MSZ][MSZ])
{
	double tx, ty;
	targetForThrow(&tx, &ty, maze);
	if (maze[(int)bullet->gety()][(int)bullet->getX()] == maze[(int)ty][(int)tx])
	{
		bullet->setIsFired(false);
		//bullet = nullptr;
		grenade->setIsExploded(true);
	}
	if (bullet ->getIsFired())
		bullet->move(maze);
}
















































