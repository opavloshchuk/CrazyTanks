#pragma once

#include <vector>

class FieldObject {
public:
	static const unsigned char UP = 193;
	static const unsigned char DOWN = 194;
	static const unsigned char LEFT = 180;
	static const unsigned char RIGHT = 195;		
	
	FieldObject(const unsigned int x = 0, const unsigned int y = 0);
	
	inline const unsigned int getX() const { return x; }
	inline void setX(unsigned int x) {
		this->x = x;
	}
	
	inline const unsigned int getY() const { return y; }
	inline void setY(unsigned int y) {
		this->y = y;
	}
	
	inline const unsigned char getLastDirection() const { return lastDirection; }
	void setLastDirection(unsigned char lastDirection) {
		this->lastDirection = lastDirection;
	}
		
	static const int getIndex(const unsigned int x, const unsigned int y, const std::vector<FieldObject> &list);
	
	static const bool existsIn(const unsigned int x, const unsigned int y, const std::vector<FieldObject> &list) {
		return -1!=getIndex(x,y,list);
	}
	
	inline const bool isIn(const unsigned int x, const unsigned int y) const {
		return this->x==x && this->y==y;	
	}
	
private:
		
	unsigned int x;
	unsigned int y;		
	unsigned char lastDirection;
};
