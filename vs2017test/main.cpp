#include "glut.h"
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Cell.h"
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Ammo.h"
#include "Health.h"
#include "Soldier.h"
#include "Technician.h"
#include "CompareCellsByColor.h"


using namespace std;


Room* rooms[NUM_ROOMS];
Health* h1=new Health();
Health* h2=new Health();
Ammo* a1 =new Ammo();
Ammo* a2=new Ammo();
Soldier* s1=new Soldier();
Soldier* s2 = new Soldier();
Soldier* s3 = new Soldier();
Soldier* s4 = new Soldier();
Technician* t1 = new Technician();
Technician* t2 = new Technician();
int maze[MSZ][MSZ];
double security_map[MSZ][MSZ] = {0};
bool startAStar = false, isPlaying = false,securityM=true;

void InitMaze();
void InitRooms();
void DigTunnels();
void InitAmmo(Ammo* a);
void InitSoldier(Soldier* s, int color, int roomNum);
void InitHealth(Health* h);
void hideAStar( Soldier* s);
void play();
void CreateSecurityMap();
void InitTechnitian(Technician* t, int color, int roomNum);
void RestorePathToHide(Cell* pc, Soldier* s);
void CheckNeighborToHide(Cell* pCurrent, int nrow, int ncol, priority_queue <Cell, vector<Cell>, CompareCellsByColor >& pq, vector <Cell>& grays, vector <Cell>& blacks);

Bullet* pb = nullptr;
Grenade* pg = nullptr;


void init()
{
	int room1, room2;

	glClearColor(0.3, 0.3, 0.3, 0);// color of window background
	//           RED GREEN BLUE

	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	srand(time(0)); // initialize random seed

	InitMaze();
	InitRooms();
	DigTunnels();

	InitHealth(h1);
	InitHealth(h2);
	InitAmmo(a1);
	InitAmmo(a2);
	room1 = rand() % NUM_ROOMS;
	do
	{
		room2 = rand() % NUM_ROOMS;
	} while (room1 == room2);
	InitSoldier(s1, TEAM1, room1);
	InitSoldier(s2, TEAM1, room1);
	InitSoldier(s3, TEAM2, room2);
	InitSoldier(s4, TEAM2, room2);
	InitTechnitian(t1,TEAM1,room1);
	InitTechnitian(t2,TEAM2,room2);


}

void FillRoom(int index) 
{
	int i, j;
	int cr, cc, w, h;

	w = rooms[index]->getW();
	h = rooms[index]->getH();
	cr = rooms[index]->getCenterRow();
	cc = rooms[index]->getCenterCol();


	for (i = cr - h / 2; i <= cr + h / 2; i++)
		for (j = cc - w / 2; j <= cc + w / 2; j++)
			maze[i][j] = SPACE;
}

bool hasOverlapping(int index,int w, int h, int crow, int ccol)
{
	int i;
	int dx, dy;
	bool overlap = false;
	for (i = 0; i < index && !overlap; i++)
	{
		dx = abs(ccol - rooms[i]->getCenterCol());
		dy = abs(crow - rooms[i]->getCenterRow());
		if (dx <w/2 + rooms[i]->getW()/2 +3 && 
				dy< h/2 +rooms[i]->getH()/2+3)
			overlap = true;
	}
	return overlap;
}

void InitHealth(Health* h)
{
	
	int num = rand() % NUM_ROOMS;
	do {
		h->setX((rooms[num]->getCenterRow() - rooms[num]->getH() / 2) + rand() % rooms[num]->getH());
		h->setY((rooms[num]->getCenterCol() - rooms[num]->getW() / 2) + rand() % rooms[num]->getW());
	} while (maze[h->getX()][h->getY()] != SPACE);
	maze[h->getX()][h->getY()] = HEALTH;
}

