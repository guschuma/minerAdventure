#include "enemy.h"
#include "sounds.h"
#include <stdio.h>
#include "raymath.h"

#define SNAKE_VELOCITY (40 * UNITS_PER_PIXEL)
#define SNAKE_HEALTH (5)
#define ENEMY_ACCEL (30 * UNITS_PER_PIXEL)
#define ENEMY_HITBOX (13 * UNITS_PER_PIXEL)

#define SKELETON_VELOCITY (20 * UNITS_PER_PIXEL)
#define SKELETON_HEALTH (3)

#define ENEMY_DAMAGE_ANIMATION_TIME 0.5

#define SKELETON_TIME_TO_ATTACK 4.0					// How much does the skeleton wait to start attacking In seconds
#define SKELETON_ATTACK_ANIMATION_SPEED 0.7			// How much time does the attack animation last


#define ARROW_VELOCITY (80 * UNITS_PER_PIXEL)
#define ARROW_HITBOX_SIZE 2
#define ARROW_VISUAL_LENGTH (7 * UNITS_PER_PIXEL)

int enemy_count = 0;
int active_enemy_count = 0;
Enemy enemy_list[MAX_ENEMY_NUMBER] = {0};
int projectile_count = 0;
ProjectileObject projectile_list[MAX_PROJECTILE_NUMBER] = {0};

#define SNAKE_ANIMATION_CHANGE_TIME 0.2			// Amount of time to wait until walk animation change (seconds)
#define SKELETON_ANIMATION_CHANGE_TIME 0.07		// 

Texture2D skeleton_sprite;
Texture2D snake_sprite;

void restart_enemies(){
	for(int e_ind = 0; e_ind < enemy_count; e_ind++){
		enemy_list[e_ind] = (Enemy){0};
	}
	enemy_count = 0;
	active_enemy_count = 0;
}
void init_enemy(Vector2 global_position, EnemyType type){
	if(enemy_count >= MAX_ENEMY_NUMBER) return;

	Enemy new_enemy;
	new_enemy.is_active = true;
	new_enemy.was_hit = false;
	new_enemy.type = type;
	new_enemy.enemy_tint = WHITE;
	new_enemy.is_attacking = false;
	new_enemy.time_since_last_attack = 0;
	new_enemy.time_since_last_animation = 0;
	new_enemy.animation_index = 0;

	switch(type){
		case ENEMY_SNAKE:
			new_enemy.velocity = SNAKE_VELOCITY * DT;
			new_enemy.texture = snake_sprite;
			new_enemy.health = SNAKE_HEALTH;
			break;
		case ENEMY_SKELETON:
			new_enemy.velocity = SKELETON_VELOCITY * DT;
			new_enemy.texture = skeleton_sprite;
			new_enemy.health = SKELETON_HEALTH;
			new_enemy.time_since_last_attack = in_game_time - GetRandomValue(0, 1000 * SKELETON_TIME_TO_ATTACK) / 1000.0; // Gets a random value between zero and attack time
			break;
	}
	new_enemy.direction = LEFT;

	new_enemy.body.position = global_position;
	new_enemy.body.hitbox.width = ENEMY_HITBOX;
	new_enemy.body.hitbox.height = ENEMY_HITBOX;

	new_enemy.body.hitbox.x = global_position.x + 0.5 * (UNITS_PER_BLOCK - ENEMY_HITBOX);
	new_enemy.body.hitbox.y = global_position.y + (UNITS_PER_BLOCK - ENEMY_HITBOX);


	enemy_list[enemy_count] = new_enemy;
	enemy_count++;
	active_enemy_count++;
}

