#pragma once // Inclua esse header apenas uma vez
#include "raylib.h"
#include "global.h"
#include "mapa.h"
#define DT 1.0/60
#define GRAVITY_ACCEL (7 * UNITS_PER_PIXEL)      // units por segundo^2    //7 normally
#define MAX_ENEMY_NUMBER 20
#define MAX_PROJECTILE_NUMBER 30

typedef enum {LEFT, RIGHT} FacingDirection;

typedef struct {
	Vector2 position;
	Rectangle hitbox;            // Hitbox do objeto
	Vector2 velocity_vector;     // Velocidade do objeto
} PhysicsBody;

typedef struct {
	PhysicsBody body; 
	Texture2D texture;           // Textura do ser
	Rectangle texture_rect;
	FacingDirection direction;   // Sentido para qual ser está mirando
	float velocity;              // Velocidade da caminhada
	bool is_grounded;			 // True se o ser está encostado no chão
	bool was_hit_animation;
	int health;
	Color tint;
} PlayerObject;

extern PlayerObject player;

typedef struct {
	PhysicsBody body;
	float velocity;              // Velocidade da caminhada
	bool is_active;				 // Projétil está ativo?
	bool is_moving;				 // Projétil está se mexendo?
} ProjectileObject;
extern ProjectileObject hook;
typedef enum {
    ENEMY_SNAKE,
	ENEMY_SKELETON
} EnemyType;

typedef struct {
    PhysicsBody body;
    int health;
    FacingDirection direction;   // Sentido para qual ser está mirando
    EnemyType type;
	float velocity;              // Velocidade da caminhada
	Texture2D texture;
	Rectangle texture_rect;
	int animation_index;
	double time_since_last_animation, time_since_last_attack;
	bool is_active, is_grounded, was_previously_grounded, was_hit, is_attacking;
	Color enemy_tint;
} Enemy;

extern int enemy_count;			// Defined in enemy.h
extern Enemy enemy_list[MAX_ENEMY_NUMBER];

extern int projectile_count;			// Defined in enemy.h
extern ProjectileObject projectile_list[MAX_PROJECTILE_NUMBER];

Rectangle position_rel_to_camera(Rectangle global_pos_rec);
Rectangle camera_to_global(Rectangle relative_pos_rec);

Vector2 v_position_rel_to_camera(Vector2 global_pos_vec);
Vector2 v_camera_to_global(Vector2 relative_pos_vec);

void set_camera_position_to(Vector2 global_position);
void move_camera_position_to(Vector2 global_position);

void update_body_position(Vector2 new_position, PhysicsBody *b);
void update_projectile_position(ProjectileObject *p, bool has_gravity);

float to_pixel_grid(float x);
Vector2 v_to_pixel_grid(Vector2 v);
// Testa se posicão atual mais a nova velocidade colide com parede em eixo vertical ou horizontal.
// Se sim, velocidade naquele eixo deve ser zero e retorne a posicão corrigida (colidida com a parede)
float collision_detect_blocks(float *new_velocity, Axis ax, PhysicsBody *b, bool *hit_ground);

void load_textures();
void load_background();

void draw_cursor_texture();