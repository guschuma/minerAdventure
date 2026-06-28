#include "scenes.h"
#include "gameObjects.h"
#include "raymath.h"
#include "score.h"
#include <stdio.h>
#include "sounds.h"
// Cutscenes are player entering, exiting or being defeated
#define INITIAL_PLAYER_HEALTH 3


#ifdef DEBUG_ACTIVE
#define START_MENU_ANIMATION_DURATION 0.001 // in seconds
#define CUTSCENE_DURATION 0.001       // in seconds
#else
#define CUTSCENE_DURATION 1.7       // in seconds 1.7
#define START_MENU_ANIMATION_DURATION 3 // in seconds 3
#endif

#define FONT_SIZE (28/CAMERA_ZOOM_FACTOR)
#define UP_ELEVATOR_ANIMATION_FRAME_AMOUNT 13
#define DOWN_ELEVATOR_ANIMATION_FRAME_AMOUNT 13
#define NUMBER_OF_LEVELS 8

SCREENS current_screen;

bool in_up_elevator_cutscene = false;
bool in_down_elevator_cutscene = false;
bool in_defeat_cutscene = false;

bool is_frame_frozen = false;
int current_up_elevator_frame = 0;
int current_down_elevator_frame = 0;
bool is_on_pause_screen = false;
bool is_on_defeated_screen = false;

bool mouse_is_on_button = false; // Used to play the select button sound
bool mouse_was_on_button = false;

Texture2D defeat_screen;
Texture2D elevator_up;
Texture2D elevator_up_idle;
Texture2D elevator_up_closed_idle;
Texture2D elevator_down;
Texture2D elevator_down_idle;
Rectangle end_elevator_rec;
Rectangle start_elevator_rec;

Texture2D layer1_menu;
Texture2D layer2_menu;
Texture2D layer3_menu;
Texture2D layer4_menu;
Texture2D layer5_menu;
Texture2D layer6_menu;
Texture2D layer7_menu;
int level = 1;
char name[MAX_NAME_SIZE];
int letterCount = 0;

typedef struct {
	Rectangle box;
	char text[MAX_NAME_SIZE];
	Color inside_color, border_color, inside_color_when_selected, text_color, shadow_color;
	int font_size;
} BUTTON;

bool draw_and_check_button(BUTTON button);
void default_button(BUTTON *b, float vertical_pos_as_perc);
// Used in pause screen
void default_blue(BUTTON *b, float vertical_pos_as_perc);
int check_button_click(BUTTON button);
// Time since the last cutscence animation frame updating
double time_since_last_cutscene_change = 0;
double time_since_cutscene_started = 0;

Vector2 get_rel_middle_of_player();
void draw_circle_with_player_in_center(float radius_size, Vector2 rel_center_pos);

void draw_score_at(int score_number, TYPE_SCORE *score_list, Color text_color, POS_ON_SCREEN pos_text, float vertical_position, int font_size);

typedef enum {USER_STILL_TYPING, USER_HAS_STOPPED_TYPING} TEXT_STATUS;
TEXT_STATUS get_name(char *name);

void enter_level_cutscene(void){
	if(!in_down_elevator_cutscene) {
		time_since_last_cutscene_change = in_game_time; // Animation has started
		time_since_cutscene_started = in_game_time;
		is_frame_frozen = true;
		in_down_elevator_cutscene = true;
		current_down_elevator_frame = 0;
		set_camera_position_to(player_starting_position);
		update_body_position(player_starting_position, &player.body);

		play_sound(DOWN_ELEVATOR);
		play_sound(TRANSITION_UP_SFX);
	}

	if(in_game_time - time_since_last_cutscene_change > CUTSCENE_DURATION / DOWN_ELEVATOR_ANIMATION_FRAME_AMOUNT){
		current_down_elevator_frame++;
		time_since_last_cutscene_change = in_game_time;
	}
	if(current_down_elevator_frame == DOWN_ELEVATOR_ANIMATION_FRAME_AMOUNT){
		// Cutscene has ended
		is_frame_frozen = false;
		in_down_elevator_cutscene = false;
		restart_entire_level();
	}
	else draw_circle_with_player_in_center((PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR) * SCREEN_W * ((in_game_time - time_since_cutscene_started)/CUTSCENE_DURATION), get_rel_middle_of_player());
}