void InitAmmo(Ammo* a)
{
	int num = rand() % NUM_ROOMS;
	do {
		a->setX((rooms[num]->getCenterRow() - rooms[num]->getH() / 2) + rand() % rooms[num]->getH());
		a->setY((rooms[num]->getCenterCol() - rooms[num]->getW() / 2) + rand() % rooms[num]->getW());
	} while (maze[a->getX()][a->getY()] != SPACE);
	maze[a->getX()][a->getY()] = AMMO;
}

void InitSoldier(Soldier* s,int color,int roomNum)
{
	int x, y;
	Cell* current;
	do {
		x=(rooms[roomNum]->getCenterRow() - rooms[roomNum]->getH() / 2+2) + rand() % (rooms[roomNum]->getH()-2);
		y=(rooms[roomNum]->getCenterCol() - rooms[roomNum]->getW() / 2+2) + rand() % (rooms[roomNum]->getW()-2);
	} while (maze[x][y] != SPACE);

	
	s->setX(x);
	s->setY(y);
	s->setColor(color);
	s->setRoomNum(roomNum);
	maze[x][y] = s->getColor();

	current = new Cell(x, y, nullptr);
	s->setCurrentCell(current);
	s->getPq().push(current);
}

void InitTechnitian(Technician* t, int color, int roomNum)
{
	int x, y;
	Cell* current;
	do {
		x = (rooms[roomNum]->getCenterRow() - rooms[roomNum]->getH() / 2 + 2) + rand() % (rooms[roomNum]->getH() - 2);
		y = (rooms[roomNum]->getCenterCol() - rooms[roomNum]->getW() / 2 + 2) + rand() % (rooms[roomNum]->getW() - 2);
	} while (maze[x][y] != SPACE);

	t->setColor(color);
	t->setRoom(roomNum);
	if (color == TEAM1)
		maze[x][y] = TECH1;
	else
		maze[x][y] = TECH2;

	current = new Cell(x, y, nullptr);
	t->setCurrentCell(current);

}

void InitRooms() 
{
	int crow, ccol, w, h;

	for (int i = 0; i < NUM_ROOMS; i++)
	{
		// init a room
		do {
			w = MIN_ROOM_WIDTH + rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH);
			h = MIN_ROOM_HEIGHT + rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT);
			crow = 2 + h / 2 + rand() % (MSZ - (5 + h));
			ccol = 2 + w / 2 + rand() % (MSZ - (5 + w));
		} while (hasOverlapping(i, w, h, crow, ccol));
		rooms[i] = new Room(crow, ccol, w, h);
		FillRoom(i);
	}

	// set random obstacles
	for (int i = 0; i < 200; i++)
		maze[rand() % MSZ][rand() % MSZ] = WALL;

	
}

void InitMaze() 
{
	int i, j;
	// setup inner space of maze
	for(i=0;i<MSZ;i++)
		for (j = 0; j < MSZ ; j++)
			maze[i][j] = WALL; 


}


void RestorePath(Cell* pCurrent,int start_row, int start_col)
{
	
//	while (!(current.getRow() == start_row && current.getCol() == start_col))
	while(pCurrent->getParent()!=nullptr)
	{
		if (maze[pCurrent->getRow()][pCurrent->getCol()] == WALL)
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
		pCurrent = pCurrent->getParent();
	}
}


// who is the neighbor at nrow ncol? If it is white then paint it gray and add to pq
// If it is gray then check two cases: 
// 1. if F of this neighbor is now better then what was before then we need to update the neighbor
// 2. if it is not better then do nothing
// If it is black do nothing
// If it is Target then we have two cases (actually this is one of the previous cases, white or gray):
// 1. if F of this target is now better then what was before then we need to update the target
// 2. if it is not better then do nothing
void CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells> &pq,
	vector <Cell> &grays, vector <Cell> &blacks)
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	double space_cost = 0.1, wall_cost = 2,cost;
	if (maze[nrow][ncol] == SPACE)
		cost = space_cost;
	else cost = wall_cost;
	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		pCurrent->getG()+cost, pCurrent);
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
					cout << "ERROR! PQ is empty in update PQ\n";
					exit(1);
				}
				tmpCell = pq.top();
				pq.pop();
				if(!(tmpCell == (*pn))) // do not copy the Cell we were looking for to tmp!(see line 173) 
					tmp.push_back(tmpCell);
			} while (!(tmpCell==(*pn)));
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

