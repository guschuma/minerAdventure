#include "gameObjects.h"
#include "global.h"
#include <math.h>
#include "raymath.h"
#include "enemy.h"
#include "player.h"

#include <stdio.h>
#define CAMERA_QUICKNESS 0.2
double in_game_time = 0;
int score = 0;
Color score_text_color;
// Update BeingObjectPosition
void update_body_position(Vector2 new_position, PhysicsBody *b){
	b->hitbox.x =  new_position.x;
	b->hitbox.y =  new_position.y;
	b->position.x = new_position.x -= 0.5 * (UNITS_PER_BLOCK - b->hitbox.width);
	b->position.y = new_position.y -= (UNITS_PER_BLOCK - b->hitbox.height);
}

void update_projectile_position(ProjectileObject *p, bool has_gravity){ // Velocity is constant
	if(!(p->is_moving)) return;
	if(has_gravity) p->body.velocity_vector.y += GRAVITY_ACCEL * DT;
	Vector2 new_position = (Vector2){p->body.hitbox.x, p->body.hitbox.y};
	bool is_grounded, took_damage; // Not utilised
	new_position.x = collision_detect_blocks(&(p->body.velocity_vector.x), HORIZO, &(p->body), &is_grounded, &took_damage, false);
	update_body_position(new_position, &(p->body));
	if(p->body.velocity_vector.x == 0) {
		p->is_moving = false;
		return;
	} 
	new_position.y = collision_detect_blocks(&(p->body.velocity_vector.y), VERTIC, &(p->body), &is_grounded, &took_damage, false);
	update_body_position(new_position, &(p->body));
	if(p->body.velocity_vector.y == 0) {
		p->is_moving = false;
		return;
	} 
	

}

Vector2 camera_position = {0};
Vector2 camera_velocity = {0};

float to_pixel_grid(float x){
	// Round to pixel grid
	return UNITS_PER_PIXEL * ((int) ((x + 0.5 * PIXEL_PER_UNITS) * PIXEL_PER_UNITS));
}

Vector2 v_to_pixel_grid(Vector2 v){
	return (Vector2){
		to_pixel_grid(v.x),
		to_pixel_grid(v.y),
	};
}

// Quantidade de zoom é definido por CAMERA_ZOOM_FACTOR
// Posicão global é colocada no meio da tela
Rectangle position_rel_to_camera(Rectangle global_pos_rec){
#ifdef USE_GLOBAL_CAMERA
	global_pos_rec.x += GLOBAL_CAMERA_OFFSET_W_UNITS;
	global_pos_rec.y += GLOBAL_CAMERA_OFFSET_H_UNITS;
	return global_pos_rec;
#else
	Vector2 middle_point = {
		((float)SCREEN_W)/CAMERA_ZOOM_FACTOR / 2,
		((float)SCREEN_H)/CAMERA_ZOOM_FACTOR / 2,
	};
	// Equation to transform CAMERA_ZOOM_FACTOR * (global_position - camera_position + middle_point.x),
	Vector2 global_position = (Vector2){global_pos_rec.x, global_pos_rec.y};
	Vector2 relative_position = Vector2Subtract(global_position, camera_position);
	relative_position = Vector2Add(relative_position, middle_point);
	relative_position = Vector2Scale(relative_position, CAMERA_ZOOM_FACTOR);
	
	return (Rectangle){relative_position.x, 
						relative_position.y, 
						global_pos_rec.width * CAMERA_ZOOM_FACTOR, 
						global_pos_rec.height * CAMERA_ZOOM_FACTOR};
#endif
}

