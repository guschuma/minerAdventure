#include "mapa.h"
#include "gameObjects.h"
#include "enemy.h"
#include "global.h"
#include "player.h"
#include "scenes.h"
#include <string.h> // For memcpy
#include <stdio.h> /// Necessary to read map
#include <math.h> // For sine wave in life animation

#define X 'Z' // Bloco
#define O ' ' // Nada
#define P 'P' // Player
#define E 'E' // Inimigo normal (snake)
#define A 'A' // Esqueleto
#define F 'F' // Final da fase
#define V 'V' // Espinho
#define I 'I' // Pilar (decorativo)
#define L 'L' // Vida

char mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS] = {};
Vector2 player_starting_position;
Texture2D block_sprite;
Texture2D block_tilemap;
Texture2D wood_pillar;
Texture2D world_border;
Texture2D spike_block;
bool level_life_was_taken = false;

void draw_down_elevator(void);
void draw_up_elevator(void);
bool is_ground_block(char c);
void draw_world_border();
Rectangle construct_rec_at_map_coords(int x, int y);// Retuns global position from map grid coordenates
bool is_block_offscreen(Rectangle rel_object_position);
void draw_oriented_spike(int i, int j); // Draws spike block oriented such that it's stuck to a block beside it, if there is one
void init_map(void){
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			Vector2 block_position = (Vector2){	UNITS_PER_BLOCK * (j + BORDER_SIZE_W),	// x
												UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};		// y};

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
	char temp_mapa[MAP_SIZE_IN_BLOCKS][MAP_SIZE_IN_BLOCKS];
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			temp_mapa[i][j] = mapa[i][j];
			if(mapa[i][j] != 'Z') continue;
			Vector2 block_position = (Vector2){	UNITS_PER_BLOCK * (j + BORDER_SIZE_W),	// x
												UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};	// y};
			bool has_block_above = (i - 1 >= 0					)? mapa[i - 1][j] == 'Z' : true;
			bool has_block_below = (i + 1 <  MAP_SIZE_IN_BLOCKS	)? mapa[i + 1][j] == 'Z' : true;
			bool has_block_left  = (j - 1 >= 0					)? mapa[i][j - 1] == 'Z' : true;
			bool has_block_right = (j + 1 <  MAP_SIZE_IN_BLOCKS	)? mapa[i][j + 1] == 'Z' : true;

			temp_mapa[i][j] = 16 + 8 * has_block_above 
								+ 4 * has_block_below 
								+ 2 * has_block_left 
								+ 1 * has_block_right;
		}
	}
	memcpy(mapa, temp_mapa, MAP_SIZE_IN_BLOCKS * MAP_SIZE_IN_BLOCKS);

	level_life_was_taken = false;
	/*for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(mapa[i][j] != ' ') printf("%3i ", mapa[i][j]);
			else printf("    ", mapa[i][j]);
		}
		printf("\n");
	}*/
}
void draw_map(void){
	draw_up_elevator();
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(mapa[i][j] == ' ') continue;
			Rectangle object_rec = construct_rec_at_map_coords(j, i);
			Rectangle local_object_rec = position_rel_to_camera(object_rec);
			if(is_block_offscreen(local_object_rec)) continue; // Block is offscreen
			if(mapa[i][j] >= 16 && mapa[i][j] <= 31){
				// Is a block
				DrawTexturePro(block_tilemap, (Rectangle){16*(mapa[i][j]-16), 0, 16, 16}, local_object_rec, (Vector2){0, 0}, 0, WHITE);
				continue;
			}
			switch(mapa[i][j]){
				case 'P':
					draw_down_elevator();
					break;
				case 'I':	// Pillar (just decorative)
					DrawTexturePro(wood_pillar, (Rectangle){0, 0, 16, 16}, local_object_rec, (Vector2){0, 0}, 0, WHITE);
					break;
				case 'L':
					if(level_life_was_taken) continue;
					object_rec.y = to_pixel_grid(object_rec.y + 0.25 * UNITS_PER_BLOCK * sinf(2 * in_game_time));
					local_object_rec = position_rel_to_camera(object_rec);
					DrawTexturePro(player_life_sprite, (Rectangle){0, 0, 16, 16}, local_object_rec, (Vector2){0, 0}, 0, WHITE);
					break;
				case 'V': 	// Spike
					draw_oriented_spike(i, j);
					break;
				case 'F': 	// exit elevator has already been at the top of the function (to make chord appear behind everything in map)
					break;
			}
		}
	}
	draw_world_border();
}
bool is_block_offscreen(Rectangle rel_object_position){
	return rel_object_position.x > SCREEN_W +  UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR || rel_object_position.y > SCREEN_H +  UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR|| 
			rel_object_position.x < -2*UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR || rel_object_position.y < -2*UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR;
			
}