// runs A* from room i to room j
void DigPath(int i, int j) 
{
	int row, col;
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell,vector<Cell>,CompareCells> pq;
	Cell* pstart = new Cell(rooms[i]->getCenterRow(), rooms[i]->getCenterCol(),
		rooms[j]->getCenterRow(), rooms[j]->getCenterCol(), 0, nullptr);

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
		if (pCurrent->getRow() == rooms[j]->getCenterRow() &&
			pCurrent->getCol() == rooms[j]->getCenterCol()) // then it is target
		{ //in this case there cannot be a better path to target!!!
			RestorePath(pCurrent, rooms[i]->getCenterRow(), rooms[i]->getCenterCol());
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
		if (row > 0) // we can go UP
			CheckNeighbor(pCurrent, row - 1, col, pq, grays, blacks);

		if(row<MSZ-1) // DOWN
			CheckNeighbor(pCurrent, row + 1, col, pq, grays, blacks);

		if (col < MSZ - 1) // RIGHT
			CheckNeighbor(pCurrent, row , col+ 1, pq, grays, blacks);

		if (col > 0)//LEFT
			CheckNeighbor(pCurrent, row, col - 1, pq, grays, blacks);

	}
}

void DigTunnels() 
{
	int i, j;
	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			DigPath(i, j); // A*
			cout << "The path from room " << i << " to room " << j << " has been digged\n";
		}
}

void ShowMaze() 
{
	int i, j;
	for(i=0;i<MSZ;i++)
		for(j=0;j<MSZ;j++)
		{
			switch (maze[i][j]) 
			{
			case WALL: 
				glColor3d(0, 0, 0);// set color black
				break;
			case SPACE: 
//				glColor3d(1, 1, 1);// set color white
				// show security data
				glColor3d(1-security_map[i][j], 1 - security_map[i][j], 1 - security_map[i][j]);
				break;
			case HEALTH:
				glColor3d(0, 1., 0);// set color green
				break;
			case AMMO:
				glColor3d(0.647059,0.164706,0.164706);// set color brown
				break;
			case TEAM1:
				glColor3d(0, 0, 1.);// set color blue
				break;
			case TEAM2:
				glColor3d(1., 0, 0);// set color red
				break;
			case TECH1:
				glColor3d(0.62352,0.372549,0.623529);// set color light-blue
				break;
			case TECH2:
				glColor3d(1., 0.5, 0.5);// set color light-red
				break;

			} // switch
			// now show the cell of maze
			glBegin(GL_POLYGON);
			glVertex2d(j,i); // left-bottom vertex
			glVertex2d(j,i+1); // left-top vertex
			glVertex2d(j+1,i+1); // right-top vertex
			glVertex2d(j+1,i); // right-bottom vertex
			glEnd();
		}
}

double ManhattanDistance(int x1, int y1,int x2,int y2)
{
	return (abs(x1 - x2) + abs(y1 - y2));
}

void cleanAStar(Soldier* s)
{
	while (!s->getPq().empty())
		s->getPq().pop();
	s->getVisited().clear();
	Cell* pCurrent = new Cell(s->getCurrentCell()->getRow(), s->getCurrentCell()->getCol(), nullptr);
	s->getPq().push(pCurrent);
	s->setCurrentCell(pCurrent);
}

void cleanMaze(Soldier* s)
{

	while (!s->getVisited().empty())
	{
		Cell* pCurrent;
		pCurrent = s->getVisited().back();
		s->getVisited().pop_back();
		if (maze[pCurrent->getRow()][pCurrent->getCol()] != s->getColor())
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
	}
}

