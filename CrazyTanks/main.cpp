#include <conio.h>

#include "field.h"

int main(int argc, char** argv) {
	Field field;
	unsigned int cmd = 0;
	while(field.makeTurn(cmd)) {
		cmd = getch();
	}
	return 0;
}