// Função inversa da anterior
Rectangle camera_to_global(Rectangle relative_pos_rec){
#ifdef USE_GLOBAL_CAMERA
	relative_pos_rec.x -= GLOBAL_CAMERA_OFFSET_W_UNITS;
	relative_pos_rec.y -= GLOBAL_CAMERA_OFFSET_H_UNITS;
	return relative_pos_rec;
#else
	Vector2 middle_point = {
		((float)SCREEN_W)/CAMERA_ZOOM_FACTOR / 2,
		((float)SCREEN_H)/CAMERA_ZOOM_FACTOR / 2,
	};
	Vector2 relative_position = (Vector2){relative_pos_rec.x, relative_pos_rec.y};
	Vector2 global_position = Vector2Scale(relative_position, 1.0/CAMERA_ZOOM_FACTOR);
	
	global_position = Vector2Subtract(global_position, middle_point);
	global_position = Vector2Add(global_position, camera_position);
	
	
	return (Rectangle){global_position.x, 
						global_position.y, 
						relative_pos_rec.width / CAMERA_ZOOM_FACTOR, 
						relative_pos_rec.height / CAMERA_ZOOM_FACTOR};
#endif
}

Vector2 v_position_rel_to_camera(Vector2 global_pos_vec){
#ifdef USE_GLOBAL_CAMERA
	global_pos_vec.x += GLOBAL_CAMERA_OFFSET_W_UNITS;
	global_pos_vec.y += GLOBAL_CAMERA_OFFSET_H_UNITS;
	return global_pos_vec;
#else
	Vector2 middle_point = {
		((float)SCREEN_W)/CAMERA_ZOOM_FACTOR / 2,
		((float)SCREEN_H)/CAMERA_ZOOM_FACTOR / 2,
	};
	// Equation to transform CAMERA_ZOOM_FACTOR * (global_position - camera_position + middle_point.x),
	Vector2 relative_position = Vector2Subtract(global_pos_vec, camera_position);
	relative_position = Vector2Add(relative_position, middle_point);
	relative_position = Vector2Scale(relative_position, CAMERA_ZOOM_FACTOR);
	
	return relative_position;
#endif
}
Vector2 v_camera_to_global(Vector2 relative_pos_vec){
#ifdef USE_GLOBAL_CAMERA
	relative_pos_vec.x -= GLOBAL_CAMERA_OFFSET_W_UNITS;
	relative_pos_vec.y -= GLOBAL_CAMERA_OFFSET_H_UNITS;
	return relative_pos_vec;
#else
	Vector2 middle_point = {
		((float)SCREEN_W)/CAMERA_ZOOM_FACTOR / 2,
		((float)SCREEN_H)/CAMERA_ZOOM_FACTOR / 2,
	};
	Vector2 global_position = Vector2Scale(relative_pos_vec, 1.0/CAMERA_ZOOM_FACTOR);
	
	global_position = Vector2Subtract(global_position, middle_point);
	global_position = Vector2Add(global_position, camera_position);
	
	
	return global_position;
#endif
}


void set_camera_position_to(Vector2 global_position){
	camera_position = (Vector2){
					to_pixel_grid(global_position.x),
					to_pixel_grid(global_position.y)};
}

void move_camera_position_to(Vector2 global_position){
	Vector2 offset = (Vector2){
		global_position.x - camera_position.x,
		global_position.y - camera_position.y,
	};
	camera_velocity = (Vector2){
		offset.x * CAMERA_QUICKNESS,
		offset.y * CAMERA_QUICKNESS,
	};
	set_camera_position_to(Vector2Add(camera_position, camera_velocity));
}

Texture2D bg_layer_1, bg_layer_2, bg_layer_3, bg_layer_4, bg_layer_5, bg_layer_6, bg_layer_7;
Texture2D load_image_texture(const char *file_name){
	Image temp_image = LoadImage(file_name);
	Texture2D sprite = LoadTextureFromImage(temp_image);
	UnloadImage(temp_image); 
	return sprite;
}

void restart_entire_level(void){
	in_game_time = 0;
	restart_projectiles();
	restart_enemies();
	init_map();
	init_player();
	hook.is_active = false;
}