void RestorePath(Cell* pc, Soldier* s)
{

	if (maze[pc->getParent()->getRow()][pc->getParent()->getCol()]==s->getColor())
		return;
	while (pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
	}
	Cell* pCurrent = new Cell(pc->getRow(), pc->getCol(), nullptr);
	maze[pc->getRow()][pc->getCol()] = s->getColor();
	maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
	s->setCurrentCell(pCurrent);
	s->getPq().push(pCurrent);
}

void CheckNeighbor(Cell* pCurrent, int row, int col, Soldier* s)
{
	int color;
	if (s->getColor() == TEAM1)
		color = TEAM2;
	else
		color = TEAM1;

	if (maze[row][col] == color) {
		RestorePath(pCurrent, s);
		cleanMaze(s);
		startAStar = false;
		cout << "found" << endl;
		s->getPq().empty();
		return;
	}
	else
	{

		Cell* pc = new Cell(row, col, pCurrent);

		maze[row][col] = GRAY;
		pc->setG(pCurrent->getG() + 1);
		pc->setH(ManhattanDistance(row, col, s->getTarget()->getRow(), s->getTarget()->getCol()));
		pc->setF(pc->getH()+pc->getG());
		s->getPq().push(pc);
		s->getVisited().push_back(pc);
	}
}

void AStarIteration(Soldier* s)
{
	int color;
	if (s->getColor() == TEAM1)
		color = TEAM2;
	else
		color = TEAM1;

	Cell* pCurrent;

	if (s->getPq().empty())
	{
		startAStar = false;
		return;
	}
	else
	{
		pCurrent = s->getPq().top();
		s->getPq().pop();
		s->getVisited().push_back(pCurrent);

		int row, col;
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;
		if(startAStar)
			if (maze[row + 1][col] == SPACE || maze[row + 1][col] == color)
				CheckNeighbor(pCurrent, row + 1, col, s);

		if (startAStar)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == color )
				CheckNeighbor(pCurrent, row - 1, col, s);

		if (startAStar)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == color )
				CheckNeighbor(pCurrent, row, col + 1, s);

		if (startAStar)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == color )
				CheckNeighbor(pCurrent, row, col - 1, s);
	}
}

void cleanMazeFromDead(Soldier* s)
{
	maze[s->getCurrentCell()->getRow()][s->getCurrentCell()->getCol()] = SPACE;
	s->deleteSoldier();
	delete s;
}






void RestorePathToHide(Cell* pc,Soldier* s)
{

	if (pc->getParent() != nullptr)
	{
		while (pc->getParent()->getParent() != nullptr)
		{
			pc = pc->getParent();
		}
	}
	
	Cell* pCurrent = new Cell(pc->getRow(), pc->getCol(), nullptr);
	if (security_map[pCurrent->getRow()][pCurrent->getCol()] == WALL && maze[pCurrent->getRow()][pCurrent->getCol()] == SPACE)
		s->setIsHidden(true);
	maze[pc->getRow()][pc->getCol()] = s->getColor();
	maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
	s->setCurrentCell(pCurrent);

}


