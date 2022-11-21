#define MAP_SIZE	10
#define UP			72
#define DOWN		80
#define LEFT		75
#define RIGHT		77

#define SWAP(a, b) {int tmp=a; a=b; b=tmp;}

#pragma warning(disable: 6001)
#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
typedef enum GameObjectType {
	Street = 0,
	Wall = 1,
	// Lower than 200 = Entity
	Drunken = 101,
	Cop,
	Sniper,
	// Higher than 200 = 
	Pub = 201,
	Home
}GameObjectType;

typedef struct GameObject { int x, y, type, prevQuad, curQuad, isAuto; } GameObject;

int buffer_map[MAP_SIZE][MAP_SIZE];
int map[MAP_SIZE][MAP_SIZE] = {
	1,0,0,1,1,0,0,0,0,0,
	0,0,0,0,0,0,1,0,1,0,
	0,1,1,1,1,0,1,0,0,0,
	0,0,0,0,0,0,1,0,1,0,
	0,1,1,0,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,1,1,1,1,0,1,0,1,0,
	0,0,0,0,0,0,1,0,0,0,
	0,1,1,0,1,0,1,0,1,0,
	0,0,0,0,0,0,0,0,0,0
};
int pubX, pubY, homeX, homeY;

void gotoxy(short x, short y) {
	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
bool InBorder(GameObject* entity, int moveDir);
bool LimitBorder(GameObject* entity, int moveDir);
void Move(GameObject* entity);
void Awake();
GameObject* CreateGameObject(GameObjectType type, int quad);
int SolveQuad(int x, int y);
bool Patrol(GameObject* entity1, GameObject* entity2);
bool Sniping(GameObject* sniper);
bool Alive(GameObject* entity);
void RenderScreen();
void DrawChar(short x, short y, int type);
void FindStructure();


int main() {
	Awake();
	bool isAlived = false;
	GameObject* drunken = CreateGameObject(Drunken, 1);
	GameObject* cop1 = CreateGameObject(Cop, 2);
	GameObject* cop2 = CreateGameObject(Cop, 3);
	GameObject* sniper = CreateGameObject(Sniper, 4);
	drunken->isAuto = false;
	while (isAlived != true) {
		Sleep(100);
		RenderScreen();
		isAlived = Alive(drunken);
		if (isAlived) continue;
		Move(drunken);
		Move(cop1);
		Move(cop2);
		Move(sniper);
	}
}

// 전체 맵 경계 + 각 entity type별 추가적인 결계 설정
bool InBorder(GameObject* entity, int moveDir) {
	switch (moveDir) {
	case UP:	return entity->y > 0 && LimitBorder(entity, moveDir); break;
	case DOWN:	return entity->y < 9 && LimitBorder(entity, moveDir); break;
	case LEFT:	return entity->x > 0 && LimitBorder(entity, moveDir); break;
	case RIGHT: return entity->x < 9 && LimitBorder(entity, moveDir); break;
	}
}

bool LimitBorder(GameObject* entity, int moveDir) {
	
	int x, y;
	if (entity->type == Drunken) {
		if (entity->curQuad == entity->prevQuad) return true;

		if(moveDir == UP && SolveQuad(entity->x, entity->y - 1) != entity->prevQuad)	return true;
		else if(moveDir == DOWN && SolveQuad(entity->x, entity->y + 1) != entity->prevQuad)	return true;
		else if(moveDir == LEFT && SolveQuad(entity->x - 1, entity->y) != entity->prevQuad)	return true;
		else if(moveDir == RIGHT && SolveQuad(entity->x + 1, entity->y) != entity->prevQuad)	return true;
	}
	else {
		if (entity->curQuad % 2) x = 5, y = (entity->curQuad / 2 == 0) ? 5 : 4;
		else x = 4, y = (entity->curQuad / 2 == 1) ? 5 : 4;

		if (moveDir == UP)	 return entity->y - 1 == y ? false : true;
		if (moveDir == DOWN) return entity->y + 1 == y ? false : true;
		if (moveDir == LEFT) return entity->x - 1 == x ? false : true;
		if (moveDir == RIGHT)return entity->x + 1 == x ? false : true;
	}

	return false;
}

void Move(GameObject* entity){
	if (entity->type == Sniper || entity->type > 200) return;

	int destPosX, destPosY, block; // 이동 후의 좌표와 블럭 
	int directions[4] = { UP, DOWN, LEFT, RIGHT }, randDir;	// 방향
	bool inBorder, isMoved = false;

	if (entity->isAuto == true) {
		while (isMoved == false) {
			randDir = rand() % 4;

			if (directions[randDir] == UP)			destPosX = entity->x, destPosY = entity->y - 1;
			else if (directions[randDir] == DOWN)	destPosX = entity->x, destPosY = entity->y + 1;
			else if (directions[randDir] == LEFT)	destPosX = entity->x - 1, destPosY = entity->y;
			else if (directions[randDir] == RIGHT)	destPosX = entity->x + 1, destPosY = entity->y;
			
			if (InBorder(entity, directions[randDir]) == true) {
				isMoved = map[destPosY][destPosX] != Wall && map[destPosY][destPosX] != Sniper;
			}
		}
	}
	else {
		while (isMoved == false) {

			int keyboard = _getch();

			if (keyboard == UP)			destPosX = entity->x, destPosY = entity->y - 1;
			else if (keyboard == DOWN)	destPosX = entity->x, destPosY = entity->y + 1;
			else if (keyboard == LEFT)	destPosX = entity->x - 1, destPosY = entity->y;
			else if (keyboard == RIGHT)	destPosX = entity->x + 1, destPosY = entity->y;

			if (InBorder(entity, keyboard) == true) {
				isMoved = map[destPosY][destPosX] != Wall && map[destPosY][destPosX] != Sniper;
			};

			
		}
	}
	if (entity->x == pubX && entity->y == pubY) map[entity->y][entity->x] = Pub;
	else if (entity->x == homeX && entity->y == homeY) map[entity->y][entity->x] = Home;
	else map[entity->y][entity->x] = Street;

	entity->x = destPosX, entity->y = destPosY;	
	map[entity->y][entity->x] = entity->type;


	if (entity->curQuad != SolveQuad(entity->x, entity->y)) {
		entity->prevQuad = entity->curQuad;
		entity->curQuad = SolveQuad(entity->x, entity->y);
	}
}

void Awake(){
	srand(time(NULL));
	for (int i = 0; i < MAP_SIZE; i++)
		memset(buffer_map[i], -1, sizeof(int) * MAP_SIZE);

	CreateGameObject(Pub, 1);
	CreateGameObject(Home, 4);

	FindStructure();

	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 1;
	cci.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

GameObject* CreateGameObject(GameObjectType type, int quad){
	GameObject* newEntity = (GameObject*)malloc(sizeof(GameObject));

	int x, y, px, py;
	bool isSpawned = false, foundPub = false;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (map[i][j] == Pub) {
				px = j;
				py = i;
				foundPub = true;
			}
		}
	}
	while (isSpawned == false) {
		if (type == Drunken) {

			while (true) {
				int randDir = rand() % 4;
				if (randDir == 0 && px > 0) x = px - 1, y = py;
				if (randDir == 1 && px < 9) x = px + 1, y = py;
				if (randDir == 2 && py > 0) y = py - 1, x = px;
				if (randDir == 3 && py < 9) y = py + 1, x = px;

				if (map[y][x] == Street) break;
			}
		}
		else {
			// quad: 1, 3
			if (quad % 2) x = rand() % 5, y = (quad / 2 == 0) ? rand() % 5 : rand() % 5 + 5;
			// quad: 2, 4
			else x = rand() % 5 + 5, y = (quad / 2 == 1) ? rand() % 5 : rand() % 5 + 5;
		}
		

 		if (map[y][x] == Street && type == Sniper) continue;
		else if (map[y][x] == Wall && type != Sniper) continue;
		isSpawned = true;

		newEntity->x = x;
		newEntity->y = y;
		newEntity->prevQuad = SolveQuad(x, y);
		newEntity->curQuad = SolveQuad(x, y);
		newEntity->isAuto = true;
		newEntity->type = type;
		map[y][x] = type;
	}

	return newEntity;
}

