#include "player.h"
#include "global.h"
#include "raymath.h"
#include "enemy.h"
#include "scenes.h"
//@TODO: Deletar stdio
#include <stdio.h>
#include <math.h>


#define PLAYER_VELOCITY (80 * UNITS_PER_PIXEL)  		// pixeis por segundo
#define MAX_PLAYER_VELOCITY (100 * UNITS_PER_PIXEL)   	// pixeis por segundo
#define PLAYER_ACCEL (10 * UNITS_PER_PIXEL )     		// pixeis por segundo^2
#define PLAYER_W (10 * UNITS_PER_PIXEL)
#define PLAYER_H (14 * UNITS_PER_PIXEL)

#define PLAYER_STOMP_VELOCITY (2.0) // All in terms of walking speed
#define JUMP_FORCE (2.3)  			
#define VERTICAL_DASH_FORCE (2.4)
#define HORIZONTAL_DASH_FORCE (2.5)
#define LITTLE_DASH_FORCE (1.2)		// Happens when player has not pressed upwards

#define FRICTION_IN_AIR 0.92 		// Closer to 1 = quicker movement in air
#define FRICTION_IN_GROUND 0.85 	// Closer to 1 = quicker movement in ground
#define MAX_VELOCITY_AIR_HORIZ (120 * UNITS_PER_PIXEL)
#define MAX_VELOCITY_AIR_VERT (170 * UNITS_PER_PIXEL)

#define DASH_DELAY 0.25	

#define ANIMATION_WALK_DELAY 0.07   // Amount of time to wait for next frame of animation
#define INVICIBILITY_TIME (2.0)
#define PLAYER_DAMAGE_ANIMATION_TIME (0.5)

#define HOOK_SIZE (3 * UNITS_PER_PIXEL)
#define HOOK_VELOCITY (330 * UNITS_PER_PIXEL)
#define HOOK_FORCE (0.1 * UNITS_PER_PIXEL)
#define HOOK_TANGENT_PORCENTAGE (0.5)	// How much the tangent of the hook affects the player velocity

PlayerObject player = {0};
ProjectileObject hook;

Texture2D player_running_sprites;
Texture2D player_jumping_sprites;
Texture2D player_life_sprite;
bool is_moving = false;

double last_walk_animation_update = 0.0;
double last_time_on_ground = 0.0;
char frame_index = 0; // index of walking animation
double last_time_since_was_hit_animation;

double time_entered_dash_delay = 0.0;
bool can_dash = true, on_dash_delay = false;


void init_player(void) {
	player.velocity = PLAYER_VELOCITY * DT;
	player.body.velocity_vector = (Vector2){0, 0};
	player.is_grounded = true;
	player.body.hitbox.width = PLAYER_W;
	player.body.hitbox.height = PLAYER_H;
	player.direction = RIGHT;
	player.tint = WHITE;
	update_body_position(player_starting_position, &player.body);
	

	last_walk_animation_update = in_game_time;
	last_time_on_ground = in_game_time;
	time_entered_dash_delay = in_game_time - DASH_DELAY;
	last_time_since_was_hit_animation = in_game_time -INVICIBILITY_TIME;
	can_dash = true;
	on_dash_delay = false;

	player.was_hit_animation = false;

	hook.is_active = false;
	hook.is_moving = false;
	hook.body.hitbox = (Rectangle){0, 0, HOOK_SIZE, HOOK_SIZE};
	hook.velocity = HOOK_VELOCITY * DT;
}

void player_will_be_allowed_to_dash(){
	on_dash_delay = true;
	time_entered_dash_delay = in_game_time;
}

float angle_for_hook(Vector2 target_position){
	float x0 = target_position.x, y0 = -target_position.y;
	float g = GRAVITY_ACCEL * DT;
	float v = hook.velocity;

	float root = v*v*v*v - g * (g * x0*x0 + 2*y0*v*v);
	if(root < 0) return INFINITY;
	return atan2f(v*v - sqrt(root), g * x0);
}

