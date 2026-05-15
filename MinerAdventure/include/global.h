#pragma once // Inclua esse header apenas uma vez


//#define USE_GLOBAL_CAMERA
//#define DRAW_HITBOXES
//#define DEBUG_ACTIVE // Press R to respawn

#ifdef USE_GLOBAL_CAMERA
#define CAMERA_ZOOM_FACTOR 1
#else
#define CAMERA_ZOOM_FACTOR 4
#endif

//Old #define GRID_CELL_SIZE 16
//Also BLOCK_SIZE
#define PIXELS_PER_BLOCK ((float)16.0)

// 640   40
#define SCREEN_W (50 * PIXELS_PER_BLOCK)
// 512    
#define SCREEN_H (32 * PIXELS_PER_BLOCK)

#define MAP_SIZE_IN_BLOCKS 30
#define BORDER_SIZE_W 1
#define BORDER_SIZE_H 1

static float min_f(float a, float b){
	return (a > b)? b : a;
}

// 480 <- Map size when shown in global camera
#define MAP_SIZE_IN_UNITS min_f(SCREEN_W - 2 * BORDER_SIZE_W * PIXELS_PER_BLOCK, SCREEN_H - 2 * BORDER_SIZE_H * PIXELS_PER_BLOCK)

// 1.0 (float)
#define UNITS_PER_PIXEL (MAP_SIZE_IN_UNITS/(MAP_SIZE_IN_BLOCKS * PIXELS_PER_BLOCK))
// 1.0 (float)
#define PIXEL_PER_UNITS (1.0/UNITS_PER_PIXEL)

// 40 <- Map width in blocks when shown in global camera
#define GRID_SCREEN_W (SCREEN_W / PIXELS_PER_BLOCK)
// 32 <- Map height in blocks when shown in global camera
#define GRID_SCREEN_H (SCREEN_H / PIXELS_PER_BLOCK)

// 16.0
#define UNITS_PER_BLOCK (PIXELS_PER_BLOCK * UNITS_PER_PIXEL)

#define GLOBAL_CAMERA_OFFSET_W_UNITS (0.5 * (SCREEN_W - MAP_SIZE_IN_UNITS) - BORDER_SIZE_W * UNITS_PER_BLOCK)
#define GLOBAL_CAMERA_OFFSET_H_UNITS (0.5 * (SCREEN_H - MAP_SIZE_IN_UNITS) - BORDER_SIZE_H * UNITS_PER_BLOCK) 

extern RenderTexture2D pixelated_screen;
extern RenderTexture2D pixelated_background;
extern Texture2D skeleton_sprite;
extern Texture2D snake_sprite;
extern Vector2 player_starting_position; // Defined in mapa.c

extern Texture2D player_life_sprite;
extern Texture2D player_running_sprites;
extern Texture2D player_jumping_sprites;
extern Texture2D elevator_up;           // Defined in scenes.c
extern Texture2D elevator_up_idle;
extern Texture2D elevator_down;         // Defined in scenes.c
extern Texture2D elevator_down_idle;    // Defined in scenes.c

extern Texture2D block_sprite;


typedef enum {HORIZO, VERTIC} Axis;


// Defining color pallete

#define VERY_VERY_DARK_PURPLE (Color){25, 11, 37, 255}
#define VERY_DARK_PURPLE (Color){27, 17, 44, 255}
#define DARK_PURPLE (Color){42, 20, 61, 255}
#define GPURPLE (Color){71, 25, 93, 255}
#define GMAGENTA (Color){115, 31, 122, 255}
#define WINE (Color){158, 32, 131, 255}
#define DARK_PINK (Color){182, 63, 131, 255}
#define GPINK (Color){200, 101, 140, 255}
#define LIGHT_PINK (Color){221, 141, 159, 255}
#define LIGHT_BLUE (Color){110, 189, 234, 255}
#define GBLUE (Color){107, 155, 207, 255}
#define CYAN_BLUE (Color){102, 125, 180, 255}
#define DARK_CYAN (Color){91, 95, 144, 255}
#define VERY_DARK_CYAN (Color){84, 76, 112, 255}