#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"
#include "global.h"
#include "gameObjects.h"


extern char mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS];

void init_map(void);
void draw_map(void);

void read_map(const char *filename);