void exit_level_cutscene(void){
	if(!in_up_elevator_cutscene) {
		time_since_last_cutscene_change = in_game_time; // Animation has started
		time_since_cutscene_started = in_game_time;
		is_frame_frozen = true;
		in_up_elevator_cutscene = true;
		update_body_position((Vector2){end_elevator_rec.x + UNITS_PER_BLOCK, end_elevator_rec.y + UNITS_PER_BLOCK}, &player.body); // Make player centered at elevator
		current_up_elevator_frame = 0;
		play_sound(UP_ELEVATOR);
		play_sound(TRANSITION_DOWN_SFX);
	}

	if(in_game_time - time_since_last_cutscene_change > CUTSCENE_DURATION / UP_ELEVATOR_ANIMATION_FRAME_AMOUNT){
		current_up_elevator_frame++;
		time_since_last_cutscene_change = in_game_time;
	}
	if(current_up_elevator_frame == UP_ELEVATOR_ANIMATION_FRAME_AMOUNT){
		// Cutscene has ended
		is_frame_frozen = false;
		in_up_elevator_cutscene = false;
		
		level %= NUMBER_OF_LEVELS;
		level++;
		read_map(TextFormat("maps/mapa%i.txt", level));
		restart_entire_level();
		enter_level_cutscene();
	}
	else draw_circle_with_player_in_center((PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR) * SCREEN_W * (1 - (in_game_time - time_since_cutscene_started)/CUTSCENE_DURATION), get_rel_middle_of_player());
}

Vector2 get_rel_middle_of_player(){
	Vector2 middle_of_player = {player.body.hitbox.x + player.body.hitbox.width * 0.5, player.body.hitbox.y + player.body.hitbox.height * 0.5};
	Vector2 middle_of_player_rel = v_position_rel_to_camera(middle_of_player);
	middle_of_player_rel.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	middle_of_player_rel.y *= PIXEL_PER_UNITS /CAMERA_ZOOM_FACTOR;
	return middle_of_player_rel;
}

void draw_circle_with_player_in_center(float radius_size, Vector2 rel_center_pos){
	// Circle transition
	radius_size *= radius_size / ((PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR) * SCREEN_W); // Makes it more smooth and slower (quadratic)
	BeginTextureMode(pixelated_screen);
	ClearBackground(VERY_VERY_DARK_PURPLE);
	
	BeginBlendMode(BLEND_SUBTRACT_COLORS);
	DrawCircleV(rel_center_pos, radius_size, WHITE);
	EndBlendMode();
	EndTextureMode();
}