//func for soldier to hide behind cover when he is in the same room with enemy
void CheckNeighborToHide(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCellsByColor>& pq,
	vector <Cell>& grays, vector <Cell>& blacks)
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	double cost=0;
	if (maze[nrow][ncol] != WALL)
		cost = -security_map[nrow][ncol];

	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol, pCurrent);
	pn->setGColor(pCurrent->getGColor() + cost);
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
		if (pn->getGColor() < it_gray->getGColor()) // then update it (F of neighbor)!!!
		{
			// we need toupdate it in two places:
			// 1. in vector grays
			it_gray->setGColor(pn->getGColor());
			// 2. in PQ
			// to update a Cell in pq we need to remove it from pq, to update it and to push it back
			vector<Cell> tmp;
			Cell tmpCell;
			do
			{
				if (pq.empty())
				{
					cout << "ERROR! PQ is empty in update PQ\n";
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

// runs A* to hiding place when with enemy in same room
void hideAStar(Soldier* s)
{
	int row, col;
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCellsByColor> pq;
	Cell* pstart = new Cell(s->getCurrentCell()->getRow(), s->getCurrentCell()->getCol(), nullptr);
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
		if (security_map[pCurrent->getRow()][pCurrent->getCol()] == WALL && maze[pCurrent->getRow()][pCurrent->getCol()]==SPACE) // then it is target
		{ //in this case there cannot be a better path to target!!!
			cout<< pCurrent->getRow() << pCurrent->getCol() <<endl;
			cout <<s1->getCurrentCell()->getRow() << s1->getCurrentCell()->getCol() << endl;
			RestorePathToHide(pCurrent,s);
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
		if (row -1 !=WALL && rooms[s->getRoomNum()]->getCenterRow() - rooms[s->getRoomNum()]->getH() / 2<row-1) // we can go UP
			CheckNeighborToHide(pCurrent, row - 1, col, pq, grays, blacks);

		if (row +1!=WALL && rooms[s->getRoomNum()]->getCenterRow() + rooms[s->getRoomNum()]->getH() / 2>row+1) // DOWN
			CheckNeighborToHide(pCurrent, row + 1, col, pq, grays, blacks);

		if (col+1 !=WALL && rooms[s->getRoomNum()]->getCenterCol() + rooms[s->getRoomNum()]->getW() / 2>col+1) // RIGHT
			CheckNeighborToHide(pCurrent, row, col + 1, pq, grays, blacks);

		if (col-1 !=WALL && rooms[s->getRoomNum()]->getCenterCol() - rooms[s->getRoomNum()]->getW() / 2>col-1)//LEFT
			CheckNeighborToHide(pCurrent, row, col - 1, pq, grays, blacks);

	}
}



void soldierRound(Soldier* s)
{
	Soldier* enemy;


	//if the soldier have 0 hp then we clean the maze from him
	if (s->getHP() == 0)
	{
		cleanMazeFromDead(s);
		s = nullptr;
		return;
	}

	//searches for the nearest enemy 
	if (s->getColor() == TEAM1)
	{
		if (s3 == nullptr && s4 == nullptr)
		{
			cout << "Team Blue WON this round!!!" << endl;
				isPlaying = false;
		}
		else
			s->SearchForEnemy(s3,s4);
		enemy = s->findEnemyByTarget(s3, s4);
	}	
	else
	{
		if (s1 == nullptr && s2 == nullptr)
		{
			cout << "Team RED WON this round!!!" << endl;
			isPlaying = false;
		}
		else
			s->SearchForEnemy(s1, s2);
		enemy = s->findEnemyByTarget(s1, s2);
	}
		
	s->updateSoldiersRoom(rooms);
	//checks if the soldier is in the same room with target
	if(s->getRoomNum()!=-1)
		s->isInSameRoomWithTarget(rooms);
	
	//if is not in the same room 
	if (!s->getIsInSameRoom())
	{
		//if enemy is in the tunnel then just wait ( to avoid them both stuck in there)   (they can fight only in "rooms")
		if (s->getTarget()->getRow() == enemy->getCurrentCell()->getRow() && s->getTarget()->getCol() == enemy->getCurrentCell()->getCol() && enemy->getRoomNum() == -1)
			return;
		//find the shortest path with A* and make stem toward the target
		do
		{
			AStarIteration(s);
		} while (startAStar);
		cleanAStar(s);
		startAStar = true;

		//update soldiers room ( num of room or -1 if in corridor)
		s->updateSoldiersRoom(rooms);
	}
	else
	{
		if (s->getHP() == 1 || (s->getAmmo() == 0 && s->getGrenades() == 0))
			s->searchWhereToRun(maze, s->getColor());
		else
		{
			if (!s->getIsHidden())
				hideAStar(s);
			if(s->getAmmo()==0)
				s->throwGrenade(rooms, maze, s3, s4);
			else if(s->getGrenades()==0)
				s->shoot(rooms, maze, enemy);
			else
			{
				if(ManhattanDistance(s->getCurrentCell()->getRow(), s->getCurrentCell()->getCol(), s->getTarget()->getRow(), s->getTarget()->getCol())<15)
					s->throwGrenade(rooms, maze, s3, s4);
				else
					s->shoot(rooms, maze, enemy);
			}

		}
	}
}

void findNearestHealth(Technician* t)
{
	int row, col;
	Cell* c;
	if (ManhattanDistance(t->getCurrentCell()->getRow(), t->getCurrentCell()->getCol(), h1->getX(), h1->getY()) < ManhattanDistance(t->getCurrentCell()->getRow(), t->getCurrentCell()->getCol(), h2->getX(), h2->getY()) && h1->getHealth()!=0)
	{
		c = new Cell(h1->getX(), h1->getY(), nullptr);
		t->setCurrentTarget(c);
	}
	else if(h2->getHealth() != 0)
	{
		c = new Cell(h2->getX(), h2->getY(), nullptr);
		t->setCurrentTarget(c);
	}
}

void findNearestAmmo(Technician* t)
{
	int row, col;
	Cell* c;
	if (ManhattanDistance(t->getCurrentCell()->getRow(), t->getCurrentCell()->getCol(), a1->getX(), a1->getY()) < ManhattanDistance(t->getCurrentCell()->getRow(), t->getCurrentCell()->getCol(), a2->getX(), a2->getY()) &&( a1->getGrenades() !=0||a1->getBullets()!=0 ))
	{
		c = new Cell(a1->getX(), a1->getY(), nullptr);
		t->setCurrentTarget(c);
	}
	else if(a1->getGrenades() != 0 || a1->getBullets() != 0)
	{
		c = new Cell(a2->getX(), a2->getY(), nullptr);
		t->setCurrentTarget(c);
	}
}



void technitianRound(Technician* t)
{
	int needed;
	Soldier* tmpSoldier1;
	Soldier* tmpSoldier2;
	//choose the color of the team depending on the color of technitian
	if (t->getColor() == TEAM1)
	{
		tmpSoldier1 = s1;
		tmpSoldier2 = s2;
	}
	else
	{
		tmpSoldier1 = s3;
		tmpSoldier2 = s4;
	}

	//if have 0 hp then need to go resupply
	if (t->getHP() == 0)
	{
		findNearestHealth(t);
		t->search(maze, HEALTH);
		if (t->getFoundSoldier())
			t->setHP(MAX_HP_CAPACITY);
	}
	//if have 0 ammo then need to go resupply
	else if (t->getAmmo() == 0)
	{
		findNearestAmmo(t);
		t->search(maze, AMMO);
		if (t->getFoundSoldier())
			t->setAmmo(MAX_AMMO_CAPACITY);
	}
	//if soldier1 have less then half hp then come to him and heal him
	else if (tmpSoldier1!=nullptr && tmpSoldier1->getHP() < (MAX_HP) / 2)
	{
		//come to soldier1 and heal
		t->setCurrentTarget(tmpSoldier1->getCurrentCell());
		if (t->getFoundSoldier())
		{
			t->restoreHealth(tmpSoldier1);
			t->setFountSoldier(false);
		}
		else
			t->search(maze, t->getColor());
	}
	//same heal as was to soldier 1
	else if (tmpSoldier2 != nullptr && tmpSoldier2->getHP() < (MAX_HP) / 2)
	{
		//come to soldier2 and heal
		t->setCurrentTarget(tmpSoldier2->getCurrentCell());
		if (t->getFoundSoldier())
		{
			t->restoreHealth(tmpSoldier2);
			t->setFountSoldier(false);
		}
		else
			t->search(maze, t->getColor());
	
	}
	//if soldier1 have less then half ammo then come to him and refill it
	else if (tmpSoldier1 != nullptr && tmpSoldier1->getAmmo() < (MAX_AMMO) / 2)
	{
		//come to soldier1 and give ammo
		t->setCurrentTarget(tmpSoldier1->getCurrentCell());
		if (t->getFoundSoldier())
		{
			t->restoreAmmo(tmpSoldier1);
			t->setFountSoldier(false);
		}
		else
			t->search(maze, t->getColor());
	}
	//tha same as revius soldier
	else if (tmpSoldier2 != nullptr && tmpSoldier2->getAmmo() < (MAX_AMMO) / 2)
	{
		//come to soldier2 and give ammo
		t->setCurrentTarget(tmpSoldier2->getCurrentCell());
		if (t->getFoundSoldier())
		{
			t->restoreAmmo(tmpSoldier2);
			t->setFountSoldier(false);
		}
		else
			t->search(maze, t->getColor());
	}
	else
	{
		if (tmpSoldier1 != nullptr && tmpSoldier1->getHP() > tmpSoldier2->getHP())
		{
			t->setCurrentTarget(tmpSoldier2->getCurrentCell());
			t->search(maze, tmpSoldier1->getColor());
			t->setFountSoldier(false);
		}
		else if(tmpSoldier2 != nullptr)
		{
			t->setCurrentTarget(tmpSoldier1->getCurrentCell());
			t->search(maze, tmpSoldier1->getColor());
			t->setFountSoldier(false);
		}
	}
}



void play()
{


	if(securityM)
	{
		CreateSecurityMap();
		CreateSecurityMap();
		securityM = false;
	}
	
	if(s1!=nullptr)
		soldierRound(s1);
	if (s2 != nullptr)
		soldierRound(s2);
	if (s3 != nullptr)
		soldierRound(s3);
	if (s4 != nullptr)
		soldierRound(s4);

	if (s1 != nullptr && s1->getHP() == 0)
	{
		cleanMazeFromDead(s1);
		s1 = nullptr;
	}
	if (s2 != nullptr && s2->getHP() == 0)
	{
		cleanMazeFromDead(s2);
		s2 = nullptr;
	}
	if (s3 != nullptr && s3->getHP() == 0)
	{
		cleanMazeFromDead(s3);
		s3 = nullptr;
	}
	if (s4 != nullptr && s4->getHP() == 0)
	{
		cleanMazeFromDead(s4);
		s4 = nullptr;
	}
	
	technitianRound(t1);
	technitianRound(t2);
}

void showAction(Soldier* s)
{
	if (s->getBullet() != nullptr)
	{
		s->getBullet()->show();
	}
	else if (s->getGrenade() != nullptr)
	{
		s->getGrenade()->show();
	}
}

void CreateSecurityMap()
{
	int num_simulations = 100;
	double damage = 0.001;
	int x, y;

	for(int i=0;i<num_simulations;i++)
	{
		x = rand() % MSZ;
		y = rand() % MSZ;
		Grenade* g = new Grenade(x, y);
		g->SimulateExplosion(maze, security_map,damage);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze();
	
	if(s1!=nullptr)
		showAction(s1);
	if (s2 != nullptr)
		showAction(s2);
	if (s3 != nullptr)
		showAction(s3);
	if (s4 != nullptr)
		showAction(s4);
		
	glutSwapBuffers(); // show all
}



// runs all the time in the background
void idle()
{
	if (isPlaying)
	{
		cout << "flag\n";
		play();
		glutPostRedisplay(); // indirect call to display
	//	Sleep(100);
	}

}

void menu(int choice) 
{
	if (choice == 1) // fire bullet
	{
		isPlaying = true;
		startAStar = true;
	}

}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeon");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	//glutMouseFunc(mouse);
	// add menu
	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Start", 1);



	init();

	glutMainLoop();
}