void load_textures(){
	snake_sprite = load_image_texture("assets/snake.png");
	skeleton_sprite = load_image_texture("assets/skeleton.png");
	bat_sprite = load_image_texture("assets/bat.png");
	frog_sprite = load_image_texture("assets/frog.png");

	player_running_sprites = load_image_texture("assets/player_correndo.png");
	player_climbing_sprite = load_image_texture("assets/player_climbing.png");
	player_jumping_sprites = load_image_texture("assets/player_pulando.png");

	player_life_sprite = load_image_texture("assets/player_life.png");


	block_sprite = load_image_texture("assets/block.png"); 
	block_tilemap = load_image_texture("assets/tilemap.png");
	wood_pillar = load_image_texture("assets/wood_pillar.png");
	torch_block = load_image_texture("assets/torch.png");
	world_border = load_image_texture("assets/world_border.png");
	spike_block = load_image_texture("assets/spike_block.png");
	ladder_block = load_image_texture("assets/ladder.png");

	elevator_up_idle = load_image_texture("assets/elevator_up_idle.png");
	elevator_up = load_image_texture("assets/elevator_up.png");
	elevator_down = load_image_texture("assets/elevator_down.png");
	elevator_down_idle = load_image_texture("assets/elevator_down_idle.png");
	elevator_up_closed_idle = load_image_texture("assets/elevator_up_closed.png");

	defeat_screen = load_image_texture("assets/defeat_screen.png");

	layer1_menu = load_image_texture("assets/menu-layers/layer1.png");
	layer2_menu = load_image_texture("assets/menu-layers/layer2.png");
	layer3_menu = load_image_texture("assets/menu-layers/layer3.png");
	layer4_menu = load_image_texture("assets/menu-layers/layer4.png");
	layer5_menu = load_image_texture("assets/menu-layers/layer5.png");
	layer6_menu = load_image_texture("assets/menu-layers/layer6.png");
	layer7_menu = load_image_texture("assets/menu-layers/layer7.png");

	bg_layer_1 = load_image_texture("assets/paralax-layer/layer_1.png");
	bg_layer_2 = load_image_texture("assets/paralax-layer/layer_2.png");
	bg_layer_3 = load_image_texture("assets/paralax-layer/layer_3.png");
	bg_layer_4 = load_image_texture("assets/paralax-layer/layer_4.png");
	bg_layer_5 = load_image_texture("assets/paralax-layer/layer_5.png");
	bg_layer_6 = load_image_texture("assets/paralax-layer/layer_6.png");
	bg_layer_7 = load_image_texture("assets/paralax-layer/layer_7.png");

}
Rectangle draw_offset_bg_layer(Texture2D layer_texture, float scalar){
	Rectangle source_rec =(Rectangle){0,
				0,
				180,
				180};

	Vector2 camera_position_offset = Vector2Subtract(camera_position, (Vector2){
					SCREEN_W * 0.5 - GLOBAL_CAMERA_OFFSET_W_UNITS, 
					SCREEN_H * 0.5 - GLOBAL_CAMERA_OFFSET_H_UNITS});
	camera_position_offset = Vector2Scale(camera_position_offset, -scalar);
	
	float screen_scalar = 2;
	float screen_offset = -(screen_scalar - 1)/2;
	
	Rectangle dest_screen = (Rectangle){	screen_offset * PIXEL_PER_UNITS * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR, 
											screen_offset * PIXEL_PER_UNITS * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR,
											screen_scalar * PIXEL_PER_UNITS * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR,  // TODO: Might have to change PIXEL_PER_UNITS
											screen_scalar * PIXEL_PER_UNITS * MAP_SIZE_IN_UNITS / CAMERA_ZOOM_FACTOR};

	Vector2 screen_position = (Vector2){dest_screen.x, dest_screen.y};
	screen_position = Vector2Add(screen_position, camera_position_offset); // Add the paralax
	dest_screen.x = to_pixel_grid(screen_position.x);
	dest_screen.y = to_pixel_grid(screen_position.y);
	Color darker_color = (Color){150, 150, 150, 255};
	DrawTexturePro(layer_texture, source_rec, dest_screen, (Vector2){0, 0}, 0, darker_color);

	return dest_screen;
}

float max_value(float a, float b);

