#pragma once

#include "fieldObject.h"

#include <vector>
#include <windows.h>

class Field {
public:	
	static const unsigned int CMD_UP = 72;
	static const unsigned int CMD_DOWN = 80;
	static const unsigned int CMD_RIGHT = 77;
	static const unsigned int CMD_LEFT = 75;
	static const unsigned int CMD_FIRE = 32;
	static const unsigned int CMD_EXIT = (unsigned int) 'e';

	Field();
	~Field();

	const bool makeTurn(const unsigned int cmd);
	
private:
	static const int WIDTH = 40;
	static const int HEIGHT = 20;	
	static const int MAX_WALLS = 50;
	static const int MAX_FOES = 5;
	
	static const unsigned int FRAME = 0x000F;
	static const unsigned int WALL = 0x0080;
	static const unsigned int ME = 0x000A;
	static const unsigned int FOE = 0x000C;
	static const unsigned int MESSAGE_GOOD = 0x000A;
	static const unsigned int MESSAGE_BAD = 0x0008;
	
	static const unsigned char C_V = 179;
	static const unsigned char C_H = 196;
	static const unsigned char C_RT = 191;
	static const unsigned char C_LB = 192;
	static const unsigned char C_LT = 218;
	static const unsigned char C_RB = 217;
	
	static const unsigned char BULLET = 249;	
	
	void renderField();
	void renderTopBorder();
	void renderBottomBorder();
	void renderRow(const int y);
	void renderHorizontalBorder(unsigned char l, unsigned char r);
	
	void renderScores();
	
	void setAttr(const unsigned int attr);
	
	const bool makeMove(unsigned int *pX, unsigned int *pY, const int maxFoe, unsigned int cmd);
	const bool makeMyMove(unsigned int cmd);
	void moveFoes();
	void fire();
	const bool moveMyBullets();	
	void removeFoe(const int i);
	const bool moveFoeBullets();		
	
	static void setLastDirection(FieldObject &fo, const unsigned char cmd);
	static const unsigned char randomCommand();
	
	char message[255];
	unsigned int attr_message;
	
	HANDLE hConsole;	
		
	int turns;
	int lives;
	FieldObject me;	
	std::vector<FieldObject> walls;	
	std::vector<FieldObject> foes;	
	std::vector<FieldObject> myBullets;	
	std::vector<FieldObject> foeBullets;
};
