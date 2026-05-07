#include "raylib.h"
#include "raymath.h"

#include "player.h" // Movimentação e objetos do jogador
#include "mapa.h"   // Geração e interação do mapa
#include "global.h"
#include "enemy.h"

#include <stdio.h>
// Nota para mim mesmo: CMD + Alt + I para Debug
// Ideia: Faça com que a fase só possa ser passada após matar todos os inimigos!
void updateDrawFrame(void);
RenderTexture2D pixelated_screen;
RenderTexture2D pixelated_background;
int main(void) { 
	InitWindow(SCREEN_W, SCREEN_H, "Miner Adventure - G. S. F.");
	SetTargetFPS(60);
 	Shader s = LoadShader(0, TextFormat("resources/bloom.fs"));
	printf("PIXEL_PER_UNITS value: %f\n\n", PIXEL_PER_UNITS);

	load_textures();
	read_map("maps/mapa1.txt");
	restart_enemies();
	restart_projectiles();
	init_map();
	init_player();
	set_camera_position_to(player.body.position);
	load_background();
	HideCursor();
	
	
	
	pixelated_screen = LoadRenderTexture(to_pixel_grid(SCREEN_W * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR), to_pixel_grid(SCREEN_H * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR));
	pixelated_background = LoadRenderTexture(to_pixel_grid(SCREEN_W * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR), to_pixel_grid(SCREEN_H * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR));
	while (!WindowShouldClose()) {
		updateDrawFrame();
	}

	CloseWindow();

	return 0;
}

void updateDrawFrame(void){
	// Event Handling
	update_player_position();
	update_projectile_positions();
	
	for(int i = 0; i < enemy_count; i++){
		update_enemy(&(enemy_list[i]));
		update_player_collision_enemy(&(enemy_list[i]));
		update_enemy_collision_projectile(&enemy_list[i], &hook, -1);

		for(int proj_index = 0;  proj_index < projectile_count; proj_index++){
			update_enemy_collision_projectile(&enemy_list[i], &projectile_list[proj_index], proj_index);
		}
		
	}
	for(int proj_index = 0;  proj_index < projectile_count; proj_index++){
		update_player_collision_projectile(&projectile_list[proj_index], proj_index);
	}
	
	
	Vector2 centralized_player_position = {
		player.body.hitbox.x + player.body.hitbox.width * 0.5,
		player.body.hitbox.y + player.body.hitbox.height * 0.5,
	};

	if(player.direction == LEFT) centralized_player_position.x -= 1*UNITS_PER_BLOCK;
	else centralized_player_position.x += 1*UNITS_PER_BLOCK;
	centralized_player_position.y -= 1*UNITS_PER_BLOCK;
	Vector2 global_mouse = v_camera_to_global(GetMousePosition());;
	centralized_player_position = Vector2Lerp(centralized_player_position, global_mouse, 0.25);
	centralized_player_position = v_to_pixel_grid(centralized_player_position);
	move_camera_position_to(centralized_player_position);
	// Drawing objects
	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
	load_background();
	DrawTexturePro(
		pixelated_background.texture,
		(Rectangle){0,
				0,
				pixelated_background.texture.width,
				-pixelated_background.texture.height},
		(Rectangle){0,
					0,
					SCREEN_W,
					SCREEN_H},
		(Vector2){0, 0},
		0.0f,
		WHITE
	);
	draw_hook_textures();
	draw_projectiles();
	draw_map();
	draw_player();
	for(int i = 0; i < enemy_count; i++){
		draw_enemy(&(enemy_list[i]));
	}
	draw_cursor_texture();
    DrawTexturePro(
		pixelated_screen.texture,
		(Rectangle){0,
				0,
				pixelated_screen.texture.width,
				-pixelated_screen.texture.height},
		(Rectangle){0,
					0,
					SCREEN_W,
					SCREEN_H},
		(Vector2){0, 0},
		0.0f,
		WHITE
	);
	#ifdef DRAW_HITBOXES
	DrawCircleLines(GetMousePosition().x, GetMousePosition().y, 3, WHITE);	// Draw mouse position
	#endif	
	
	

	EndDrawing();
}