void load_background(){
	BeginTextureMode(pixelated_background);
	ClearBackground(BLANK);

	float max_layer_paralax_scale = 0.8;
	
	draw_offset_bg_layer(bg_layer_1, max_layer_paralax_scale * 1 * 0.14);
	draw_offset_bg_layer(bg_layer_2, max_layer_paralax_scale * 2 * 0.14);
	draw_offset_bg_layer(bg_layer_5, max_layer_paralax_scale * 3 * 0.14);
	draw_offset_bg_layer(bg_layer_3, max_layer_paralax_scale * 4 * 0.14);
	draw_offset_bg_layer(bg_layer_4, max_layer_paralax_scale * 5 * 0.14);
	draw_offset_bg_layer(bg_layer_6, max_layer_paralax_scale * 5.5 * 0.14);
	Rectangle last_layer_on_screen = draw_offset_bg_layer(bg_layer_7, max_layer_paralax_scale * 7 * 0.14);
	
	Color offscreen_color = VERY_VERY_DARK_PURPLE;
	DrawRectangle(	0, 
					0, 
					last_layer_on_screen.x + last_layer_on_screen.width + 1, last_layer_on_screen.y + 1, 
					offscreen_color);
	
	DrawRectangle(	0, 
					0, 
					last_layer_on_screen.x + 1, 
					last_layer_on_screen.y + last_layer_on_screen.height + 1, offscreen_color);
	DrawRectangle(	max_value(last_layer_on_screen.x + last_layer_on_screen.width, 0), 
					0, 
					pixelated_background.texture.width, 
					pixelated_background.texture.height, offscreen_color);

	DrawRectangle(	0, 
					max_value(last_layer_on_screen.y + last_layer_on_screen.height, 0), 
					pixelated_background.texture.width, 
					pixelated_background.texture.height, offscreen_color);
	
	EndTextureMode();
}

void draw_textured_screen(RenderTexture2D render_screen, bool semitransparent){
	Color color = WHITE;
	if(semitransparent) {
	BeginBlendMode(BLEND_MULTIPLIED);
	}
	
	DrawTexturePro(
		render_screen.texture,
		(Rectangle){0,
				0,
				render_screen.texture.width,
				-render_screen.texture.height},
		(Rectangle){0,
					0,
					SCREEN_W,
					SCREEN_H},
		(Vector2){0, 0},
		0.0f,
		color
	);

	if(semitransparent) {
	EndBlendMode();
	}
}

float max_value(float a, float b){
	return (a > b)? a : b;
}

void draw_cursor_texture(){
	BeginTextureMode(pixelated_screen);
	Vector2 mousePosition = GetMousePosition();
	mousePosition = Vector2Scale(mousePosition, PIXEL_PER_UNITS/CAMERA_ZOOM_FACTOR);
	float circle_cursor_radius = (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT))? 2: 1;
	DrawCircleLines(mousePosition.x + 0.5, mousePosition.y + 0.5, circle_cursor_radius, GPINK);
	EndTextureMode();
}

bool isPressed(key k){
	switch(k){
		case LEFTK:
		return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
		case RIGHTK:
		return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
		case UPK:
		return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE);
		case SHIFTK:
		return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
		case DOWNK:
		return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
		default:
		return false;
	}
}

void camera_handling(void){
	Vector2 centralized_player_position = {
		player.body.hitbox.x + player.body.hitbox.width * 0.5,
		player.body.hitbox.y + player.body.hitbox.height * 0.5,
	};

	if(player.direction == LEFT) centralized_player_position.x -= 1*UNITS_PER_BLOCK;
	else centralized_player_position.x += 1*UNITS_PER_BLOCK;
	centralized_player_position.y -= 1*UNITS_PER_BLOCK;
	Vector2 global_mouse = v_camera_to_global(GetMousePosition());
	centralized_player_position = Vector2Lerp(centralized_player_position, global_mouse, 0.25);
	centralized_player_position = v_to_pixel_grid(centralized_player_position);
	move_camera_position_to(centralized_player_position);
}

