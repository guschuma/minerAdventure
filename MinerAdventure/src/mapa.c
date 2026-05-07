#include "mapa.h"
#include "gameObjects.h"
#include "enemy.h"
#include <stdio.h> /// Necessary to read map

#define X 'Z' // Bloco
#define O ' ' // Nada
#define P 'P' // Player
#define E 'E' // Inimigo normal (snake)
#define A 'A' // Esqueleto

char mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS] = {
	{X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,E,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,X,X,X,X,X,X,X,X,X,X,X,X,X,X,O,X,O,X,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,E,O,O,O,E,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X,X,X,X,X,X,X,X,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,E,O,E,O,O,O,O,O,O,O,O,O,E,O,O,O,O,O,O,O,O,O,O,O,O},
	{X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,O,O,O,X,X,X},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X,O,O,O},
	{O,O,O,O,P,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X,O,O,O,O},
	{X,X,X,X,X,X,X,X,X,X,O,O,O,O,O,O,O,O,O,O,X,O,O,X,X,X,X,X,O,O},
	{X,X,X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X,X,O,O,O,O,O,O,O,O,O},
	{O,O,X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X,X,X,O,O,O,O,O,O,O,O,O},
	{O,O,X,X,X,O,O,O,O,O,O,O,X,X,X,X,X,X,X,X,X,O,O,O,O,O,O,O,O,O},
	{O,O,X,O,O,O,O,O,O,O,O,X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,X,O,O,E,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,X,X,X,X,X,X,X,X,X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O},
	{X,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,X}
};
Vector2 player_starting_position;
Texture2D block_sprite;

void init_map(void){
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			Vector2 block_position = (Vector2){UNITS_PER_BLOCK * (j + BORDER_SIZE_W),  // x
														UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};	 // y};
			if(mapa[i][j] == 'P') player_starting_position = block_position;

			if(mapa[i][j] == 'E') init_enemy(block_position, ENEMY_SNAKE);
			if(mapa[i][j] == 'A') init_enemy(block_position, ENEMY_SKELETON);
		}
	}
}
void draw_map(void){
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(mapa[i][j] == ' ') continue;
			Rectangle object_rec = (Rectangle){UNITS_PER_BLOCK * (j + BORDER_SIZE_W),  // x
														UNITS_PER_BLOCK * (i + BORDER_SIZE_H), // y
														UNITS_PER_BLOCK,					   // width
														UNITS_PER_BLOCK};					   // height
			switch(mapa[i][j]){
				case 'Z':	// bloco
					object_rec = position_rel_to_camera(object_rec);
					DrawTexturePro(block_sprite, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 0, WHITE);
					break;
			}
		}
	}
}

float collision_detect_blocks(float *new_velocity, Axis ax, PhysicsBody *b, bool *hit_ground){
	float new_hitbox_position = (ax == HORIZO)? b->hitbox.x + *new_velocity : b->hitbox.y + *new_velocity;
	*hit_ground = false;
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(mapa[i][j] != 'Z') continue;

			Vector2 object_position = {UNITS_PER_BLOCK * (j + BORDER_SIZE_W), UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};
			Rectangle object_hitbox = {object_position.x, object_position.y, UNITS_PER_BLOCK, UNITS_PER_BLOCK};

			Rectangle new_hitbox = b->hitbox;
			if(ax == HORIZO) new_hitbox.x = new_hitbox_position;
			if(ax == VERTIC) new_hitbox.y = new_hitbox_position;
			if(!CheckCollisionRecs(object_hitbox, new_hitbox)) continue; 
			// Ocorreu uma colisão entre o bloco e o personagem após se mover
			if(*new_velocity > 0) {
				if(ax == HORIZO) { // Lado direito do p colidiu
					new_hitbox_position = object_position.x - b->hitbox.width;
					*new_velocity = 0;
					return new_hitbox_position;
				}
				if(ax == VERTIC) { // Parte de baixo do p colidiu
					new_hitbox_position = object_position.y - b->hitbox.height;
					*new_velocity = 0;
					*hit_ground = true;
					return new_hitbox_position;
				}
			}

			if(*new_velocity < 0) {
				if(ax == HORIZO) { // Lado esquerdo de p colidiu
					new_hitbox_position = object_position.x + UNITS_PER_BLOCK;
					*new_velocity = 0;
					return new_hitbox_position;
				}
				if(ax == VERTIC) { // Topo de p colidiu
					new_hitbox_position = object_position.y + UNITS_PER_BLOCK;
					*new_velocity = 0;
					return new_hitbox_position;
				}
			}
			
		}
	}
	return new_hitbox_position;
}

void read_map(const char *filename) {
	char buffer[MAP_SIZE_IN_BLOCKS + 3];
	int i = 0, j = 0;
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error: Map file could not be opened.\n");
        return;
    }

	while (fgets(buffer, MAP_SIZE_IN_BLOCKS + 3, f)) {
		for(int index = 0; buffer[index] != '\0'; index++){
			if (buffer[index] != '\n' && buffer[index] != '\0' && j < MAP_SIZE_IN_BLOCKS && i < MAP_SIZE_IN_BLOCKS) {
				mapa[i][j] = buffer[index];
				j++;
			}
		}
		j = 0;
		i++;
    }
	
    fclose(f);
}
