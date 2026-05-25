#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"
#include "global.h"
#include "gameObjects.h"


extern char mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS];
extern bool level_life_was_taken; // Only one life power-up per level is allowed!
extern RenderTexture2D shadows;

void init_map(void);
void draw_map(void);
void read_map(const char *filename);
void print_map();
void transform_zs_into_tilemap(char mapa[][MAP_SIZE_IN_BLOCKS]);