#pragma once
const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;	
const int GRAY = 5;
const int WHITE = 6;	//space
const int HEALTH = 7;	//health, green
const int AMMO = 8;		//ammo, brown
const int TEAM1 = 9;	//first team, blue
const int TEAM2 = 10;	//second team, red
const int TECH1 = 11;	//technitian of first team, blue
const int TECH2 = 12;	//technitian od second team, red

const int MSZ = 100;

const int NUM_ROOMS = 15;
const int MAX_ROOM_WIDTH = MSZ / 5;
const int MAX_ROOM_HEIGHT = MSZ / 5;
const int MIN_ROOM_WIDTH = 10;
const int MIN_ROOM_HEIGHT = 10;

const int SOLDIERS_IN_TEAM = 2;

const int BULLET_DAMAGE = 1;
const int GRENADE_DAMAGE = 3;


//soldiers params
const int MAX_HP = 5;
const int MAX_AMMO = 10;
const int MAX_GRENADES = 2;
