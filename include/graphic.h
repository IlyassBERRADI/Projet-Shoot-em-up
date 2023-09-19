#ifndef GRAPHIC_H
#define GRAPHIC_H
#include <MLV/MLV_all.h>
#include <time.h>

#include "jeu.h"



Heart* create_new_heart();
void draw_player(int index, Player* new_player, int up, int down, int right, int left);
MLV_Image* initialize_image (char* image_path, int width, int heigth);
void draw_frame(int pos, int pos2, int index, Player* new_player, int *duration, Enemy*** enemies_ptr, int *time_passed, int start_time);
Player* initialize_player(int width, int height, int lives);
void move_player(int index, Player* new_player);
void draw_enemy(int index, Enemy* new_enemy, int up, int down, int right, int left);
Enemy* initialize_enemy(int x, int width, int height, int lives, int right, int left);
Enemy** enemy_appears();

void move_enemy(Enemy** enemies, int* time_passed, int index);


void generate_enemy(int *duration, Enemy*** enemies_ptr, int *time_passed, int index, Player* new_player);
Fire* initialize_fire_enemy(Enemy* new_enemy);
void draw_fire_enemy(int index, int up, int down, Enemy* new_enemy, int time_passed);


Fire* initialize_fire_player(Player* new_player);
void draw_fire_player(int index, int up, int down, Player* new_player);

void draw_UI(Player* player, int start_time);

#endif 