void update_enemy(Enemy *e){
	if((e->health <= 0 || e->body.hitbox.y > MAP_SIZE_IN_UNITS) && e->is_active) {
		e->is_active = false;
		score += 40;
	}
	if(!e->is_active) return;
	Vector2 current_velocity = e->body.velocity_vector;

	if(e->direction == LEFT) current_velocity.x -= ENEMY_ACCEL * DT;
	else current_velocity.x += ENEMY_ACCEL * DT;

	current_velocity.y +=  GRAVITY_ACCEL * DT;
	if(current_velocity.x > e->velocity) current_velocity.x = e->velocity;
	else if(current_velocity.x < -e->velocity) current_velocity.x = -e->velocity;
	
	if(current_velocity.y > 3 * e->velocity) current_velocity.y = 3 * e->velocity;

	if(e->type == ENEMY_SKELETON){
		if(e->is_grounded && (in_game_time - e->time_since_last_attack > SKELETON_TIME_TO_ATTACK && !e->was_hit)){
			// Skeleton is bouta attack!
			e->time_since_last_attack = in_game_time;
			e->time_since_last_animation = in_game_time;
			e->is_attacking = true;
			current_velocity.x = 0;
			e->animation_index = 8;

			if(e->body.position.x > player.body.position.x) e->direction = LEFT;
			else e->direction = RIGHT;
		}
	}
	if(e->is_attacking) current_velocity.x = 0;
	Vector2 new_position;
	bool took_damage = false;
	new_position.x = collision_detect_blocks(&current_velocity.x, HORIZO, &e->body, &e->is_grounded, &took_damage, false);
	new_position.y = collision_detect_blocks(&current_velocity.y, VERTIC, &e->body, &e->is_grounded, &took_damage, false);
	if(took_damage) enemy_was_hit_update(e);
	if(e->was_previously_grounded && !e->is_grounded && !e->was_hit){
		if(current_velocity.x < 0) e->direction = RIGHT;
		else e->direction = LEFT;
		current_velocity.y = -2 * e->velocity;
	}
	if(current_velocity.x == 0 && e->is_grounded && !e->is_attacking){
		if(e->direction == LEFT) e->direction = RIGHT;
		else e->direction = LEFT;
	}
	
	e->body.velocity_vector = current_velocity;

	update_body_position(new_position, &(e->body));
	
	update_enemy_texture(e);
	e->was_previously_grounded = e->is_grounded;
}

void enemy_was_hit_update(Enemy *e){
	if(!e->was_hit){ // Enemy was not moments before previously hit
		play_sound(HIT_ENEMY);
		score += 20;
		e->health -= 1;
		if(e->health == 0) active_enemy_count--;
		e->was_hit = true;
		e->body.velocity_vector.y = - player.velocity * 0.5;
	}
}
void update_enemy_texture(Enemy *e){
	if(!e->is_active) return;
	double current_animation_timer = in_game_time - e->time_since_last_animation;
	int falling_sprite = (e->type == ENEMY_SNAKE)? 2 : 13;
	if(e->was_hit){
		e->animation_index = falling_sprite;
		e->is_attacking = false;
		int damaged_enemy_tint = 255*(current_animation_timer / ENEMY_DAMAGE_ANIMATION_TIME);
		Color new_enemy_tint = (Color){damaged_enemy_tint, 255, 255, 255};

		e->enemy_tint = new_enemy_tint;
		if(current_animation_timer > ENEMY_DAMAGE_ANIMATION_TIME){
			e->was_hit = false;
			e->time_since_last_animation = in_game_time;
			e->enemy_tint = WHITE;
		}
		
		return;
	}
	else if(!e->is_grounded) {
		e->animation_index = falling_sprite;
		e->time_since_last_animation = in_game_time;
		return;
	}
	
	int max_walk_animation_index;
	float time_for_animation_change;
	switch(e->type){
		case ENEMY_SNAKE:		max_walk_animation_index = 2; time_for_animation_change = SNAKE_ANIMATION_CHANGE_TIME; break;
		case ENEMY_SKELETON:	max_walk_animation_index = 8; time_for_animation_change = SKELETON_ANIMATION_CHANGE_TIME; break;
	}

	if(e->is_attacking){
		// Skeleton is attacking!
		if(current_animation_timer > SKELETON_ATTACK_ANIMATION_SPEED / 5){
			if(e->animation_index == 12) {
				// Stop attack animation
				e->animation_index = 0;
				e->is_attacking = false;
			}
			else if(e->animation_index == 10) {
				// Skeleton has attacked!
				Vector2 middle_of_skeleton = (Vector2){e->body.hitbox.x + e->body.hitbox.width * 0.5, e->body.hitbox.y + e->body.hitbox.height * 0.5};
				play_sound(BOW_SFX);
				initialize_arrow(middle_of_skeleton);
				e->animation_index++;
			}
			else e->animation_index++;

			e->time_since_last_animation = in_game_time;
		}
	}
	else if(in_game_time - e->time_since_last_animation > time_for_animation_change){
		e->time_since_last_animation = in_game_time;
		
		
		switch(e->type){
			case ENEMY_SNAKE:		max_walk_animation_index = 2; break;
			case ENEMY_SKELETON:	max_walk_animation_index = 8; break;
		}
		e->animation_index = (e->animation_index + 1) % max_walk_animation_index;
	}
}

