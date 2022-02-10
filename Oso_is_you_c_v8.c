#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdbool.h>
#include <conio.h> // _getch		windows
#include <stdlib.h> // system
#include <string.h>  // memcpy
#include "func.h"
//#include <termios.h>   // Linux

// Todo: Change (item_num -> items_created) where posible

char (*map)[height];
int items_created = 0, level = 0, load_counter = 0, loaded = 0, level_star_position[10];
char targets[6] = {'n','H','R','F','T','O'};  // id letter to target the changes

// hedge_txt, hedge, rock_txt, rock, flag_txt, flag, push_txt, stop_txt, win_txt, defeat_txt, tiger_txt, tiger,	 osa_txt, osa, is_txt, you_txt;
//		0		1		2		3		4		5		6		7			8		9			10		11			12		13		14		15
// Prototypes 

void gen_Prototypes() {
	prototypes[0].id = 'h';		//	hedge_txt
	prototypes[0].text_noun = true;
	prototypes[0].push = true;
	prototypes[0].target = 1;

	prototypes[1].id = 'H';		//	hedge

	prototypes[2].id = 'r';		//	rock_txt
	prototypes[2].text_noun = true;
	prototypes[2].push = true;
	prototypes[2].target = 2;

	prototypes[3].id = 'R';		//	rock

	prototypes[4].id = 'f';		//	flag_txt
	prototypes[4].text_noun = true;
	prototypes[4].push = true;
	prototypes[4].target = 3;

	prototypes[5].id = 'F';		//	flag

	prototypes[6].id = 'p';		//	push_txt
	prototypes[6].text_adj = true;
	prototypes[6].push = true;
	prototypes[6].meaning = 1;

	prototypes[7].id = 's';		//	stop_txt
	prototypes[7].text_adj = true;
	prototypes[7].push = true;
	prototypes[7].meaning = 2;

	prototypes[8].id = 'w';		//	win_txt
	prototypes[8].text_adj = true;
	prototypes[8].push = true;
	prototypes[8].meaning = 4;

	prototypes[9].id = 'd';		//	defeat_txt
	prototypes[9].text_adj = true;
	prototypes[9].push = true;
	prototypes[9].meaning = 5;

	prototypes[10].id = 't';		//	tiger_txt
	prototypes[10].text_noun = true;
	prototypes[10].push = true;
	prototypes[10].target = 4;

	prototypes[11].id = 'T';		//	tiger

	prototypes[12].id = 'o';		//	osa_txt
	prototypes[12].text_noun = true;
	prototypes[12].push = true;
	prototypes[12].target = 5;

	prototypes[13].id = 'O';		//	osa

	prototypes[14].id = 'i';		//	is_txt
	prototypes[14].text_filler = true;
	prototypes[14].push = true;

	prototypes[15].id = 'y';		//	you_txt
	prototypes[15].text_adj = true;
	prototypes[15].push = true;
	prototypes[15].meaning = 3;
}
/*				Linux
char getch(void)        // code block _getch alternative
{
	char buf = 0;
	struct termios old = { 0 };
	fflush(stdout);
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	printf("%c\n", buf);
	return buf;
}
*/
void empty_World() {
	//int x = sizeof(map) / sizeof(map[0]), y = sizeof(map[0]) / sizeof(map[0][0]);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			map[i][j] = ' ';
		}
	}
}

void update_World() {
	empty_World();
	for (int i = 0;i < item_num;i++) {
		if (items[i].used == true) {
			map[items[i].x][items[i].y] = items[i].id;
		}
	}
}

void show_World() {
	clearscr();
	update_World();
	//printf("%c%c\n", '\xc2', '\xa9');
	printf("W, A, S, D - move; R - restart; Z - undo; Q - quit;\n \n");
	printf("Lower case letters represent text \nUppercase letters represent objects\n\nH - hedge; R - rock; F - flag; O - oso; T - tiger;");
	printf("\ni - is; p - push; s - stop; y - you; w - win; d - defeat; \n\n\n");
	printf("                   Level:  %i\n",level);
	for (int i = 0;i < height;i++) { printf(" +"); }
	printf("\n");
	for (int i = 0;i < width;i++) {
		printf("+");
		for (int j = 0;j < height;j++) {
			//printf(" ");
			printf("%c", map[i][j]);
			printf(" ");
		}
		printf("+\n");
	}
	for (int i = 0;i < height;i++) { printf(" +"); }
}

