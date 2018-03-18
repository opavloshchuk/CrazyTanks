#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "field.h"

const int rand1(const int max) {
	int n = rand() % max;
	if (0 == n) {
		n = 1;
	}
	return n;
} 

Field::Field() {
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	lives = 3;
	turns = 0;
	strcpy(message,"");
	
	// Set random generator:
	time_t t;
	srand((unsigned int)time(&t));
	
	// Set my tank:
	me.setY(HEIGHT-1);
	me.setX(WIDTH / 2);
	
	// Generate walls:
	const int maxWalls = rand1(MAX_WALLS);	
	while( maxWalls>this->walls.size()) {
		const int x = rand() % WIDTH;
		const int y = rand() % HEIGHT;
		if( !me.isIn(x,y)
		    && !FieldObject::existsIn(x,y, this->walls)) {
			this->walls.push_back( *(new FieldObject(x,y)));
		}
	}
		
	// Generate foes:
	const int maxFoes = rand1(MAX_FOES);	
	while( maxFoes>this->foes.size()) {
		const int x = rand() % WIDTH;
		const int y = rand() % HEIGHT;
		if( !me.isIn(x,y)
		    && !FieldObject::existsIn(x,y, this->walls)
			&& !FieldObject::existsIn(x,y, this->foes)) {
			this->foes.push_back( *(new FieldObject(x,y)));
		}
	}
}

Field::~Field() {
}

const bool Field::makeTurn(const unsigned int cmd) {
	strcpy(message,"");
	setAttr(FRAME);
	system("cls");	
	if( CMD_EXIT==cmd ) {
		renderField();
		return false;
	}
	if( (CMD_LEFT!=cmd && CMD_RIGHT!=cmd && CMD_UP!=cmd && CMD_DOWN!=cmd && CMD_FIRE!=cmd) 
		|| !makeMyMove(cmd) ) {
		renderField();		
		return true;	
	}
	++turns;
	if(CMD_FIRE==cmd) {
		fire();
	}
	moveFoes();
	if (moveMyBullets()) {
		// your hit
		attr_message = MESSAGE_GOOD;
		if( 0==foes.size() ) {
			strcpy(message, "YOUR VICTORY! :)");
			renderField();
			return false;
		}
		strcpy(message, "YOUR HIT! :)");
	}
	if (moveFoeBullets()) {
		// your wound
		attr_message = MESSAGE_BAD;
		if( 0==lives ) {
			strcpy(message, "YOUR LOSE! :(");
			renderField();
			return false;
		}
		lives--;
		strcpy(message, "YOUR WOUND! :(");
	}	
	
	renderField();
	return true;
}

const bool Field::makeMove(unsigned int *x, unsigned int *y, const int maxFoe, unsigned int cmd) {
	switch( cmd ) {
	case CMD_LEFT:
		if( 0==*x) {
			return false;
		}
		--*x;
		break;
	case CMD_UP:
		if( 0==*y) {
			return false;
		}
		--*y;
		break;			
	case CMD_RIGHT:
		if( WIDTH-1==*x) {
			return false;
		}
		++*x;
		break;
	case CMD_DOWN:
		if( HEIGHT-1==*y) {
			return false;
		}
		++*y;
		break;			
	default:
		return true;
	}
	// Check for walls:
	for( int i=0; i<walls.size(); i++ ) {
		const FieldObject &fo = walls[i];
		if( fo.isIn(*x,*y) ) {
			return false;
		}
	}
	
	// Check for foes:
	for( int i=0; i<maxFoe; i++ ) {
		const FieldObject &fo = foes[i];
		if( fo.isIn(*x,*y) ) {
			return false;
		}
	}	
	
	return true;
}

const bool Field::makeMyMove(unsigned int cmd) {
	unsigned int x = me.getX();
	unsigned int y = me.getY();
	
	if( !makeMove(&x, &y, foes.size(), cmd) ) {
		return false;
	}
	me.setX(x);
	me.setY(y);	
	setLastDirection(me,cmd);
	return true;
}

void Field::moveFoes() {
	for( int i=0; i<foes.size(); i++ ) {
		FieldObject &foe = foes[i];
		unsigned int x = foe.getX();
		unsigned int y = foe.getY();
		unsigned char cmd;
		do {
			cmd = randomCommand();
		} while ( !makeMove(&x,&y, i, cmd));
		if ( CMD_FIRE==cmd) {
			unsigned int x = foe.getX();
			unsigned int y = foe.getY();
			unsigned char lastDir = foe.getLastDirection();
			FieldObject *b = new FieldObject(x,y);
			b->setLastDirection(lastDir);
			foeBullets.push_back(*b);
		} else {		
			foe.setX(x);
			foe.setY(y);	
			setLastDirection(foe,cmd);
		}
	}
}

void Field::fire() {
	unsigned int x = me.getX();
	unsigned int y = me.getY();
	unsigned char lastDir = me.getLastDirection();
	FieldObject *fo = new FieldObject(x,y);
	fo->setLastDirection(lastDir);
	myBullets.push_back(*fo);
}

