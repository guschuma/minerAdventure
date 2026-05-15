#include "raylib.h"
#include "raymath.h"

#include "player.h" // Movimentação e objetos do jogador
#include "mapa.h"   // Geração e interação do mapa
#include "global.h"
#include "enemy.h"
#include "scenes.h"
#include "score.h"

#include <stdio.h>
// Nota para mim mesmo: CMD + Alt + I para Debug
// Ideia: Faça com que a fase só possa ser passada após matar todos os inimigos!
void update_gameplay_loop(void);
void event_handling(void);

RenderTexture2D pixelated_screen;
RenderTexture2D pixelated_background;
int main(void) { 
	InitWindow(SCREEN_W, SCREEN_H, "Miner Adventure - G. S. F.");
	SetTargetFPS(60);
	printf("PIXEL_PER_UNITS value: %f\n\n", PIXEL_PER_UNITS);

	load_textures();
	read_map("maps/mapa1.txt");
	restart_enemies();
	restart_entire_level();
	player.health = PLAYER_HEALTH;
	set_camera_position_to(player.body.position);
	load_background();
	HideCursor();

	current_screen = START_SCREEN;
	
	pixelated_screen = LoadRenderTexture(to_pixel_grid(SCREEN_W * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR), to_pixel_grid(SCREEN_H * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR));
	pixelated_background = LoadRenderTexture(to_pixel_grid(SCREEN_W * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR), to_pixel_grid(SCREEN_H * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR));
	while (!WindowShouldClose()) {
		switch(current_screen){
		case PAUSE_SCREEN:
			on_pause_screen();
			break;
			
		case GAMEPLAY_SCREEN:
			in_game_time += GetFrameTime();
			update_gameplay_loop();
			if(IsKeyPressed(KEY_TAB)){
				on_pause_screen();
			}
			else is_on_pause_screen = false;
			break;

		case DEFEAT_SCREEN:
			on_defeated_screen();
			break;
		
		case START_SCREEN:
			on_start_screen();
			break;

		case SCORE_SCREEN:
			on_score_screen();
			break;
		}
	}

	CloseWindow();

	return 0;
}

void update_gameplay_loop(void){
	// Clearing pixelated screen
	BeginTextureMode(pixelated_screen);
    ClearBackground(BLANK);
	EndTextureMode();
	// Event Handling and updating position
	#ifdef DEBUG_ACTIVE
	if(IsKeyPressed(KEY_R)){
		enter_level_cutscene();
		restart_enemies();
		init_map();
		player.health = PLAYER_HEALTH;
		init_player();
		restart_projectiles();
	}
	if(IsKeyPressed(KEY_K)){
		player.health = 0;
	}
	#endif

	if(player.health == 0) defeated_cutscene();
	if(!is_frame_frozen){
		event_handling();
	}
	else{
		if(in_down_elevator_cutscene) enter_level_cutscene();
		if(in_up_elevator_cutscene) exit_level_cutscene();
	}
	camera_handling();
	// Drawing objects	
	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
	load_background();
	draw_textured_screen(pixelated_background);
	if(!is_frame_frozen) draw_hook_textures();
	if(!is_frame_frozen) draw_projectiles();
	draw_map();
	if(!in_down_elevator_cutscene && !in_up_elevator_cutscene) draw_player();
	draw_lifes(player.health);
	for(int i = 0; i < enemy_count; i++){
		draw_enemy(&(enemy_list[i]));
	}
	BeginTextureMode(pixelated_screen);
	const char *score_text = TextFormat("%i", score);
	int text_width = MeasureText(score_text, 10 * UNITS_PER_PIXEL);
	DrawText(score_text, UNITS_PER_PIXEL * to_pixel_grid(0.99 * SCREEN_W / CAMERA_ZOOM_FACTOR - text_width * UNITS_PER_PIXEL), UNITS_PER_PIXEL * to_pixel_grid(0.01 * SCREEN_H * PIXEL_PER_UNITS/ CAMERA_ZOOM_FACTOR), 10 * UNITS_PER_PIXEL, score_text_color);
	EndTextureMode();
	draw_cursor_texture();
	draw_textured_screen(pixelated_screen);

	#ifdef DRAW_HITBOXES
	DrawCircleLines(GetMousePosition().x, GetMousePosition().y, 3, WHITE);	// Draw mouse position
	#endif	
	
	EndDrawing();
}

void event_handling(void){
	score_text_color = GPINK;
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
}