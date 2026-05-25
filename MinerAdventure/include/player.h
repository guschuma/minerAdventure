#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"
#include "gameObjects.h"

#define PLAYER_HEALTH 3

extern PlayerObject player; // Variavel player é definida em player.c
extern bool is_player_on_ladder;
void init_player(void);

// Pega novos valores de velocidade e posicao do jogador
void update_player_position();

void update_player_collision_enemy(Enemy *e);
void update_player_collision_projectile(ProjectileObject *p, int p_index);

void draw_hook_textures();
void draw_player(void);

// Animation functions
double time_on_air();

bool is_walk_animation_timer_active();


void draw_lifes(int health);

void player_was_hit();
void player_got_life();