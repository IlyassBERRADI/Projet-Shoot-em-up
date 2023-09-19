#include <MLV/MLV_all.h>
#include <time.h>
#include <stdio.h>
#include "../include/menu.h"

/* Dessine un bouton avec un texte à l'intérieur */
void draw_button(int x, int y, int w, int h, char *text, MLV_Font *font, int mouse_x, int mouse_y) {
    int text_width, text_height;
    MLV_get_size_of_text(text, &text_width, &text_height);
    MLV_Color text_color = MLV_COLOR_WHITE;

    if (
        mouse_x >= x && mouse_x <= x + w &&
        mouse_y >= y && mouse_y <= y + h
    ) {
        text_color = MLV_COLOR_ORANGE1;
    }

    MLV_draw_text_box_with_font(x, y, w, h, text, font, 1, MLV_COLOR_WHITE, text_color, MLV_COLOR_NAVY_BLUE, MLV_TEXT_CENTER, MLV_TEXT_CENTER, MLV_TEXT_CENTER);
}

/* Initialise le menu du jeu */
int init_menu(int  WIDTH, int HEIGHT ) {
    int mouse_x, mouse_y;
    int button_width = 220;
    int button_height = 70;
    int button_x = WIDTH / 2 - button_width / 2;
    int button_y = 200;
    int button_y2 = 300;
    
    /* Charger la police de caractères */
    MLV_Font *font = MLV_load_font("./data/font01.ttf", 50);
    if (font == NULL) {
        fprintf(stderr, "Erreur de chargement de la police.\n");
        return -1;
    }

    int quit = 0;
    int y = 0;
    int y2 = 0;
    int x = 0;
    int x2 = 5;
    int main_menu_state = 1; /* 1 pour afficher le menu principal, 0 pour le masquer*/
    int play_state = 0;
    int pressed = 0;
    
    /* Charger les images */
    MLV_Image *background = MLV_load_image("./data/ocean01.png");
    MLV_Image *cloud = MLV_load_image("./data/cloud01.png");
    MLV_Image *logo = MLV_load_image("./data/logo.png");
    
    if (background == NULL || cloud == NULL || logo == NULL) {
        fprintf(stderr, "Erreur de chargement des images.\n");
        return -1;
    }
    
    /* Ajuster la taille des images */
    MLV_resize_image(cloud, WIDTH, HEIGHT);
    MLV_resize_image(logo, 300, 100);
    MLV_resize_image(background, WIDTH, HEIGHT);
    
    /* Boucle principale de gestion des événements */
    while (!quit) {
        /* THIS FUNCTION CALL A SINGLE TIME MLV_actualize_window */
        /* draw_window(&param, &grid);*/

        /* Code de déplacement de l'arrière-plan et des nuages */
        y = y + 1;
        y2 = y2 + 1;
        if (y >= HEIGHT) {
            y = 0;
        }
        if (y2 >= HEIGHT) {
            y2 = 0;
        }
        
        /* Dessiner le fond et les nuages */
        MLV_draw_image(background, x, y);
        MLV_draw_image(background, x, y - HEIGHT);
        MLV_draw_image(cloud, x2, y2);
        MLV_draw_image(cloud, x2, y2 - HEIGHT);

        /* Dessiner le logo */
        MLV_draw_image(logo, 100, 100);

        /* Récupérer la position de la souris */
        MLV_get_mouse_position(&mouse_x, &mouse_y);

        /* Si le menu principal est actif */
        if (main_menu_state && pressed == 0) {
            /* Dessiner les boutons Play et Leave */
            draw_button(button_x, button_y, button_width, button_height, "Play", font, mouse_x, mouse_y);
            draw_button(button_x, button_y2, button_width, button_height, "Leave", font, mouse_x, mouse_y);

            /* Si l'utilisateur clique sur le bouton Play */
            if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED &&
                mouse_x >= button_x && mouse_x <= button_x + button_width &&
                mouse_y >= button_y && mouse_y <= button_y + button_height) {
                main_menu_state = 0;
                pressed = 1;
                return play_state = 1;
            }

            /* Si l'utilisateur clique sur le bouton Leave */
            if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED &&
                mouse_x >= button_x && mouse_x <= button_x + button_width &&
                mouse_y >= button_y2 && mouse_y <= button_y2 + button_height) {
                return play_state = 0;
            }
        }
        
        /* Si l'utilisateur relâche le bouton gauche de la souris */
        if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_RELEASED) {
            pressed = 0;
        }

        /* Mettre à jour la fenêtre */
        MLV_actualise_window();
    }

    /* Libérer les ressources */
    MLV_free_image(cloud);
    MLV_free_image(background);
    MLV_free_image(logo);
    MLV_free_font(font);
}
