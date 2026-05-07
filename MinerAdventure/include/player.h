#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"
#include "gameObjects.h"

extern PlayerObject player; // Variavel player é definida em player.c

void init_player(void);

// Pega novos valores de velocidade e posicao do jogador
void update_player_position();

void update_player_collision_enemy(Enemy *e);
void update_player_collision_projectile(ProjectileObject *p, int p_index);

void draw_hook_textures();
void draw_player(void);

typedef enum {LEFTK, RIGHTK, UPK, DOWNK, SHIFTK} key;
bool isPressed(key k);

// Animation functions
double time_on_air();

bool is_walk_animation_timer_active();

double time_since_last_dash();

void player_was_hit();