// Defeat animation, right before the defeat screen
void defeated_cutscene(void){
	if(!in_defeat_cutscene) {
		play_sound(TRANSITION_DOWN_SFX);
		in_defeat_cutscene = true;
		is_frame_frozen = true;
		time_since_cutscene_started = in_game_time;
	}
	draw_circle_with_player_in_center((PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR) * SCREEN_W * (1 - (in_game_time - time_since_cutscene_started)/CUTSCENE_DURATION), get_rel_middle_of_player());
	if(in_game_time - time_since_cutscene_started > CUTSCENE_DURATION) {
		in_defeat_cutscene = false;
		current_screen = DEFEAT_SCREEN;
	}
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//												PAUSE SCREEN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void on_pause_screen(){
	BeginTextureMode(pixelated_screen);
	ClearBackground(BLANK);
	EndTextureMode();
	is_on_pause_screen = true;
	current_screen = PAUSE_SCREEN;

	camera_handling();
	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
   
	
	load_background();
	draw_textured_screen(pixelated_background, false);

	mouse_was_on_button = mouse_is_on_button;
	mouse_is_on_button = false;
	write_text_aligned("< Pausado >", SCREEN_H * 0.2, FONT_SIZE, WHITE, CENTERED);
	BUTTON continue_b;
	default_blue(&continue_b, 0.50);
	TextCopy(continue_b.text, "Continuar");
	mouse_is_on_button |= draw_and_check_button(continue_b);

	BUTTON menu_b;
	default_blue(&menu_b, 0.65);
	TextCopy(menu_b.text, "Voltar ao menu");
	mouse_is_on_button |= draw_and_check_button(menu_b);

	BUTTON exit_b;
	default_blue(&exit_b, 0.8);
	TextCopy(exit_b.text, "Sair");
	mouse_is_on_button |= draw_and_check_button(exit_b);

	if(mouse_is_on_button && !mouse_was_on_button) play_sound(BUTTON_SELECT);
	draw_cursor_texture();
	draw_textured_screen(pixelated_screen, false);
	
	EndDrawing();
	// Clicking in button 
	bool pause_pressed = IsKeyPressed(KEY_TAB);
	if(check_button_click(continue_b) || pause_pressed) {
		current_screen = GAMEPLAY_SCREEN;
		hook.is_active = false;
	}
	if(check_button_click(menu_b)) current_screen = START_SCREEN;
	if(check_button_click(exit_b)) should_quit = true;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//												DEFEAT SCREEN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
double time_since_last_blinking_cursor;
bool show_cursor;

bool should_write_name = false;		// True if player score is good enough to go to score list
void on_defeated_screen(){
	if(!is_on_defeated_screen){
		is_on_defeated_screen = true;
		current_screen = DEFEAT_SCREEN;
		//write_scores_bin();
		should_write_name = should_be_included_in_list(score);
	}
	bool will_go_to_menu = false;
	if(should_write_name){
		TEXT_STATUS t_status = get_name(name);
		if(t_status == USER_HAS_STOPPED_TYPING){
			TYPE_SCORE new_score = {0};
			TextCopy(new_score.name, name);
			new_score.score = score;
			score = 0;
			insert_in_list(new_score);
			will_go_to_menu = true;
		}
	}

	BeginTextureMode(pixelated_screen);
	ClearBackground(BLANK);
	DrawTexture(defeat_screen, to_pixel_grid(SCREEN_W * 0.5 * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR - 40) , to_pixel_grid(SCREEN_H * PIXEL_PER_UNITS * 0.4/ CAMERA_ZOOM_FACTOR - 32), WHITE);
	EndTextureMode();

	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
	write_text_aligned("< Fim de Jogo! >", SCREEN_H * 0.04, FONT_SIZE, WHITE, CENTERED);
	
	if(should_write_name){
		write_text_aligned("Boa pontuação!", SCREEN_H * 0.68, FONT_SIZE, WHITE, CENTERED);
		write_text_aligned("Escreva seu nome:", SCREEN_H * 0.78, FONT_SIZE, WHITE, CENTERED);
		if(GetTime() - time_since_last_blinking_cursor > 0.5) {
			show_cursor = !show_cursor;
			time_since_last_blinking_cursor = GetTime();
		}
		char display_name[MAX_NAME_SIZE + 2];

		TextCopy(display_name, name);
		int display_letter_count = letterCount;

		if(letterCount == MAX_NAME_SIZE - 1) display_letter_count = letterCount;
		else if(show_cursor)
			TextAppend(display_name, "_", &display_letter_count);
		else
			TextAppend(display_name, " ", &display_letter_count);

		write_text_aligned(display_name, to_pixel_grid(SCREEN_H * 0.88), FONT_SIZE, WHITE, CENTERED);
		
	}
	else {
		write_text_aligned("\"Essa foi por pouco!\"", SCREEN_H * 0.68, FONT_SIZE, GBLUE, CENTERED);
		//write_text_aligned("", SCREEN_H * 0.FONT_SIZE8, 10, WHITE);
		BUTTON b;
		
		default_button(&b, 0.8);

		TextCopy(b.text, "Voltar ao menu");
		mouse_was_on_button = mouse_is_on_button;
		mouse_is_on_button = draw_and_check_button(b);

		if(mouse_is_on_button && !mouse_was_on_button) play_sound(BUTTON_SELECT);
		if(check_button_click(b) || IsKeyPressed(KEY_ENTER) ) will_go_to_menu = true;
	}
	
	draw_cursor_texture();
	draw_textured_screen(pixelated_screen, false);

	EndDrawing();

	if(will_go_to_menu){
		is_on_defeated_screen = false;
		#ifndef DONT_RESTART_WHEN_DEATH
		level = 1;
		read_map(TextFormat("maps/mapa%i.txt", level));
		#endif
		current_screen = START_SCREEN;
		will_go_to_menu = false;
	}
}

void write_text_aligned(char *string, float vertical_pos, int font_size, Color color, POS_ON_SCREEN pos){
	BeginTextureMode(pixelated_screen);
	int text_width = MeasureText(string, font_size * PIXEL_PER_UNITS);
	float horizontal_position = SCREEN_W * 0.5;

	switch(pos){
		case RIGHT_ALIGNED: horizontal_position = SCREEN_W  - CAMERA_ZOOM_FACTOR* UNITS_PER_PIXEL * text_width - 2 * UNITS_PER_PIXEL * font_size; break;
		case CENTERED     : horizontal_position -= 0.5 * text_width * UNITS_PER_PIXEL * CAMERA_ZOOM_FACTOR; break;
		case LEFT_ALIGNED : horizontal_position = 2 * UNITS_PER_PIXEL * font_size; break; // Font_size acts as padding on left and right alignment
	}
	DrawText(string, 
		to_pixel_grid(PIXEL_PER_UNITS * horizontal_position / CAMERA_ZOOM_FACTOR), 
		to_pixel_grid(PIXEL_PER_UNITS * vertical_pos / CAMERA_ZOOM_FACTOR), 
		font_size * PIXEL_PER_UNITS, 
		color);
	EndTextureMode();
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//												MENU SCREEN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
double time_when_entered_start_screen = 0;
bool is_on_start_screen = false;
bool on_start_gameplay_animation = false;
double time_when_start_screen_exit_animation = 0;
void on_start_screen(){
	if(!is_on_start_screen){
		time_when_entered_start_screen = GetTime();
		is_on_start_screen = true;
		current_screen = START_SCREEN;
	}
	BeginTextureMode(pixelated_screen);
	ClearBackground(BLANK);
	EndTextureMode();

	BeginTextureMode(pixelated_background);
	
	// These variables make images "pop" into screen when menu is open. 
	float time_since_start_screen_open = GetTime() - time_when_entered_start_screen;
	float time_playing_animation = START_MENU_ANIMATION_DURATION - time_since_start_screen_open;
	float scalar_offset_animation = (time_since_start_screen_open > START_MENU_ANIMATION_DURATION)? 1 : 
			1 + 10 * (time_playing_animation) *  (time_playing_animation) ; // quadratic for smoothness

	float image_zoom_animation = (time_since_start_screen_open > START_MENU_ANIMATION_DURATION)? 1 : 
			1 + 1 * (time_playing_animation) *  (time_playing_animation) ; 
			
	ClearBackground(BLANK);
	float paralax_scale = 0.006 * scalar_offset_animation;
	draw_offset_menu(layer2_menu, paralax_scale * 1, 1.2* image_zoom_animation, true);	// Dither
	draw_offset_menu(layer3_menu, paralax_scale * 1.5, 1.2* image_zoom_animation, true);// Background mountains
	draw_offset_menu(layer4_menu, paralax_scale * 4, 1 * image_zoom_animation, false); 	// Skeleton
	draw_offset_menu(layer5_menu, paralax_scale * 5, 1 * image_zoom_animation, false);	// Snake
	draw_offset_menu(layer6_menu, paralax_scale * 6, 1 * image_zoom_animation, false);	// Title
	draw_offset_menu(layer7_menu, paralax_scale * 7, 1 * image_zoom_animation, false); 	// Player
	draw_offset_menu(layer1_menu, paralax_scale * 2, 1.1 * image_zoom_animation, false);	// Foreground rocks
	
	EndTextureMode();

	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
	draw_textured_screen(pixelated_background, false);

	mouse_was_on_button = mouse_is_on_button;
	mouse_is_on_button = false;

	BUTTON start_b;	
	default_button(&start_b, 0.50 * image_zoom_animation);
	TextCopy(start_b.text, "Começar");
	mouse_is_on_button |= draw_and_check_button(start_b);

	BUTTON scores_b;	
	default_button(&scores_b, 0.65* image_zoom_animation);
	TextCopy(scores_b.text, "Pontuações");
	mouse_is_on_button |= draw_and_check_button(scores_b);

	BUTTON exit_b;	
	default_button(&exit_b, 0.8* image_zoom_animation);
	TextCopy(exit_b.text, "Sair");
	mouse_is_on_button |= draw_and_check_button(exit_b);

	if(mouse_is_on_button && !mouse_was_on_button && !on_start_gameplay_animation) play_sound(BUTTON_SELECT);

	Vector2 mouse_pos = GetMousePosition();
	mouse_pos.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	mouse_pos.y *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	float time_of_screen_exit_animation = GetTime() - time_when_start_screen_exit_animation;
	float circle_radius = PIXEL_PER_UNITS * SCREEN_W / CAMERA_ZOOM_FACTOR * (1 - time_of_screen_exit_animation / CUTSCENE_DURATION);
	if(on_start_gameplay_animation) draw_circle_with_player_in_center(circle_radius, mouse_pos);
	draw_cursor_texture();
	draw_textured_screen(pixelated_screen, false);
	
	EndDrawing();

	if(on_start_gameplay_animation){
		if(time_of_screen_exit_animation > CUTSCENE_DURATION) {
			score = 0;
			player.health = INITIAL_PLAYER_HEALTH;
			current_screen = GAMEPLAY_SCREEN;
			is_on_start_screen = false;
			restart_entire_level();
			enter_level_cutscene();
			on_start_gameplay_animation = false;
		}
		
	}
	else if(check_button_click(start_b)) {
		play_sound(TRANSITION_DOWN_SFX);
		on_start_gameplay_animation = true;
		time_when_start_screen_exit_animation = GetTime();
	}
	else if(check_button_click(scores_b)) {
		current_screen = SCORE_SCREEN;
		is_on_start_screen = false;
	}
	else if(check_button_click(exit_b)) should_quit = true;
}
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//												SCORE SCREEN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool is_on_score_screen = false;
double time_when_entered_score = 0;
TYPE_SCORE score_list[MAX_NUMBER_OF_PLAYERS_IN_SCORE];
unsigned long num_of_names;
void on_score_screen(){
	if(!is_on_score_screen){
		read_scores(score_list, &num_of_names);
		is_on_score_screen = true;
		current_screen = SCORE_SCREEN;
		time_when_entered_score = GetTime();
	}
	BeginTextureMode(pixelated_screen);
	ClearBackground(BLANK);
	EndTextureMode();

	BeginTextureMode(pixelated_background);
	// These variables make images "pop" back from screen is open. 
	float time_since_score_screen_open = GetTime() - time_when_entered_score;
	float scalar_offset_animation = (time_since_score_screen_open > START_MENU_ANIMATION_DURATION)? 0 : 
			1 + 1 * (time_since_score_screen_open) *  (time_since_score_screen_open) ; // quadratic for smoothness

	float image_zoom_animation = (time_since_score_screen_open > START_MENU_ANIMATION_DURATION)? 0 : 
			1 + 1 * (time_since_score_screen_open) *  (time_since_score_screen_open) ; 
			
	ClearBackground(BLANK);
	float paralax_scale = 0.006 * scalar_offset_animation;
	draw_offset_menu(layer2_menu, paralax_scale * 1, 1.2* image_zoom_animation, true);	// Dither
	draw_offset_menu(layer3_menu, paralax_scale * 1.5, 1.2* image_zoom_animation, true);// Background mountains
	draw_offset_menu(layer4_menu, paralax_scale * 4, 1 * image_zoom_animation, false); 	// Skeleton
	draw_offset_menu(layer5_menu, paralax_scale * 5, 1 * image_zoom_animation, false);	// Snake
	draw_offset_menu(layer6_menu, paralax_scale * 6, 1 * image_zoom_animation, false);	// Title
	draw_offset_menu(layer7_menu, paralax_scale * 7, 1 * image_zoom_animation, false); 	// Player
	draw_offset_menu(layer1_menu, paralax_scale * 2, 1.1 * image_zoom_animation, false);	// Foreground rocks
	
	EndTextureMode();

	BeginDrawing();
	ClearBackground(VERY_VERY_DARK_PURPLE);
	draw_textured_screen(pixelated_background, false);
	
	mouse_was_on_button = mouse_is_on_button;
	mouse_is_on_button = false;
	BUTTON back_b;	
	default_button(&back_b, 0.8);
	TextCopy(back_b.text, "Voltar");
	mouse_is_on_button |= draw_and_check_button(back_b);

	if(mouse_is_on_button && !mouse_was_on_button) play_sound(BUTTON_SELECT);

	float score_popup = START_MENU_ANIMATION_DURATION * 0.6 - time_since_score_screen_open;
	score_popup = (score_popup <= 0)? 0 : score_popup;
	float initial_score_position = SCREEN_H * 0.05 - 1000 * score_popup * score_popup;
	float score_f_size = FONT_SIZE;
	float score_line_size =(2.0/CAMERA_ZOOM_FACTOR) * SCREEN_H / FONT_SIZE;


	draw_score_at(0, score_list, GOLD, LEFT_ALIGNED , initial_score_position, score_f_size);
	draw_score_at(5, score_list, WHITE, RIGHT_ALIGNED, initial_score_position + 1 * score_line_size, score_f_size);
	draw_score_at(1, score_list, GRAY, LEFT_ALIGNED, initial_score_position   + 2 * score_line_size, score_f_size);
	draw_score_at(6, score_list, WHITE, RIGHT_ALIGNED, initial_score_position + 3 * score_line_size, score_f_size);
	draw_score_at(2, score_list, BROWN, LEFT_ALIGNED, initial_score_position  + 4 * score_line_size, score_f_size);
	draw_score_at(7, score_list, WHITE, RIGHT_ALIGNED, initial_score_position + 5 * score_line_size, score_f_size);
	draw_score_at(3, score_list, WHITE, LEFT_ALIGNED, initial_score_position  + 6 * score_line_size, score_f_size);
	draw_score_at(8, score_list, WHITE, RIGHT_ALIGNED, initial_score_position + 7 * score_line_size, score_f_size);
	draw_score_at(4, score_list, WHITE, LEFT_ALIGNED, initial_score_position  + 8 * score_line_size, score_f_size);
	draw_score_at(9, score_list, WHITE, RIGHT_ALIGNED, initial_score_position + 9 * score_line_size, score_f_size);


	draw_cursor_texture();

	draw_textured_screen(pixelated_screen, false);
	
	EndDrawing();

	if(check_button_click(back_b)) {
		current_screen = START_SCREEN;
		is_on_score_screen = false;	
	}
}

TEXT_STATUS get_name(char *name){
	int key = GetCharPressed();

	while (key > 0)
	{
		// Only allow keys in range [32..125]
		if ((key >= 32) && (key <= 125) && (letterCount < MAX_NAME_SIZE - 1))
		{
			play_sound(BUTTON_SELECT);
			name[letterCount] = (char)key;
			name[letterCount+1] = '\0'; 
			letterCount++;
		}

		key = GetCharPressed(); 
	}

	if (IsKeyPressed(KEY_BACKSPACE))
	{
		play_sound(BUTTON_SELECT);
		letterCount--;
		if (letterCount < 0) letterCount = 0;
		name[letterCount] = '\0';
	}

	if(IsKeyPressed(KEY_ENTER)) return USER_HAS_STOPPED_TYPING;
	return USER_STILL_TYPING;
}

bool draw_and_check_button(BUTTON button){
	BeginTextureMode(pixelated_screen);
	char text[MAX_NAME_SIZE];
	TextCopy(text, button.text);
	int text_width = MeasureText(text, button.font_size * UNITS_PER_PIXEL);

	Rectangle box_in_grid = button.box;
	box_in_grid.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	box_in_grid.y *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	
	DrawRectangle(box_in_grid.x + 2, box_in_grid.y + 2, box_in_grid.width, box_in_grid.height, button.shadow_color); // Draw a "shadow"
	
	Vector2 mouse_position = (Vector2){GetMousePosition().x * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR, GetMousePosition().y  * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR};
	if(CheckCollisionPointRec(mouse_position, box_in_grid)) // Fills in button based on if mouse is over it
		DrawRectangle(box_in_grid.x, box_in_grid.y, box_in_grid.width, box_in_grid.height, button.inside_color_when_selected);
	else 
		DrawRectangleGradientV(box_in_grid.x, box_in_grid.y, box_in_grid.width, box_in_grid.height, button.inside_color_when_selected, button.inside_color);
	
	box_in_grid.y -= 0.5; // If don't do this, border sometimes appears misaligned
	DrawRectangleLinesEx(box_in_grid, 1, button.border_color); // Draw border
	DrawText(button.text, 
			to_pixel_grid(box_in_grid.x + 0.5 * box_in_grid.width - 0.5 * text_width), 
			to_pixel_grid(box_in_grid.y + 0.5 * box_in_grid.height - button.font_size * UNITS_PER_PIXEL + 2 * UNITS_PER_PIXEL), 
			button.font_size, 
			button.text_color); // Draw text

	EndTextureMode();

	return CheckCollisionPointRec(mouse_position, box_in_grid);
}

int check_button_click(BUTTON button){
	Rectangle box_in_grid = button.box;
	box_in_grid.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	box_in_grid.y *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;

	Vector2 mouse_position = (Vector2){GetMousePosition().x * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR, GetMousePosition().y * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR};
	return CheckCollisionPointRec(mouse_position, box_in_grid) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void default_button(BUTTON *b, float vertical_pos_as_perc){
	float b_width = SCREEN_W * 0.4;
	float b_height = SCREEN_H * 0.1;
	b->box = (Rectangle){(SCREEN_W * 0.5 - 0.5 * b_width), 
						SCREEN_H * vertical_pos_as_perc, 
						b_width * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR, 
						b_height * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR};
	b->font_size = FONT_SIZE;
	b->text_color = WINE;
	b->inside_color = GPINK;
	b->inside_color_when_selected = LIGHT_PINK;
	b->border_color = DARK_PINK;
	b->shadow_color = WINE;
}

void default_blue(BUTTON *b, float vertical_pos_as_perc){
	float b_width = SCREEN_W * 0.4;
	float b_height = SCREEN_H * 0.1;
	b->box = (Rectangle){(SCREEN_W * 0.5 - 0.5 * b_width), 
						SCREEN_H * vertical_pos_as_perc, 
						b_width * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR, 
						b_height * PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR};
	b->font_size = FONT_SIZE;
	b->text_color = WHITE;
	b->inside_color =DARK_CYAN;
	b->inside_color_when_selected = CYAN_BLUE;
	b->border_color = VERY_DARK_CYAN;
	b->shadow_color = DARK_CYAN;
}

// Used to draw offset paralax layer art on start menu
Rectangle draw_offset_menu(Texture2D layer_texture, float scalar, float screen_scalar, bool darker){
	Rectangle source_rec =(Rectangle){0,
				0,
				200,
				128};

	Vector2 camera_position_offset = Vector2Subtract(GetMousePosition(), (Vector2){
					SCREEN_W * 0.5, 
					SCREEN_H * 0.5});
	Vector2 circle_motion = (Vector2){cosf(GetTime() * 1 * 3.14), sinf(GetTime() * 1 * 3.14)};
	circle_motion.x *= 300*PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	circle_motion.y *= 300*PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	camera_position_offset = Vector2Add(camera_position_offset, circle_motion);
	camera_position_offset = Vector2Scale(camera_position_offset, -scalar);
	
	float screen_offset = -(screen_scalar - 1)/2;
	
	Rectangle dest_screen = (Rectangle){	screen_scalar * screen_offset * UNITS_PER_PIXEL * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR, 
											screen_scalar *screen_offset * UNITS_PER_PIXEL * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR,
											200 * screen_scalar * 4 / CAMERA_ZOOM_FACTOR,
											128 * screen_scalar * 4 / CAMERA_ZOOM_FACTOR};

	Vector2 screen_position = (Vector2){dest_screen.x, dest_screen.y};
	screen_position = Vector2Add(screen_position, camera_position_offset); // Add the paralax
	dest_screen.x = to_pixel_grid(screen_position.x);
	dest_screen.y = to_pixel_grid(screen_position.y);
	Color layer_color = (darker)? (Color){150, 150, 150, 255}: WHITE;
	DrawTexturePro(layer_texture, source_rec, dest_screen, (Vector2){0, 0}, 0, layer_color);

	return dest_screen;
}

void draw_score_at(int score_number, TYPE_SCORE *score_list, Color text_color, POS_ON_SCREEN pos_text, float vertical_position, int font_size){
	write_text_aligned((char *)TextFormat("%s: %i\n",  
		score_list[score_number].name, score_list[score_number].score),
		vertical_position, font_size, text_color, pos_text);
}