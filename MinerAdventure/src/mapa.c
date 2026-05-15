#include "mapa.h"
#include "gameObjects.h"
#include "enemy.h"
#include "global.h"
#include "scenes.h"
#include <stdio.h> /// Necessary to read map

#define X 'Z' // Bloco
#define O ' ' // Nada
#define P 'P' // Player
#define E 'E' // Inimigo normal (snake)
#define A 'A' // Esqueleto
#define F 'F' // Final da fase

char mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS] = {};
Vector2 player_starting_position;
Texture2D block_sprite;

void draw_down_elevator(void);
void draw_up_elevator(void);
bool is_block_offscreen(Rectangle rel_object_position);
void init_map(void){
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			Vector2 block_position = (Vector2){UNITS_PER_BLOCK * (j + BORDER_SIZE_W),  // x
														UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};	 // y};

			if(mapa[i][j] == 'E') init_enemy(block_position, ENEMY_SNAKE);
			if(mapa[i][j] == 'A') init_enemy(block_position, ENEMY_SKELETON);

			if(mapa[i][j] == 'P') {
				player_starting_position = block_position;
				player_starting_position.x += 3;
				player_starting_position.y += 2;
				start_elevator_rec = (Rectangle){UNITS_PER_BLOCK * (j + BORDER_SIZE_W - 1),  		// x
													UNITS_PER_BLOCK * (i + BORDER_SIZE_H - 1), 		// y
													UNITS_PER_BLOCK * 3,					   		// width
													UNITS_PER_BLOCK * 2};					   		// height
				
			}

			if(mapa[i][j] == 'F'){
				end_elevator_rec = (Rectangle){UNITS_PER_BLOCK * (j + BORDER_SIZE_W - 1),  		// x
												UNITS_PER_BLOCK * (i + BORDER_SIZE_H - 1), 		// y
												UNITS_PER_BLOCK * 3,					   		// width
												UNITS_PER_BLOCK * 2};					   		// height
			}
		}
	}
}
void draw_map(void){
	draw_up_elevator();
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(mapa[i][j] == ' ') continue;
			Rectangle object_rec = (Rectangle){UNITS_PER_BLOCK * (j + BORDER_SIZE_W),  // x
														UNITS_PER_BLOCK * (i + BORDER_SIZE_H), // y
														UNITS_PER_BLOCK,					   // width
														UNITS_PER_BLOCK};					   // height
			object_rec = position_rel_to_camera(object_rec);
			if(is_block_offscreen(object_rec)) continue; // Block is offscreen
			switch(mapa[i][j]){
				case 'Z':	// bloco
					//object_rec = position_rel_to_camera(object_rec);
					DrawTexturePro(block_sprite, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 0, WHITE);
					break;
				case 'P':
					draw_down_elevator();
					break;
				case 'F': // exit elevator has already been at the top of the function (to make chord appear behind everything in map)
					break;
			}
		}
	}
}
bool is_block_offscreen(Rectangle rel_object_position){
	return rel_object_position.x > SCREEN_W +  UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR || rel_object_position.y > SCREEN_H +  UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR|| 
			rel_object_position.x < -2*UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR || rel_object_position.y < -2*UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR;
			
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

void draw_down_elevator(void){
	// If not in down elevator cutscene, just draw the elevator normally
	if(!in_down_elevator_cutscene) {
		DrawTexturePro(elevator_down_idle, (Rectangle){0, 0, 48, 32}, position_rel_to_camera(start_elevator_rec), (Vector2){0, 0}, 0, WHITE);
		return;
	}

	// Else is in down elevator cutscene
	Rectangle source_rec = (Rectangle){48 * current_down_elevator_frame, 0, 48, 32};

	DrawTexturePro(elevator_down, source_rec, position_rel_to_camera(start_elevator_rec), (Vector2){0, 0}, 0, WHITE);
}

void draw_up_elevator(void){
	// Draw chord
	Rectangle chord_rec = (Rectangle){
				end_elevator_rec.x + UNITS_PER_BLOCK + 7*UNITS_PER_PIXEL, 
				end_elevator_rec.y - UNITS_PER_BLOCK,
				2 * UNITS_PER_PIXEL,
				UNITS_PER_BLOCK
				};
	Rectangle chord_position_rel = position_rel_to_camera(chord_rec);
	DrawRectangleRec(chord_position_rel, GPURPLE);
	while(chord_position_rel.y >  -UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR){
		chord_position_rel.y -= UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR;
		DrawRectangleRec(chord_position_rel, GPURPLE);
	}

	// If not in down elevator cutscene, just draw the elevator normally
	if(!in_up_elevator_cutscene) {
		DrawTexturePro(elevator_up_idle, (Rectangle){0, 0, 48, 32}, position_rel_to_camera(end_elevator_rec), (Vector2){0, 0}, 0, WHITE);
		return;
	}

	// Else is in up elevator cutscene
	Rectangle source_rec = (Rectangle){48 * current_up_elevator_frame, 0, 48, 32};

	DrawTexturePro(elevator_up, source_rec, position_rel_to_camera(end_elevator_rec), (Vector2){0, 0}, 0, WHITE);
}

