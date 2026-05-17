#pragma once // Inclua esse header apenas uma vez
#include "gameObjects.h"

extern int enemy_count;			// Defined in enemy.h
extern int active_enemy_count;
extern Enemy enemy_list[MAX_ENEMY_NUMBER];

void restart_enemies();
void init_enemy(Vector2 global_position, EnemyType type);
void update_enemy(Enemy *e);
void update_enemy_texture(Enemy *e);
void draw_enemy(Enemy *e);

void initialize_arrow(Vector2 initial_position);
void draw_projectiles();
void update_projectile_positions();
void delete_projectile(int index);
void restart_projectiles();

void enemy_was_hit_update(Enemy *e);
void update_enemy_collision_projectile(Enemy *e, ProjectileObject *p, int proj_index);