const bool Field::moveMyBullets() {
	std::vector<FieldObject> newList;
	bool wasHits = false;	
	for( int i=0; i<myBullets.size(); i++ ) {
		FieldObject &bullet = myBullets[i];
		unsigned int x = bullet.getX();
		unsigned int y = bullet.getY();
		bool valid = true;
		switch(bullet.getLastDirection()) {
			case FieldObject::UP:
				if( 0==y ) {
					valid = false;
				} else {
					--y;
				}
				break;
			case FieldObject::DOWN:
				if( HEIGHT-1==y ) {
					valid = false;
				} else {
					++y;
				}
				break;
			case FieldObject::LEFT:
				if( 0==x) {
					valid = false;
				} else {
					--x;
				}
				break;
			case FieldObject::RIGHT:
				if( WIDTH-1==x) {
					valid = false;
				} else {
					++x;
				}
				break;		
		}		
		if( valid && !FieldObject::existsIn(x,y,walls) ) {
			const int foeHitIndex = FieldObject::getIndex(x,y,foes);
			if( -1<foeHitIndex) {
				removeFoe(foeHitIndex);					
				wasHits = true;
			} else {
				bullet.setX(x);
				bullet.setY(y);
				newList.push_back(bullet);
			}
		}
		
	}	
	myBullets = newList;
	return wasHits;
}

void Field::removeFoe(const int n) {
	std::vector<FieldObject> newList;
	for( int i=0;i<foes.size(); i++) {
		if( i!=n) {
			newList.push_back(foes[i]);
		}
	}
	foes = newList;
}

const bool Field::moveFoeBullets() {
	std::vector<FieldObject> newList;
	bool wasHits = false;	
	for( int i=0; i<foeBullets.size(); i++ ) {
		FieldObject &bullet = foeBullets[i];
		unsigned int x = bullet.getX();
		unsigned int y = bullet.getY();
		bool valid = true;
		switch(bullet.getLastDirection()) {
			case FieldObject::UP:
				if( 0==y ) {
					valid = false;
				} else {
					--y;
				}
				break;
			case FieldObject::DOWN:
				if( HEIGHT-1==y ) {
					valid = false;
				} else {
					++y;
				}
				break;
			case FieldObject::LEFT:
				if( 0==x) {
					valid = false;
				} else {
					--x;
				}
				break;
			case FieldObject::RIGHT:
				if( WIDTH-1==x) {
					valid = false;
				} else {
					++x;
				}
				break;		
		}		
		if( valid && !FieldObject::existsIn(x,y,walls) ) {
			if( me.isIn(x,y) ) {
				wasHits = true;
			} else {
				bullet.setX(x);
				bullet.setY(y);
				newList.push_back(bullet);
			}
		}
		
	}	
	myBullets = newList;
	return wasHits;	
}

// ----------------

void Field::renderField() {
	renderTopBorder();
	for( int y=0; y<HEIGHT; y++) {
		renderRow(y);
	}
	renderBottomBorder();
	renderScores();
}

void Field::renderTopBorder() {
	renderHorizontalBorder(C_LT, C_RT);
}

void Field::renderBottomBorder() {
	renderHorizontalBorder(C_LB, C_RB);	
}

void Field::renderHorizontalBorder(unsigned char l, unsigned char r) {
	unsigned char *buffer = new unsigned char[WIDTH+3];
	buffer[WIDTH+2] = '\0';
	buffer[0] = l;
	buffer[WIDTH+1] = r;
	for( int i=1; i<WIDTH+1; i++ ) {
		buffer[i] = C_H;
	}
	setAttr(FRAME);
	printf("%s\n", buffer);
	delete buffer;
}

void Field::renderRow(const int y) {
	setAttr(FRAME);
	printf("%c", C_V);
	for( int x=0; x<WIDTH; x++) {
		const int foeIndex = FieldObject::getIndex(x,y,foes);
		if( FieldObject::existsIn(x,y,walls)) {
			setAttr(WALL);
			printf(" ");
		} else if (this->me.isIn(x,y)) {
			setAttr(ME);
			printf("%c", me.getLastDirection());
		} else if (-1!=foeIndex) {
			setAttr(FOE);			
			printf("%c", foes[foeIndex].getLastDirection());
		} else if ( FieldObject::existsIn(x,y,myBullets)) {
			setAttr(ME);
			printf("%c", BULLET);
		} else if ( FieldObject::existsIn(x,y,foeBullets)) {
			setAttr(FOE);
			printf("%c", BULLET);
		} else {
			setAttr(FRAME);
			printf(" ");			
		}
	}
	setAttr(FRAME);
	printf("%c\n", C_V);	
}

void Field::renderScores() {
	setAttr(FRAME);
	printf("\nTurns: %d. Lives: %d\n", turns, lives);
	if( 0!=strlen(message) ) {
		setAttr(attr_message);
		printf("%s\n", message);
	}
	setAttr(FRAME);
}

void Field::setAttr(const unsigned int attr) {
	SetConsoleTextAttribute(hConsole, attr );
}

void Field::setLastDirection(FieldObject &fo, const unsigned char cmd) {
	switch(cmd) {
	case CMD_LEFT:
		fo.setLastDirection(FieldObject::LEFT); break;
	case CMD_RIGHT:
		fo.setLastDirection(FieldObject::RIGHT); break;
	case CMD_UP:
		fo.setLastDirection(FieldObject::UP); break;
	case CMD_DOWN:
		fo.setLastDirection(FieldObject::DOWN); break;
	}
}

const unsigned char Field::randomCommand() {
	int n = rand() % 5;
	switch(n) {
		case 0:
			return CMD_UP;
		case 1:
			return CMD_LEFT;
		case 2:
			return CMD_RIGHT;
		case 3:
			return CMD_DOWN;
		default:
			return CMD_FIRE;
	}
} 