float collision_detect_blocks(float *new_velocity, Axis ax, PhysicsBody *b, bool *hit_ground, bool *took_damage, bool body_is_player){
	float new_hitbox_position = (ax == HORIZO)? b->hitbox.x + *new_velocity : b->hitbox.y + *new_velocity;
	*hit_ground = false;
	for(int i = 0; i < MAP_SIZE_IN_BLOCKS; i++){
		for(int j = 0; j < MAP_SIZE_IN_BLOCKS; j++){
			if(!is_ground_block(mapa[i][j]) && mapa[i][j] != 'V' && mapa[i][j] != 'L') continue;

			Vector2 object_position = {UNITS_PER_BLOCK * (j + BORDER_SIZE_W), UNITS_PER_BLOCK * (i + BORDER_SIZE_H)};
			Rectangle object_hitbox = {object_position.x, object_position.y, UNITS_PER_BLOCK, UNITS_PER_BLOCK};

			Rectangle new_hitbox = b->hitbox;
			if(ax == HORIZO) new_hitbox.x = new_hitbox_position;
			if(ax == VERTIC) new_hitbox.y = new_hitbox_position;
			if(!CheckCollisionRecs(object_hitbox, new_hitbox)) continue; 
			// Ocorreu uma colisão entre o bloco e o personagem após se mover
			if(mapa[i][j] == 'V') *took_damage |= true; // hit a spike!
			if(mapa[i][j] == 'L' && body_is_player && !level_life_was_taken) { // player got a life!
				level_life_was_taken = true;
				player.health++;
				player_got_life();
				continue;
			}
			else if(mapa[i][j] == 'L') continue;
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
		if(active_enemy_count == 0) DrawTexturePro(elevator_up_idle, (Rectangle){0, 0, 48, 32}, position_rel_to_camera(end_elevator_rec), (Vector2){0, 0}, 0, WHITE);
		else DrawTexturePro(elevator_up_closed_idle, (Rectangle){0, 0, 48, 32}, position_rel_to_camera(end_elevator_rec), (Vector2){0, 0}, 0, WHITE);
		return;
	}

	// Else is in up elevator cutscene
	Rectangle source_rec = (Rectangle){48 * current_up_elevator_frame, 0, 48, 32};

	DrawTexturePro(elevator_up, source_rec, position_rel_to_camera(end_elevator_rec), (Vector2){0, 0}, 0, WHITE);
}

void draw_world_border(){
	// Goes around the border of map checking for outer blocks to add the map edge texture
	int map_limit = MAP_SIZE_IN_BLOCKS - 1;
	for(int y = 0;y <= map_limit; y++){
		if(mapa[y][0] == ' ' && mapa[y][map_limit] == ' ') continue;
		Rectangle object_rec = construct_rec_at_map_coords(0, y);
		object_rec = position_rel_to_camera(object_rec);
		if(!is_block_offscreen(object_rec) && is_ground_block(mapa[y][0])){
			DrawTexturePro(world_border, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 90, WHITE);
		}

		object_rec = construct_rec_at_map_coords(map_limit + 1, y + 1);
		object_rec = position_rel_to_camera(object_rec);
		if(!is_block_offscreen(object_rec) && is_ground_block(mapa[y][map_limit])){
			DrawTexturePro(world_border, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 270, WHITE);
		}
	}

	for(int x = 0;x <= map_limit; x++){
		if(mapa[0][x] == ' ' && mapa[map_limit][x] == ' ') continue;
		Rectangle object_rec = construct_rec_at_map_coords(x + 1, 0);
		object_rec = position_rel_to_camera(object_rec);
		if(!is_block_offscreen(object_rec) && is_ground_block(mapa[0][x])){
			DrawTexturePro(world_border, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 180, WHITE);
		}

		object_rec = construct_rec_at_map_coords(x, map_limit + 1);
		object_rec = position_rel_to_camera(object_rec);
		if(!is_block_offscreen(object_rec) && is_ground_block(mapa[map_limit][x])){
			DrawTexturePro(world_border, (Rectangle){0, 0, 16, 16}, object_rec, (Vector2){0, 0}, 0, WHITE);
		}
	}

}

Rectangle construct_rec_at_map_coords(int x, int y){
	return (Rectangle){UNITS_PER_BLOCK * (x + BORDER_SIZE_W),  			// x
								UNITS_PER_BLOCK * (y + BORDER_SIZE_H), 	// y
								UNITS_PER_BLOCK,					   				// width
								UNITS_PER_BLOCK};					  		 		// height
}

void draw_oriented_spike(int i, int j){
	Rectangle object_rec = construct_rec_at_map_coords(j, i);
	Rectangle dest = position_rel_to_camera(object_rec);
	float block_rotation;
	int direction = 1;
	if(is_ground_block(mapa[i + 1][j])) {
		block_rotation = 0;
	}
	else if(is_ground_block(mapa[i - 1][j])) {
		block_rotation = 180;
		direction = -1;
	}
	else if(is_ground_block(mapa[i][j + 1])) {
		block_rotation = 270;
		direction = -1;
	}
	else if(is_ground_block(mapa[i][j - 1])) {
		block_rotation = 90;
	}
	else block_rotation = 0;

	Vector2 origin = { dest.width/2, dest.height/2 };
    dest.x += origin.x;
    dest.y += origin.y;
	DrawTexturePro(spike_block, (Rectangle){0, 0, 16 * direction, 16}, dest, origin, block_rotation, WHITE);
}

bool is_ground_block(char c){
	return c >= 16 && c <= 31;
}