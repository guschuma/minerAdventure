#include "raylib.h"
#include "raymath.h"

#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 640
#define MAP_SIZE 5

int cell_size_x = WIDTH_SCREEN / (MAP_SIZE);
int cell_size_y = HEIGHT_SCREEN / (MAP_SIZE);

const int floor_height = 60;
const int player_size = 50;
bool is_grounded = false;
const float gravity = 0.2;
float vertical_velocity = 0;
float horiz_velocity = 0;
const float jump_velocity = 10;
Vector2 position = {WIDTH_SCREEN/2, HEIGHT_SCREEN/2};

Texture2D sprite_generico;

// Nota para mim mesmo: CMD + Alt + I para Debug
void updateDrawFrame(void);
int main(void) { 
	InitWindow(WIDTH_SCREEN, HEIGHT_SCREEN, "Template-4.0.0");
	SetTargetFPS(60);

	Image imagem = LoadImage("assets/block.png");  // Load image data into CPU memory (RAM)
    sprite_generico = LoadTextureFromImage(imagem);       // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(imagem);                                    // Unload image data from CPU memory (RAM)

	while (!WindowShouldClose()) {
		updateDrawFrame();
	}

	CloseWindow();

	return 0;
}

char mapa[MAP_SIZE][MAP_SIZE] = {
	{' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' '},
	{'X', ' ', ' ', 'X', 'X'},
	{' ', 'X', ' ', ' ', 'X'},
	{'X', 'X', 'X', 'X', 'X'},
};

float arredonde(float n){
	return 4.0 * (int)((n) / 4);
}

void updateDrawFrame(void){
	// Event Handling
	if(IsKeyDown(KEY_UP) && is_grounded){
		vertical_velocity -= jump_velocity; 
		is_grounded = false;
	}
	bool moving_horiz = false;
	if(IsKeyDown(KEY_LEFT)){
		horiz_velocity = -5;
		moving_horiz = true;
	}
	if(IsKeyDown(KEY_RIGHT)){
		horiz_velocity = 5;
		moving_horiz = true;
	}

	
	// Updating position
	if(!is_grounded) {
		vertical_velocity += gravity;
	}
	is_grounded = false;
	if(moving_horiz) position.x += horiz_velocity;
	else horiz_velocity = 0;
	Rectangle player_rec = {position.x , position.y, player_size, player_size};
	for(int i = 0; i < MAP_SIZE; i++){
		for(int j = 0; j < MAP_SIZE; j++){
			if(mapa[i][j] == ' ') continue;

			Vector2 object_position = {cell_size_x * (j), cell_size_y * (i)};
			Rectangle object = {object_position.x, object_position.y, cell_size_x, cell_size_y};

			if(CheckCollisionRecs(object, player_rec)){
				if(horiz_velocity > 0) {
					horiz_velocity = 0;
					player_rec.x = object_position.x - player_size;
					position.x = object_position.x - player_size;
				}

				if(horiz_velocity < 0) {
					horiz_velocity = 0;
					player_rec.x = object_position.x + cell_size_x;
					position.x = object_position.x + cell_size_x;
				}
			}
		}
	}
	
	position.y += vertical_velocity;
	player_rec.y = position.y;

	for(int i = 0; i < MAP_SIZE; i++){
		for(int j = 0; j < MAP_SIZE; j++){
			if(mapa[i][j] == ' ') continue;

			Vector2 object_position = {cell_size_x * (j), cell_size_y * (i)};
			Rectangle object = {object_position.x, object_position.y, cell_size_x, cell_size_y};

			if(CheckCollisionRecs(object, player_rec)){
				if(vertical_velocity > 0) {
					vertical_velocity = 0;
					player_rec.y = object_position.y - player_size;
					position.y = object_position.y - player_size;
					is_grounded = true;
				}

				if(vertical_velocity < 0) {
					vertical_velocity = 0;
					player_rec.y = object_position.y + cell_size_y;
					position.y = object_position.y + cell_size_y;
				}
			}
		}
	}



	

	
	// Drawing objects
	BeginDrawing();
	ClearBackground(DARKBROWN);

	for(int i = 0; i < MAP_SIZE; i++){
		for(int j = 0; j < MAP_SIZE; j++){
			if(mapa[i][j] == ' ') continue;

			Vector2 object_position = {cell_size_x * (j), cell_size_y * (i)};
			object_position.x = arredonde(object_position.x);
			object_position.y = arredonde(object_position.y);
			Rectangle object = {object_position.x, object_position.y, cell_size_x, cell_size_y};
			
			DrawTexturePro(sprite_generico, (Rectangle){0, 0, 16, 16}, object, (Vector2){0, 0}, 0, WHITE);
		}
	}


	Vector2 player_object_light = {arredonde(position.x) + player_size * 0.5, arredonde(position.y) + player_size * 0.5};
	DrawRectangle(arredonde(position.x), arredonde(position.y), player_size, player_size, RED);
	DrawCircleGradient(player_object_light.x, player_object_light.y, player_size * 3, (Color){255, 255, 210, 100}, (Color){0, 0, 0, 0});
	
	
	EndDrawing();
}