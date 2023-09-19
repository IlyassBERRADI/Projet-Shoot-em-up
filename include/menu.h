#ifndef MENU_H
#define MENU_H

#include <MLV/MLV_all.h>
#include "jeu.h"
#define MENU_STATE_MAIN 0

int init_menu(int  WIDTH, int HEIGHT );
void draw_button(int x, int y, int w, int h, char *text, MLV_Font *font, int mouse_x, int mouse_y);

#endif /* MENU_H */