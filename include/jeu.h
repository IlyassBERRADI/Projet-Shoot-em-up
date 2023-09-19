#ifndef JEU_H
#define JEU_H

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define WIDTH_PLAYER 100
#define HEIGHT_PLAYER 120

#define WIDTH_FIRE 50
#define HEIGHT_FIRE 50


#include <MLV/MLV_all.h>


typedef struct _fire
{
    int x;
    int y;
    MLV_Image* positions[4];
    struct _fire* next;
} Fire;

typedef struct Heart {
    int x;
    int y;
    int width;
    int height;
    MLV_Image* image;
    struct Heart* next;
} Heart;


typedef struct _player{
    int nb_lives;
    int x;
    int y;
    Fire* shots;
    int width;
    int height;
    int is_fire_shot;
    int time_shoot;
    MLV_Image* positions[3];
    int score;  
    Heart* hearts;
    int kill_enemies;
} Player;



typedef struct _enemy{
    int nb_lives;
    int x;
    int y;
    int right;
    int left;
    int width;
    int height;
    int fires_shot;
    int time_shoot;
    Fire* enemy_fire;
    MLV_Image* positions[3];
} Enemy;


void free_fire(Fire* fire);
void free_player(Player* new_player);
void free_enemy(Enemy* new_enemy);

int enemy_present(Enemy* new_enemy);
void change_direction(Enemy* new_enemy);
int enemy_disappears(Enemy*** enemies_ptr, int *time_passed, int* duration, Player* new_player);
int enemy_timer(int *duration);

void check_collision_fire_enemy(Fire* fire, Enemy** enemies, int num_enemies, Player* new_player);
void check_collision_player_enemy(Player* player, Enemy** enemies, int num_enemies);
void check_collision_enemy_fire_player(Enemy** enemies, int num_enemies, Player* player);
void check_collision_player_heart(Player* player);
void update_hearts(Player* player);

void write_score_to_file(Player* player);

#endif /* JEU_H */