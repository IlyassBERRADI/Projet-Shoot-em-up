#include <MLV/MLV_all.h>
#include "../include/jeu.h"
#include "../include/graphic.h" 
#include "../include/menu.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

int main(int argc, char *argv[]){
    

    MLV_create_window("Godzilla vs King Dragon", "Godzilla vs King Dragon", WINDOW_WIDTH, WINDOW_HEIGHT); /*  Cree la fenetre de jeu */

    /* Appel de la fonction init_menu() pour afficher le menu */
    int play;
    play = init_menu(WINDOW_WIDTH, WINDOW_HEIGHT);

    if(play==0)
    {
        exit(0);
    }
    if(play==1)
    {
        // Initialisation du joueur
        Player* new_player = initialize_player(WIDTH_PLAYER, HEIGHT_PLAYER, 5);
        if (new_player == NULL) {
            fprintf(stderr, "Erreur d'initialisation du joueur.\n");
            return 1;
        }
        
        int cpt=0;
        int cpt2=0;
        int index=0;
        Enemy** enemies=NULL;
        int time_passed=0;
        int duration=0;
        int game_over = 0;
        int start_time = MLV_get_time();
        
        while (!game_over)
        {
            // Gestion des pauses et de la fin du jeu
            if (MLV_get_keyboard_state(MLV_KEYBOARD_p) == MLV_PRESSED) {
                while (1) {
                     MLV_wait_milliseconds(100);
                    if (MLV_get_keyboard_state(MLV_KEYBOARD_p) == MLV_PRESSED) {
                        break;
                    }
                }
            }
            if (MLV_get_keyboard_state(MLV_KEYBOARD_q) == MLV_PRESSED) {
                game_over = 1;
                break;
            }
            
            // Gestion des dragons
            if (cpt%150==0)
            {
                index++;
            }

            // Création des cœurs
            if (cpt % 2500 == 0) {
                Heart* new_heart = create_new_heart();
                new_heart->next = new_player->hearts;
                new_player->hearts = new_heart;
            }
            
            // Dessiner les éléments de jeu
            draw_frame(cpt, cpt2, index, new_player, &duration, &enemies, &time_passed, start_time);

            cpt+=2;
            cpt2++;
        }
                    
        // Écrire le score final dans le fichier
        write_score_to_file(new_player);

        MLV_wait_seconds(2);

        // Libération de la mémoire
        free_player(new_player);
        if (enemies != NULL) {
            free_enemy(enemies);
        }
        MLV_free_window(); /* Libérer les ressources de la fenêtre */
        

        return 0;
    }
}
