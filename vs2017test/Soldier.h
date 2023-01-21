#pragma once
#include "Cell.h"
#include <queue>
#include <vector>
#include "CompareSoldierCells.h"
#include "CompareCells.h"
#include "math.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Room.h"
#include "CompareCellsToRun.h"

using namespace std;



class Soldier
{
private:
	int x;
	int y;
	int ammo;
	int grenades;
	int healthPoints;
	int color;
	int roomNum;
	bool isInSameRoom;
	bool isHidden;

	
	Cell* c=nullptr;
	Cell* target=nullptr;

	priority_queue <Cell*, vector<Cell*>, CompareSoldierCells > pq;
	vector<Cell*> visited;

	Bullet* bullet;
	Grenade* grenade;


public:
	Soldier();
	Soldier(int xx,int yy,int c);
	~Soldier();
	int getX() { return x; }
	int getY() { return y; }
	void setX(int xx) { x = xx; }

	void setY(int yy) { y = yy; }
	bool getIsInSameRoom() { return isInSameRoom; }
	void setIsInSameRoom(bool flag) { isInSameRoom = flag; }
	bool getIsHidden() { return isHidden; }
	void setIsHidden(bool flag) { isHidden = flag; }
	int getAmmo() { return ammo; }
	int getGrenades() { return grenades; }
	int getHP() { return healthPoints; }
	void setAmmo(int a) { ammo=a; }
	void setGrenades(int g) { grenades=g; }
	void setHP(int hp) { healthPoints=hp; }
	void setColor(int c) { color=c; }
	int getColor() { return color; }
	void setRoomNum(int num) { roomNum = num; }
	int getRoomNum() { return roomNum; }
	void setCurrentCell(Cell* cell) { c = cell; }
	void setCurrentTarget(Cell* cell) { target = cell; }
	Cell* getCurrentCell() { return c; }
	Cell* getTarget() { return target; }
	vector<Cell*>& getVisited() { return visited; }
	Bullet* getBullet() { return bullet; }
	void deleteSoldier();
	Grenade* getGrenade() { return grenade; }

	int AuclidicDistance(Cell* c);
	void SearchForEnemy(Soldier* enemy1,Soldier* enemy2);
	void isInSameRoomWithTarget(Room** rooms);
	void updateSoldiersRoom(Room** rooms);
	void shoot(Room** rooms,int maze[MSZ][MSZ], Soldier* enemy);
	void InitBullet(Room** rooms);
	void bulletMovement(int maze[MSZ][MSZ], Soldier* enemy);

	void throwGrenade(Room** rooms, int maze[MSZ][MSZ], Soldier* enemy1, Soldier* enemy2);
	void InitGrenade(Room** rooms, int maze[MSZ][MSZ]);
	void grenadeMovement(int maze[MSZ][MSZ], Soldier* enemy1, Soldier* enemy2);
	void targetForThrow(double* x, double* y, int maze[MSZ][MSZ]);
	void bulletBeforeGrenade(int maze[MSZ][MSZ]);

	Soldier* findEnemyByTarget(Soldier* e1, Soldier* s2);
	void updateHP(int num) { healthPoints += num; }

	priority_queue <Cell*, vector<Cell*>, CompareSoldierCells >& getPq() { return pq; }


	void searchWhereToRun(int maze[MSZ][MSZ], int color);
	void RestorePath(Cell* pCurrent, int maze[MSZ][MSZ]);
	void CheckNeighbor(Cell* pCurrent, int nrow, int ncol, priority_queue <Cell, vector<Cell>, CompareCellsToRun>& pq1, vector <Cell>& grays, vector <Cell>& blacks, int maze[MSZ][MSZ]);
	double ManhattanDistance(int x1, int y1, int x2, int y2);

};