void init_hook_at(Vector2 init_position){
	update_body_position(init_position, &hook.body); // Update position
	// From here on: Update initial velocity
	Vector2 mouse_local_position = v_camera_to_global(GetMousePosition());
	Vector2 hook_position_sub = Vector2Subtract(mouse_local_position, init_position); // Difference between player and hook
	float init_angle = angle_for_hook(hook_position_sub);
	if(init_angle == INFINITY){
		// Just use the direct angle for the initial velocity (Target is unachiveable)
		hook_position_sub.x -= hook.body.hitbox.width * 0.5;
		hook_position_sub.y -= hook.body.hitbox.height * 0.5;
		hook_position_sub = Vector2Normalize(hook_position_sub);
		hook.body.velocity_vector = Vector2Scale(hook_position_sub, hook.velocity); // Make initial velocity of hook that direction
	}
	else{
		// Use complex equation (considering the arch of the hook) for initial velocity
		float v = hook.velocity;
		hook.body.velocity_vector.x = v * cos(init_angle);
		hook.body.velocity_vector.y = -v * sin(init_angle);
	}
	hook.is_active = true;
	hook.is_moving = true;
}
void update_hook_position(){
	if(hook.is_moving) update_projectile_position(&hook, true);
	Vector2 middle_of_player = {player.body.hitbox.x + player.body.hitbox.width * 0.5, player.body.hitbox.y + player.body.hitbox.height * 0.5};

	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)|| IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
		// Initialize hook if player started clicking
		init_hook_at(middle_of_player);
	}
}

// Updates the player velocity if hook is not moving (is hooked to a block)
Vector2 calculate_player_hook_velocity(Vector2 current_velocity){
	update_hook_position();
	if((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) && !hook.is_moving){
		Vector2 hook_center = {
			hook.body.hitbox.x + hook.body.hitbox.width * 0.5,
			hook.body.hitbox.y + hook.body.hitbox.height * 0.5
		};
		Vector2 middle_of_player = {player.body.hitbox.x + player.body.hitbox.width * 0.5, player.body.hitbox.y + player.body.hitbox.height * 0.5};

		Vector2 hook_position_sub = Vector2Subtract((Vector2){hook_center.x, hook_center.y}, middle_of_player);
		Vector2 hook_force_vector = Vector2Scale(hook_position_sub,  HOOK_FORCE * DT);
		Vector2 perpendicular_hook_vector;
		perpendicular_hook_vector.x = hook_position_sub.y;
		perpendicular_hook_vector.y = -hook_position_sub.x;
		perpendicular_hook_vector = Vector2Normalize(perpendicular_hook_vector);
		float force_value = Vector2DotProduct(current_velocity, perpendicular_hook_vector);
		perpendicular_hook_vector = Vector2Scale(perpendicular_hook_vector, force_value);
		current_velocity = Vector2Lerp(current_velocity, perpendicular_hook_vector, HOOK_TANGENT_PORCENTAGE);

		current_velocity = Vector2Add(current_velocity, hook_force_vector);
	}
	return current_velocity;
}
void draw_hook_textures(){
	Vector2 middle_of_player = {player.body.hitbox.x + player.body.hitbox.width * 0.5, player.body.hitbox.y + player.body.hitbox.height * 0.5};
	Vector2 hook_center = {
		hook.body.hitbox.x + hook.body.hitbox.width * 0.5,
		hook.body.hitbox.y + hook.body.hitbox.height * 0.5
	};

	BeginTextureMode(pixelated_screen);
	Vector2 middle_of_player_rel = v_position_rel_to_camera(middle_of_player);
	middle_of_player_rel.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	middle_of_player_rel.y *= PIXEL_PER_UNITS /CAMERA_ZOOM_FACTOR;
	Vector2 hook_rel = v_position_rel_to_camera((Vector2){hook_center.x, hook_center.y});
	hook_rel.x *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	hook_rel.y *= PIXEL_PER_UNITS / CAMERA_ZOOM_FACTOR;
	// 0.5 para arredondamento, -1 para quadrado 3x3 ter centro no meio
	DrawRectangleRec((Rectangle){hook_rel.x + 0.5 - 2, hook_rel.y + 0.5 - 2, 3, 3}, GBLUE);
	bool is_player_clicking = IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	if(is_player_clicking){
		DrawLineEx(
			middle_of_player_rel,
			hook_rel,
			1,
			GPURPLE
		);
	}
	EndTextureMode();
}
void update_player_position(){
	Vector2 current_velocity = player.body.velocity_vector;
	is_moving = false;
	// Update dash related features -=-
	if(in_game_time - time_entered_dash_delay > DASH_DELAY && on_dash_delay){
		can_dash = true;
		on_dash_delay = false;
	}
	if(player.is_grounded && !on_dash_delay){
		player_will_be_allowed_to_dash();
	}
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	if(isPressed(UPK) && player.is_grounded && !player.was_hit_animation){
		current_velocity.y -= player.velocity * JUMP_FORCE; // -
		player.body.position.y -= 2; // Offset um pouco para cima para evitar colisao
		player.is_grounded = false;
		

		if(CheckCollisionRecs(player.body.hitbox, end_elevator_rec)) exit_level_cutscene();
	}
	if(isPressed(LEFTK) && !player.was_hit_animation){
		current_velocity.x -= PLAYER_ACCEL * DT; // -
		is_moving = true;
		player.direction = LEFT;
	}
	if(isPressed(RIGHTK) && !player.was_hit_animation){
		current_velocity.x += PLAYER_ACCEL * DT; // +
		is_moving = true;
		player.direction = RIGHT;
	}
	if(isPressed(SHIFTK) && can_dash && !player.was_hit_animation){
		// Dash movement!
		can_dash = false;
		on_dash_delay = false;
		if(player.direction == RIGHT && is_moving) current_velocity.x += player.velocity * HORIZONTAL_DASH_FORCE;
		if(player.direction == LEFT && is_moving) current_velocity.x -= player.velocity * HORIZONTAL_DASH_FORCE;
		if(isPressed(UPK)) {
			current_velocity.y = (current_velocity.y < 0)? current_velocity.y -player.velocity * VERTICAL_DASH_FORCE : -player.velocity * VERTICAL_DASH_FORCE;
		}
		else if(isPressed(DOWNK)){
			current_velocity.y = player.velocity * VERTICAL_DASH_FORCE;
		}
		else current_velocity.y = -player.velocity * LITTLE_DASH_FORCE;
	}
	if(fabsf(current_velocity.x) > player.velocity && player.is_grounded){ 
		current_velocity.x *= FRICTION_IN_GROUND;
	}
	else if((fabsf(current_velocity.x) > MAX_VELOCITY_AIR_HORIZ * DT) && !player.is_grounded){
		current_velocity.x *= FRICTION_IN_AIR;
	}
	if(fabsf(current_velocity.y) > MAX_VELOCITY_AIR_VERT * DT){
		current_velocity.y *= FRICTION_IN_AIR;
	}
	if(!is_moving) {
		if(current_velocity.x < 0.001 && current_velocity.x > -0.001) current_velocity.x = 0;
		else current_velocity.x *= FRICTION_IN_GROUND;
	}
	if(!player.is_grounded) current_velocity.y += GRAVITY_ACCEL * DT;
	
	
	// -=-=-=-=-=-=-=-=-=-=-=- Hook feature!!! -=-=-=-=-=--=-=-=-=-=-=-

	current_velocity = calculate_player_hook_velocity(current_velocity);
	// -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-==-=-=-==-=-=-=-=-=-=-
	
	Vector2 new_position;
	new_position.x = collision_detect_blocks(&current_velocity.x, HORIZO, &player.body, &(player.is_grounded));
	new_position.y = collision_detect_blocks(&current_velocity.y, VERTIC, &player.body, &(player.is_grounded));
	player.body.velocity_vector = current_velocity;
	update_body_position(new_position, &player.body);
}