void draw_enemy(Enemy *e){
	if(!(e->is_active)) return;

	Vector2 rel_enemy_position = v_to_pixel_grid(e->body.position);
	rel_enemy_position = v_position_rel_to_camera(rel_enemy_position);
	Rectangle enemy_destination_rect = (Rectangle){
		rel_enemy_position.x,
		rel_enemy_position.y,
		UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR,
		UNITS_PER_BLOCK * CAMERA_ZOOM_FACTOR,
	};
	e->texture_rect = (Rectangle){16 * e->animation_index, 0, 16, 16};
	if(e->direction == RIGHT) e->texture_rect.width *= -1; 
	
	DrawTexturePro(e->texture, e->texture_rect, enemy_destination_rect, (Vector2){0, 0}, 0, e->enemy_tint);
	
	# ifdef DRAW_HITBOXES
	Rectangle rel_enemy_hitbox = position_rel_to_camera((Rectangle){e->body.hitbox.x, 
																		e->body.hitbox.y,
																		e->body.hitbox.width,
																		e->body.hitbox.height});
	
	DrawRectangleLines(rel_enemy_hitbox.x, rel_enemy_hitbox.y, rel_enemy_hitbox.width, rel_enemy_hitbox.height, RED);
	# endif
}

// Initializes arrow directed to player
void initialize_arrow(Vector2 initial_position){
	if(projectile_count >= MAX_PROJECTILE_NUMBER) return;

	ProjectileObject new_arrow = {0};
	new_arrow.velocity = ARROW_VELOCITY * DT;
	update_body_position(initial_position, &new_arrow.body);

	Vector2 middle_of_player = (Vector2){player.body.hitbox.x + player.body.hitbox.width * 0.5, player.body.hitbox.y + player.body.hitbox.height * 0.5};
	Vector2 arrow_velocity_vector = Vector2Subtract(middle_of_player, initial_position);
	arrow_velocity_vector = Vector2Normalize(arrow_velocity_vector);
	new_arrow.body.velocity_vector = Vector2Scale(arrow_velocity_vector, new_arrow.velocity);
	new_arrow.is_active = true;
	new_arrow.is_moving = true;
	new_arrow.body.hitbox.width = new_arrow.body.hitbox.height = ARROW_HITBOX_SIZE;

	projectile_list[projectile_count] = new_arrow;
	projectile_count++;
}
// Updates projectile_list
void update_projectile_positions(){
	for(int p_index = 0; p_index < enemy_count; p_index++){
		if(!(projectile_list[p_index]).is_active) continue;
		ProjectileObject current_projectile = projectile_list[p_index];
		update_projectile_position(&(projectile_list[p_index]), false);
		if(!current_projectile.is_moving){
			delete_projectile(p_index);
		}
		float beyond_range = 10 * UNITS_PER_BLOCK;
		if(current_projectile.body.hitbox.y < -beyond_range || current_projectile.body.hitbox.x < -beyond_range
		||current_projectile.body.hitbox.x > MAP_SIZE_IN_UNITS + beyond_range || current_projectile.body.hitbox.y > MAP_SIZE_IN_UNITS + beyond_range){
			delete_projectile(p_index); // Projectile has gonna way too much outside map
		}
	}
}

