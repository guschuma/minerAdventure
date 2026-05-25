#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"

#define MAX_NUMBER_OF_PLAYERS_IN_SCORE 10U
#define SCORE_FILE_NAME "placar.bin"
#define MAX_NAME_SIZE 20

typedef struct type_score
{
	char name[MAX_NAME_SIZE];
	int score;
} TYPE_SCORE;

void write_scores_bin();
void read_scores(TYPE_SCORE *score_list, unsigned long *num_of_names);
void print_scores();
void insert_in_list(TYPE_SCORE new_score);
bool should_be_included_in_list(int score);