int flicker_invincibility = 0;
void update_player_texture(){
	if(is_frame_frozen) return;
	player.tint = (can_dash)? WHITE : (Color){170,170,170,255};
	if(player.is_grounded) last_time_on_ground = in_game_time;
	if(in_game_time - last_time_since_was_hit_animation < INVICIBILITY_TIME){
		flicker_invincibility++;
		flicker_invincibility %= 10;
		
		if(flicker_invincibility >= 5){
			player.tint.a = 170;
		}
	}
	if(player.was_hit_animation){
		player.was_hit_animation = true;
		if(in_game_time - last_time_since_was_hit_animation > PLAYER_DAMAGE_ANIMATION_TIME){
			player.was_hit_animation = false;
		}
		int player_damage_tint = 150 + 105 * (in_game_time - last_time_since_was_hit_animation)/PLAYER_DAMAGE_ANIMATION_TIME;
		score_text_color.g = player_damage_tint * 101 / 255;
		int player_alpha = player.tint.a;
		player.tint = (Color){255, player_damage_tint, 255, player_alpha};
		player.texture =  player_jumping_sprites;
		player.texture_rect = (Rectangle){16, 0, 16, 16};
	}
	
	else if(!player.is_grounded && time_on_air() > 0.1){
		// Necessary for player to not flicker while on ground
		player.texture = player_jumping_sprites;
		if(player.body.velocity_vector.y > 0){ // Jogador caindo
			player.texture_rect = (Rectangle){16, 0, 16, 16};
		}
		else player.texture_rect = (Rectangle){0, 0, 16, 16};
	}
	else if(is_moving){
		if(is_walk_animation_timer_active()){
			frame_index++;
			frame_index %= 8;
		}
		player.texture = player_running_sprites;
		player.texture_rect = (Rectangle){16 * frame_index, 0, 16, 16};
	}
	else { // Player has stopped
		player.texture = player_running_sprites;
		player.texture_rect = (Rectangle){0, 0, 16, 16};
	}
	if(player.direction == LEFT) player.texture_rect.width *= -1; // Flip the sprite
	
}