void clear_adj() {
	for (int i = 0; i < item_num; i++) {
		if (!items[i].text_noun && !items[i].text_filler && !items[i].text_adj) {
			items[i].push = false;
			items[i].stop = false;
			items[i].you = false;
			items[i].win = false;
			items[i].defeat = false;
		}
	}
}

void scan_Map() {   // Searching for 3 word pairs with correct grammar and aplying their rule
	clear_adj();
	// vertical scan 3x1
	int temp_x, temp_y;
	for (int i = 0;i < item_num;i++) {
		if (items[i].text_noun) {
			for (int j = 0;j < item_num;j++) {  // First word
				if (items[j].text_filler && items[i].x + 1 == items[j].x && items[i].y == items[j].y) { // Second word
					for (int k = 0;k < item_num;k++) {
						if ((items[k].text_noun || items[k].text_adj) && items[i].x + 2 == items[k].x && items[i].y == items[k].y) { // Third word
							if (items[k].text_noun) {						// If third word is a noun
								for (int m = 0; m < item_num;m++) {
									if (items[m].id == targets[items[i].target]) {
										temp_x = items[m].x;			// items[i] should be generalize to the item described		done!
										temp_y = items[m].y;
										for (int n = 0;n < prototype_num; n++) {
											if (prototypes[n].id == targets[items[k].target])
											{
												memcpy(&items[m], &prototypes[n], sizeof(prototypes[n]));   /// todo: put in fuction and generalize
												break;
											}
										}
										items[m].used = true;
										items[m].x = temp_x;
										items[m].y = temp_y;
									}
								}
							}
							else if (items[k].text_adj) {					// If third word is an adj
								//printf("%c", items[k].meaning);
								for (int m = 0; m < item_num;m++) {
									if (items[m].id == targets[items[i].target]) {		// items[i] should be generalize to the item described  // also items[i] changed to		backup[0][i]	because value can be already changed
										if (items[k].meaning == 1) {items[m].push = true;}
										else if (items[k].meaning == 2) { items[m].stop = true; }
										else if (items[k].meaning == 3) { items[m].you = true; }
										else if (items[k].meaning == 4) { items[m].win = true; }
										else if (items[k].meaning == 5) { items[m].defeat = true; }
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// horizontal  scan 1x3
	for (int i = 0;i < item_num;i++) {
		if (items[i].text_noun) {
			for (int j = 0;j < item_num;j++) {  // First word
				if (items[j].text_filler && items[i].x == items[j].x && items[i].y + 1 == items[j].y) { // Second word
					for (int k = 0;k < item_num;k++) {
						if ((items[k].text_noun || items[k].text_adj) && items[i].x == items[k].x && items[i].y + 2 == items[k].y) { // Third word
							if (items[k].text_noun) {						// If third word is a noun
								for (int m = 0; m < item_num;m++) {
									if (items[m].id == targets[items[i].target]) {
										temp_x = items[m].x;			// items[i] should be generalize to the item described		done!
										temp_y = items[m].y;
										for (int n = 0;n < prototype_num; n++) {
											if (prototypes[n].id == targets[items[k].target])
											{
												memcpy(&items[m], &prototypes[n], sizeof(prototypes[n]));   /// todo: put in fuction and generalize
												break;
											}
										}
										items[m].used = true;
										items[m].x = temp_x;
										items[m].y = temp_y;
									}
								}
							}
							else if (items[k].text_adj) {					// If third word is an adj
								//printf("%c", items[k].meaning);
								for (int m = 0; m < item_num;m++) {
									if (items[m].id == targets[items[i].target]) {		// items[i] should be generalize to the item described  // also items[i] changed to		backup[0][i]	because value can be already changed
										if (items[k].meaning == 1) { items[m].push = true; }
										else if (items[k].meaning == 2) { items[m].stop = true; }
										else if (items[k].meaning == 3) { items[m].you = true; }
										else if (items[k].meaning == 4) { items[m].win = true; }
										else if (items[k].meaning == 5) { items[m].defeat = true; }
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void creat_Item(int x, int y, int type) {
	memcpy(&items[items_created], &prototypes[type], sizeof(prototypes[type]));
	items[items_created].used = true;
	items[items_created].x = x;
	items[items_created].y = y;
	items_created++;
}
// hedge_txt, hedge, rock_txt, rock, flag_txt, flag, push_txt, stop_txt, win_txt, defeat_txt, tiger_txt, tiger,	 osa_txt, osa, is_txt, you_txt;
//		0		1		2		3		4		5		6		7			8		9			10		11			12		13		14		15

void creat_World() {
	empty_World();

	//if (loaded_level[load_counter].it == 100) { load_counter = 0; level = 0; }
	
	for (load_counter = level_star_position[level] + 1;loaded_level[load_counter].it != 50 && loaded_level[load_counter].it != 100; load_counter++)
		creat_Item(loaded_level[load_counter].x, loaded_level[load_counter].y, loaded_level[load_counter].it);
	load_counter++;
}

void player_Move(int x, int y) {
	for (int i = back_up_mem - 1;i > 0;i--) {memcpy(&backup[i], &backup[i-1], sizeof(backup[i - 1]));}		// save game state
	memcpy(&backup[0], &items, sizeof(items));

	for (int i = 0; i < item_num;i++) {
		if (items[i].you && items[i].used) {
			if (backup[0][i].x != items[i].x || backup[0][i].y != items[i].y) { break; } // if u already got pushed by other you - pass
			move(x, y, &items[i]); 
		}
	}
	update_World();
}

void go_Back() { // And yes, you can go back before level was created or restarted
	memcpy(&items, &backup[0], sizeof(backup[0]));		// Load game state
	for (int i = 0;i < back_up_mem-1;i++) {memcpy(&backup[i], &backup[i + 1], sizeof(backup[i + 1]));}
}

void on_check() {
	for (int i = 0;i < item_num;i++) {
		for (int j = 0;j < item_num;j++) {
			if (items[i].x == items[j].x && items[i].y == items[j].y) {					// On same location
				if (items[i].you && items[j].defeat) { items[i].used = false; }			// death
				if (items[i].you && items[j].win) { 
					level++;
					memset(backup, 0, sizeof(backup));   // to prevent cheating 
					memset(items, 0, sizeof(items)); // cleaning item list
					game();
					break;
				}							// reset on win
			}
		}
	}
}

int load_levels() {
	FILE* in_file;
	int lev = 0;
	in_file = fopen("levels.txt", "r");
	if (in_file == NULL) return 1;
	while (!feof(in_file)) {
		for (int i = 0;i < max_load_items;i++) {
			if (!feof(in_file)) {
				fscanf(in_file, "%d", &loaded_level[i].x);
				fscanf(in_file, "%d", &loaded_level[i].y);
				fscanf(in_file, "%d", &loaded_level[i].it);
			}
			else break;
			if (loaded_level[i].it == 50) {
				level_star_position[lev] = i;
				lev++;
			}
		}
	}
	fclose(in_file);
	return 2;
}

int game(){
	char player_choice;
	items_created = 0;
	bool quit = false;

	creat_World();
	scan_Map();
	on_check();
	show_World();
	while (1) {
		if (level == 5) { level = 0; game(); break; } // break
		player_choice = _getch();
		//player_choice = getchar(1);
		//player_choice = getc(stdin);
		switch (player_choice) {
		case 'a':
			player_Move(0, -1);
			break;
		case 'w':
			player_Move(-1, 0);
			break;
		case 'd':
			player_Move(0, 1);
			break;
		case 's':
			player_Move(1, 0);
			break;
		case 'z':
			go_Back();
			break;
		case 'r':
			game();
			break;
		case 'q':
			quit = true;
			break;
		default:
			break;
			//continue;
		}
		if (quit)break;
		scan_Map();
		on_check();
		show_World();
	}
	return 0;
}


int main() {
	if (loaded == 0) loaded = load_levels();
	if (loaded == 1) return 1;

	int* item_read = calloc(1, sizeof(int));
	if (item_read == NULL) return 2;

	char (*map_)[height] = malloc(sizeof(char[width][height]));
	if (map_ == NULL) return 3;
	map = map_;
	
	printf("Enter Level number 0 - 4\n \n");
	scanf("%d", item_read);
	if (*item_read >= 0 && *item_read <= 4) {
		level = *item_read;
		gen_Prototypes();
		game();
	}
	free(item_read);
	free(map_);
	return 0;
}