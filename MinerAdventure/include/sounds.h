#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"

typedef enum {TRANSITION_UP_SFX, TRANSITION_DOWN_SFX, JUMP_SFX, DASH_SFX,HOOK_LAND_SFX, ZIPPER_SFX, 
		BOW_SFX, HIT_ENEMY, FOOTSTEP, UP_ELEVATOR, DOWN_ELEVATOR, HIT_PLAYER, BUTTON_SELECT, LIFE_UP} SOUND;

void init_sounds();
void play_sound(SOUND sound);
void unload_sounds();