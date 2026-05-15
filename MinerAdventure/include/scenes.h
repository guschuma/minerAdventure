#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"

extern bool in_up_elevator_cutscene;
extern bool in_down_elevator_cutscene;
extern bool in_defeat_cutscene;

extern int current_up_elevator_frame;
extern int current_down_elevator_frame;

extern bool is_frame_frozen;
extern Rectangle end_elevator_rec;
extern Rectangle start_elevator_rec;

extern Texture2D defeat_screen;

extern bool is_on_pause_screen;

typedef enum {LEFT_ALIGNED, CENTERED, RIGHT_ALIGNED} POS_ON_SCREEN; // Used when writing text
typedef enum {DEFEAT_SCREEN, START_SCREEN, GAMEPLAY_SCREEN, PAUSE_SCREEN, SCORE_SCREEN} SCREENS;
extern SCREENS current_screen;

void enter_level_cutscene(void);
void exit_level_cutscene(void);
void defeated_cutscene(void);

void on_defeated_screen();
void on_pause_screen();
void on_start_screen();
void on_score_screen();
Rectangle draw_offset_menu(Texture2D layer_texture, float scalar, float screen_scalar, bool darker);