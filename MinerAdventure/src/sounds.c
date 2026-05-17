#include "sounds.h"
Sound transition_up;
Sound transition_down;
Sound jump_sfx;
Sound dash_sfx;
Sound hook_l_sfx;
Sound zipper_sfx;
Sound bow_sfx;
Sound hit_enemy;
Sound footstep;
Sound up_elevator;
Sound down_elevator;
Sound hit_player;
Sound button_select;
Sound life_up;
void init_sounds(){
	transition_up = LoadSound("assets/sound-effects/transition_up.mp3");
	SetSoundVolume(transition_up, 0.6);
	transition_down = LoadSound("assets/sound-effects/transition_down.mp3");
	SetSoundVolume(transition_down, 0.6);
	jump_sfx = LoadSound("assets/sound-effects/jump.mp3");
	SetSoundVolume(jump_sfx, 2);
	dash_sfx = LoadSound("assets/sound-effects/dash_sound.mp3");
	SetSoundVolume(dash_sfx, 15);
	hook_l_sfx = LoadSound("assets/sound-effects/hook_land.mp3");
	SetSoundVolume(hook_l_sfx, 2.4);
	zipper_sfx = LoadSound("assets/sound-effects/zipper.mp3");
	SetSoundVolume(zipper_sfx, 6.5);
	bow_sfx = LoadSound("assets/sound-effects/bow.mp3");
	SetSoundVolume(bow_sfx, 0.1);
	hit_enemy = LoadSound("assets/sound-effects/hit_enemy.wav");
	SetSoundVolume(hit_enemy, 0.24);
	footstep = LoadSound("assets/sound-effects/footstep.mp3");
	SetSoundVolume(footstep, 0.6);
	up_elevator = LoadSound("assets/sound-effects/up_elevator.mp3");
	SetSoundVolume(up_elevator, 0.5);
	down_elevator = LoadSound("assets/sound-effects/down_elevator.mp3");
	SetSoundVolume(down_elevator, 0.5);
	hit_player = LoadSound("assets/sound-effects/hit_player.wav");
	SetSoundVolume(hit_player, 0.24);
	button_select = LoadSound("assets/sound-effects/button_select.mp3");
	SetSoundVolume(button_select, 0.3);
	life_up = LoadSound("assets/sound-effects/life_up.mp3");
}
void play_sound(SOUND sound){
	switch(sound){
		case TRANSITION_UP_SFX:
			PlaySound(transition_up); break;
		case TRANSITION_DOWN_SFX:
			PlaySound(transition_down); break;
		case JUMP_SFX:
			PlaySound(jump_sfx); break;
		case DASH_SFX:
			PlaySound(dash_sfx); break;
		case HOOK_LAND_SFX:
			PlaySound(hook_l_sfx); break;
		case ZIPPER_SFX: // used for hook
		SetSoundPitch(zipper_sfx, 0.8 + 0.1*GetRandomValue(0, 5));
			PlaySound(zipper_sfx); break;
			
		case BOW_SFX:
			PlaySound(bow_sfx); break;
		case HIT_ENEMY:
			PlaySound(hit_enemy); break;
		case FOOTSTEP:
			PlaySound(footstep); break;
		case UP_ELEVATOR:
			PlaySound(up_elevator); break;
		case DOWN_ELEVATOR:
			PlaySound(down_elevator); break;
		case HIT_PLAYER:
			PlaySound(hit_player); break;
		case BUTTON_SELECT:
			PlaySound(button_select); break;
		case LIFE_UP:
			PlaySound(life_up); break;
	}
	return;
}

void unload_sounds(){
	UnloadSound(transition_up);
	UnloadSound(transition_down);
	UnloadSound(jump_sfx);
	UnloadSound(dash_sfx);
	UnloadSound(hook_l_sfx);
	UnloadSound(zipper_sfx);
	UnloadSound(bow_sfx);
	UnloadSound(hit_enemy);
	UnloadSound(footstep);
	UnloadSound(down_elevator);
	UnloadSound(up_elevator);
	UnloadSound(hit_player);
	UnloadSound(button_select);
	UnloadSound(life_up);
}