void delete_projectile(int index){
	int temp_index = index;
	projectile_count--;
	
	while(temp_index < projectile_count){
		projectile_list[temp_index] = projectile_list[temp_index + 1];
		temp_index++;
	}
	projectile_list[projectile_count] = (ProjectileObject){0};
}

void restart_projectiles(){
	int index = projectile_count;
	while(index >= 0) delete_projectile(index--);
	projectile_count = 0;
}

// Draws projectile_list
void draw_projectiles(){
	BeginTextureMode(pixelated_screen);
	
	for(int p_index = 0; p_index < projectile_count; p_index++){
		if(!(projectile_list[p_index]).is_active || !(projectile_list[p_index]).is_moving) continue;
		
		ProjectileObject current_projectile = projectile_list[p_index];
		Vector2 projectile_head_position = (Vector2){current_projectile.body.hitbox.x, current_projectile.body.hitbox.y};
		Vector2 proj_rel_head_position = v_position_rel_to_camera(projectile_head_position);
		proj_rel_head_position = Vector2Scale(proj_rel_head_position, PIXEL_PER_UNITS/CAMERA_ZOOM_FACTOR);
		// 0.5 to round to nearest grid aligned pixel
		DrawRectangle(proj_rel_head_position.x + 0.5, proj_rel_head_position.y + 0.5, 2, 2, CYAN_BLUE); // Tip of arrow
		
		Vector2 projectile_direction = Vector2Normalize(current_projectile.body.velocity_vector);
		
		Vector2 projectile_end_position = (Vector2){projectile_head_position.x - projectile_direction.x * ARROW_VISUAL_LENGTH,  // Arrow visual length
													projectile_head_position.y - projectile_direction.y * ARROW_VISUAL_LENGTH};
		Vector2 proj_rel_end_position =  v_position_rel_to_camera(projectile_end_position);
		proj_rel_end_position = Vector2Scale(proj_rel_end_position, PIXEL_PER_UNITS/CAMERA_ZOOM_FACTOR);

		DrawLine(proj_rel_end_position.x + 0.5, proj_rel_end_position.y + 0.5,
				proj_rel_head_position.x + 0.5, proj_rel_head_position.y + 0.5, DARK_CYAN); // Body of the arrow
		
	}
	EndTextureMode();

	# ifdef DRAW_HITBOXES
	for(int p_index = 0; p_index < projectile_count; p_index++){
		if(!(projectile_list[p_index]).is_active) continue;
		ProjectileObject current_projectile = projectile_list[p_index];
		Rectangle rel_p_hitbox = position_rel_to_camera(current_projectile.body.hitbox);
		DrawRectangleLines(rel_p_hitbox.x, rel_p_hitbox.y, rel_p_hitbox.width, rel_p_hitbox.height, WHITE);
	}
	# endif
}


// -1 to not delete projectile (such as in hook)
void update_enemy_collision_projectile(Enemy *e, ProjectileObject *p, int proj_index){
	if(e->is_attacking && (proj_index != -1)) return; 	// Return if it's not the hook and enemy is attacking
														// (Skeleton should not damage itself with arrows)
	if(!e->is_active) return;
	if(!p->is_moving) return;
	if(!p->is_active) return;
	if(!CheckCollisionRecs(e->body.hitbox, p->body.hitbox)) return; 
	
	// Enemy was hit with projectile while not attacking!
	enemy_was_hit_update(e);
	if(proj_index != -1) delete_projectile(proj_index);
	
}