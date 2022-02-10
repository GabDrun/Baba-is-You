#include <stdlib.h>
#include <stdbool.h>
#include "func.h"

void clearscr()
{
	system("cls");				// slow but clean
								// witth no screen clearing fast and messy
	//printf("\e[1;1H\e[2J");	//if linux
}

bool move(int x, int y, struct item* obj) {// Trying to move recursivly checking next element in line
	bool can_move = false, taken = false;
	if (obj->x + x < 0 || obj->y + y < 0 || obj->x + x >= width || obj->y + y >= height) { return can_move; }
	else {
		for (int i = 0;i < item_num;i++) {
			if (items[i].x == obj->x + x && items[i].y == obj->y + y) {
				if (items[i].used == false) { continue; }		// skip reserved dor later items  
				if (items[i].push == false) {
					if (items[i].stop) { return can_move; }
				}
				else {
					can_move = move(x, y, &items[i]);
					if (can_move) {
						obj->x += x;
						obj->y += y;
					}
					return can_move;
				}
			}
		}if (taken) { return can_move; }
		else {
			obj->x += x;
			obj->y += y;
			return true;
		}
	}
}