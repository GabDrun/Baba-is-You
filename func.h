#ifndef FUNC_H
#define FUNC_H

#define width 18
#define height 24
#define item_num 60
#define prototype_num 16
#define max_load_items 1000
#define back_up_mem 100  // maximum states saved

int game();
void clearscr();
bool move(int x, int y, struct item* obj);

struct item
{
	bool used;
	bool text_noun;
	bool text_filler;
	bool text_adj;
	bool stop;
	bool you;
	bool push;
	bool win;
	bool defeat;
	int meaning;			//   1 - push; 2 - stop; 3 - you; 4 - win; 5 - defeat;
	int target;				// 1 - hedge; 2 - rock; 3 - flag; 4 - tiger; 5 - osa;
	char id;			// wchar_t
	int x;
	int y;
} items[item_num], backup[back_up_mem][item_num], prototypes[prototype_num];


struct level_load
{
	int x;
	int y;
	int it;
}loaded_level[max_load_items];

#endif