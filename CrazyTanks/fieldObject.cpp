#include "fieldObject.h"

FieldObject::FieldObject(const unsigned int x, const unsigned int y) {
	this->x = x;
	this->y = y;
	this->lastDirection = UP;
}

const int FieldObject::getIndex(const unsigned int x, const unsigned int y, const std::vector<FieldObject> &list) {
	for( int i=0; i<list.size(); i++ ) {
		const FieldObject &fo = list[i];
		if( fo.isIn(x,y) ) {
			return i;
		}
	}
	return -1;
}