void draw_player(void){
	Vector2 relative_position = v_position_rel_to_camera((Vector2){
		to_pixel_grid(player.body.position.x), 
		to_pixel_grid(player.body.position.y)});

	update_player_texture();

	Rectangle player_destination_rect = {
		relative_position.x,
		relative_position.y,
		UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR,
		UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR,
	};

	DrawTexturePro(player.texture, player.texture_rect, player_destination_rect, (Vector2){0, 0}, 0, player.tint);
	# ifdef DRAW_HITBOXES
	Rectangle rel_player_hitbox = position_rel_to_camera(player.body.hitbox);
	Rectangle rel_hook_hitbox = position_rel_to_camera(hook.body.hitbox);
	DrawRectangleLines(rel_hook_hitbox.x, rel_hook_hitbox.y, rel_hook_hitbox.width, rel_hook_hitbox.height, WHITE);
	DrawRectangleLines(rel_player_hitbox.x, rel_player_hitbox.y, rel_player_hitbox.width, rel_player_hitbox.height, BLUE);
	# endif
	
}
// Draws life UI
void draw_lifes(int health){
	Vector2 first_life_position = (Vector2){0, 0};
	Rectangle source_rec = (Rectangle){0, 0, 16, 16}; // Size of image sprite

	BeginTextureMode(pixelated_screen);
	if(health >= 1) DrawTextureRec(player_life_sprite, source_rec, first_life_position, WHITE);
	first_life_position.x += PIXELS_PER_BLOCK;
	if(health >= 2) DrawTextureRec(player_life_sprite, source_rec, first_life_position, WHITE);
	first_life_position.x += PIXELS_PER_BLOCK;
	if(health >= 3) DrawTextureRec(player_life_sprite, source_rec, first_life_position, WHITE);
	EndTextureMode();

}

void update_player_collision_enemy(Enemy *e){
	if(!e->is_active) return;
	if(!CheckCollisionRecs(e->body.hitbox, player.body.hitbox)) return;

	float player_vertical_velocity = player.body.velocity_vector.y;
	float player_vertical_position = player.body.hitbox.y;
	float enemy_vertical_position = e->body.hitbox.y;

	player_vertical_position += player.body.hitbox.height * 0.5;
	if(player_vertical_position < enemy_vertical_position && player_vertical_velocity > 0){
		// Player has stomped
		player_will_be_allowed_to_dash();
		enemy_was_hit_update(e);
		player.body.hitbox.y = enemy_vertical_position - player.body.hitbox.height;
		player.body.velocity_vector.y = -PLAYER_STOMP_VELOCITY * player.velocity;
	}
	else {
		player_was_hit();
	}
}
void update_player_collision_projectile(ProjectileObject *p, int p_index){
	if(!p->is_active) return;
	
	if(!CheckCollisionRecs(p->body.hitbox, player.body.hitbox)) return;
	player_was_hit();
	delete_projectile(p_index);
}

void player_was_hit(){
	if(in_game_time - last_time_since_was_hit_animation < INVICIBILITY_TIME) return; // Player was during invicibility time
	player.health--;
	score -= 50;
	last_time_since_was_hit_animation = in_game_time;
	player.was_hit_animation = true;
	if(player.direction == LEFT) player.body.velocity_vector.x += player.velocity;
	else player.body.velocity_vector.x -= player.velocity;
}

// Necessary to wait a bit before triggering jumping animation,
//otherwise charater flickers on ground
double time_on_air(){
	return in_game_time - last_time_on_ground;
}

bool is_walk_animation_timer_active(){
	if(in_game_time - last_walk_animation_update < ANIMATION_WALK_DELAY) return false; // Time still not active
	
	last_walk_animation_update = in_game_time;
	return true;
}