int SolveQuad(int x, int y) {
	if (x / 5)	return y < 5 ? 2 : 4;
	else		return y < 5 ? 1 : 3;
}

bool Detecting(GameObject* entity) {
	if (entity->type == Drunken) return false;

	if (entity->type == Cop) {
		int dx = -1, dy, cx = -1, cy, min, max;
		for (int i = 0; i < 10; i++) 
			for (int j = 0; j < 10; j++) {
				if (map[i][j] == Drunken) dx = j, dy = i;
				else if (map[i][j] == Cop) cx = j, cy = i;
			}
		if (dx == -1 || cx == -1) return false;

		if (dx == cx) {
			if (dy > cy) SWAP(dy, cy);
			for (int i = dy; i <= cy; i++)
				if (map[i][dx] == Wall) return false;
		}
		else if (dy == cy) {
			if (dx > cx) SWAP(dx, cx);
			for (int i = dx; i <= cx; i++)
				if (map[dy][i] == Wall) return false;
		}

	}
	else if (entity->type == Sniper) {
		if (map[entity->y + 1][entity->x] == Drunken) return true;
		else if (map[entity->y - 1][entity->x] == Drunken) return true;
		else if (map[entity->y][entity->x + 1] == Drunken) return true;
		else if (map[entity->y][entity->x - 1] == Drunken) return true;
		else return false;
	}
	return true;
}

bool Sniping(GameObject* sniper) {
	return true;
	//return map[sniper->y][sniper->x] == Drunken ? 
}

bool Alive(GameObject* entity) {
	int key = 0;
	if (entity->x == homeX && entity->y == homeY) {
		gotoxy(0, 10);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("!!!도착!!!\n(엔터를 누르면 종료됩니다)");
		while (key != 13) {
			key = _getch();
		}
		return true;
	}
	return false;
}

void RenderScreen(){
	for (int i = 0; i < MAP_SIZE; ++i) 
		for (int j = 0; j < MAP_SIZE; ++j) 
			if (map[i][j] != buffer_map[i][j]) {
				buffer_map[i][j] = map[i][j];
				DrawChar(j, i, buffer_map[i][j]);
			}
}

void DrawChar(short x, short y, int type){
	gotoxy(x, y);
	if (type == Drunken)		printf("ⓓ");
	else if (type == Cop)		printf("ⓒ");
	else if (type == Sniper)	printf("ⓢ");
 	else if (type == Pub)		printf("Ｐ");
	else if (type == Home)		printf("Ｈ");
	else if (type == Wall)		printf("■");
	else if (type == Street)	printf("□");
	//gotoxy(0, 10);
}

void FindStructure(){
	for (int i = 0; i < 10; i++) 
		for (int j = 0; j < 10; j++) {
			if (map[i][j] == Pub) pubX = j, pubY = i;
			if (map[i][j] == Home) homeX = j, homeY = i;
		}
}