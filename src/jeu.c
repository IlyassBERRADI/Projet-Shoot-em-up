#include "../include/jeu.h"
#include <MLV/MLV_all.h>

#define LIFE_WEIGHT 100
#define ENEMY_WEIGHT 50
#define TIME_WEIGHT 1

/* Écrit le score du joueur dans un fichier */
void write_score_to_file(Player* player) {
    FILE* file = fopen("scores.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier des scores.\n");
        return;
    }
    int new_score = player->score + player->nb_lives * LIFE_WEIGHT  + player->kill_enemies * ENEMY_WEIGHT;
    fprintf(file, "Nouveau Score: %d, Score Initial: %d, Vies restantes: %d, Ennemis tués: %d\n",
            new_score, player->score, player->nb_lives, player->kill_enemies);
    fclose(file);
}

/* Libère la mémoire associée à un objet Fire */
void free_fire(Fire* fire) {
    while (fire != NULL) {
        Fire* next_fire = fire->next; 
        int i; 
        for ( i = 0; i < 4; i++) {
            if (fire->positions[i] != NULL) {
                MLV_free_image(fire->positions[i]);
            }
        }
        free(fire); 
        fire = next_fire;
    }
}

/* Libère la mémoire associée à un objet Player */
void free_player(Player* new_player){
    if (new_player == NULL) {
        return;
    }
    free_fire(new_player->shots); 
    int i; 
    for ( i = 0; i < 3; i++) {
        if(new_player->positions[i] != NULL){
            MLV_free_image(new_player->positions[i]);
        }
    }
    free(new_player);
}

/* Libère la mémoire associée à un objet Enemy */
void free_enemy(Enemy* new_enemy) {
    if (new_enemy == NULL) {
        fprintf(stderr, "new_enemy est NULL.\n");
        return;
    }
    if (new_enemy->enemy_fire != NULL) {
        int i;
        for(i=0; i<4; i++) {
            if(new_enemy->enemy_fire->positions[i] != NULL) {
                MLV_free_image(new_enemy->enemy_fire->positions[i]);
            }
        }
        free(new_enemy->enemy_fire);
    }
    int i; 
    for(i=0; i<3; i++) {
        if(new_enemy->positions[i] != NULL) {
            MLV_free_image(new_enemy->positions[i]);
        }
    }
    free(new_enemy);
}

int enemy_present(Enemy* new_enemy){
    if (new_enemy != NULL && new_enemy->y<0) 
    {
        return 0;
    }
    else
    {
        return 1;
    }
    
}


//Quand l'ennemi arrive au bout de l'écran, on le bouge à l'autre côté
void change_direction(Enemy* new_enemy){
    if (new_enemy->x>=WINDOW_WIDTH-WIDTH_PLAYER)
    {
        new_enemy->right=0;
        new_enemy->left=1;
    }
    else if(new_enemy->x<=0)
    {
        new_enemy->right=1;
        new_enemy->left=0;
    }
    
}

void update_hearts(Player* player) {
    Heart* current_heart = player->hearts;
    Heart* previous_heart = NULL;
    while (current_heart != NULL) {
        // Si le coeur est en dehors de la fenêtre, le libérer
        if (current_heart->y > WINDOW_HEIGHT) {
            // Libérer l'image du coeur
            MLV_free_image(current_heart->image);
            // Si le coeur à supprimer est le premier de la liste
            if (previous_heart == NULL) {
                player->hearts = current_heart->next;
            } else {
                previous_heart->next = current_heart->next;
            }
            Heart* heart_to_free = current_heart;
            current_heart = current_heart->next;
            // Libérer le coeur lui-même
            free(heart_to_free);
        } else {
            previous_heart = current_heart;
            current_heart = current_heart->next;
        }
    }
}

//Si l'ennemi arrive au bas de l'écran, on désalloue sa mémoire
int enemy_disappears(Enemy*** enemies_ptr, int *time_passed, int* duration, Player* new_player){
    int i;
    
    
    if ((*enemies_ptr)[0] == NULL) //
    {
        
        //cas : si tous les enemies sont détruit, mettre tableau enemi à nul, afin que la fonction generate_enemy soit activé
        (*enemies_ptr)=NULL; 
       
        *time_passed=0;
        *duration=0;
        return 1;
    }
    if ((*enemies_ptr)[0] != NULL && ((*enemies_ptr)[0]) ->y>WINDOW_HEIGHT) //
    {
        for ( i = 0; i < 2; i++)
        {
            if ((*enemies_ptr)[i])
            {
                
                free_enemy((*enemies_ptr)[i]);
                (*enemies_ptr)[i] = NULL;
            }
            
        }
        // Réallouez la mémoire
        (*enemies_ptr)=NULL;
        *time_passed=0;
        *duration=0;
        new_player->score -= 10;
        return 1;
    }
    else{
        return 0;
    }
}



// Gère le timing d'apparition des ennemis
int enemy_timer(int *duration){
    if (*duration<500)  
    {
        (*duration)++;
        return 0;
    }
    else
    {
        return 1;
    }
    
    
}

int collides_with(int x1, int y1, int width1, int height1,
                  int x2, int y2, int width2, int height2) {
    if (x1 < x2 + width2 &&
        x1 + width1 > x2 &&
        y1 < y2 + height2 &&
        y1 + height1 > y2) {
        // Les rectangles se chevauchent
        return 1;
    } else {
        // Les rectangles ne se chevauchent pas
        return 0;
    }
}

Fire* delete_fire(Fire* current_fire, Fire** enemy_fire) {
    // Supprimer le tir de l'ennemi
    if (current_fire == *enemy_fire) {
        *enemy_fire = current_fire->next;
        free(current_fire);
        current_fire = *enemy_fire;
    } else {
        Fire* previous_fire = *enemy_fire;
        while (previous_fire->next != current_fire) {
            previous_fire = previous_fire->next;
        }
        previous_fire->next = current_fire->next;
        free(current_fire);
        current_fire = previous_fire->next;
    }
    return current_fire;
}


void game_over(Player* player) {
    char score_str[50];
    // Affiche un message à l'écran indiquant que le joueur a perdu + score
    MLV_clear_window(MLV_COLOR_BLACK);

    sprintf(score_str, "Score: %d", player->score);
    MLV_draw_text(10, 10, score_str, MLV_COLOR_WHITE);
    MLV_draw_text(
        WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
        "Vous avez perdu ! Appuyez sur n'importe quelle touche pour quitter...",
        MLV_COLOR_WHITE
    );
    MLV_actualise_window();

    // Écrire le score final dans le fichier
    write_score_to_file(player);
    printf("Fichier de score MAJ !");

    // Attendre que l'utilisateur appuie sur une touche pour quitter
    while(1) {
        MLV_Keyboard_button touche;
        MLV_wait_keyboard(&touche, NULL, NULL);

        if(touche != MLV_KEYBOARD_NONE) {
            break;
        }
    }

    // Libérer les ressources et quitter le programme
    MLV_free_window();
    exit(0);
}



void check_collision_fire_enemy(Fire* fire, Enemy** enemies, int num_enemies, Player* new_player) {
    Fire* current_fire = fire;
    int i;

    if (enemies == NULL) {
        return;
    }

    
    while (current_fire != NULL) {
        for (i = 0; i < num_enemies; i++) {
            if (enemies[i] == NULL) {
                continue;
            }
        
            if (current_fire != NULL && enemies[i] != NULL && //si current fire est null on laisse tomber, pareil pour enemies[i]
                current_fire->x < enemies[i]->x + enemies[i]->width &&
                current_fire->x + WIDTH_FIRE > enemies[i]->x &&
                current_fire->y < enemies[i]->y + enemies[i]->height &&
                current_fire->y + HEIGHT_FIRE > enemies[i]->y) {
                
                
                // Effacer graphiquement l'ennemi touché
                MLV_draw_filled_rectangle(enemies[i]->x, enemies[i]->y, enemies[i]->width, enemies[i]->height, MLV_COLOR_BLACK);
                
                // Effacer graphiquement le tir du joueur
                MLV_draw_filled_rectangle(current_fire->x, current_fire->y, WIDTH_FIRE, HEIGHT_FIRE, MLV_COLOR_BLACK);
                MLV_actualise_window();
                // Réduire la vie de l'ennemi touché 
                enemies[i]->nb_lives--;
                
                // Vérifier si l'ennemi n'a plus de vie
                if (enemies[i]->nb_lives <= 0) {
                    // Le désallouer de la mémoire
                    
                    free_enemy(enemies[i]);
                    enemies[i] = NULL;

                    // Ajout des points au score du joueur + augmentation ennemie tue
                    new_player->score += 50;
                    new_player->kill_enemies++;
                }
                
                // Supprimer le tir du joueur
                Fire* next_fire = current_fire->next; // Sauvegarder le next_fire avant de le libérer
                if (current_fire == fire) {
                    new_player->shots = next_fire;
                    free(current_fire);
                    current_fire = new_player->shots;
                } else {
                    Fire* previous_fire = new_player->shots;
                    while (previous_fire->next != current_fire) {
                        previous_fire = previous_fire->next;
                    }
                    previous_fire->next = next_fire;
                    free(current_fire);
                    current_fire = next_fire;
                }

                
            }
        }
        current_fire = (current_fire != NULL) ? current_fire->next : NULL;
    }

}

// Vérifie si le joueur a une collision avec un ennemi
void check_collision_player_enemy(Player* player, Enemy** enemies, int num_enemies) {
    // Pas besoin de déclarer i ici
    if (enemies == NULL) {
        return;
    }

    // Utilisation d'une boucle for sans déclaration de i
    for (int i = 0; i < num_enemies; i++) {
        if (enemies[i] == NULL) {
            continue;
        }
    
        if (enemies[i] != NULL && // si enemies[i] est null on laisse tomber
            player->x < enemies[i]->x + enemies[i]->width &&
            player->x + player->width > enemies[i]->x &&
            player->y < enemies[i]->y + enemies[i]->height &&
            player->y + player->height > enemies[i]->y) {
            
            // Effacer graphiquement l'ennemi
            MLV_draw_filled_rectangle(enemies[i]->x, enemies[i]->y, enemies[i]->width, enemies[i]->height, MLV_COLOR_BLACK);
            
            // Réduire la vie du joueur
            player->nb_lives--;

            // Ajout des points au score du joueur
            player->score -= 50;
            
            // Vérifier si le joueur n'a plus de vie
            if (player->nb_lives <= 0) {
                    // Affiche un message à l'écran indiquant que le joueur a perdu
                    // et écrit le score final dans le fichier
                    // L'opération d'écriture du score dans le fichier est supposée être définie ailleurs
                  game_over(player);
                }
            
            // Le désallouer de la mémoire
            free_enemy(enemies[i]);
            enemies[i] = NULL;
        }
    }
}

// Vérifie si le joueur a une collision avec le feu d'un ennemi
void check_collision_enemy_fire_player(Enemy** enemies, int num_enemies, Player* player) {
    int i = 0;
    if (enemies == NULL) {
        return;
    }

    for (i = 0; i < num_enemies; i++) {
        if (enemies[i] == NULL) {
            continue;
        }
        
        Fire* current_fire = enemies[i]->enemy_fire;
        
        while (current_fire != NULL) {
            if (player != NULL && //si player est null on laisse tomber
                current_fire->x < player->x + player->width &&
                current_fire->x + WIDTH_FIRE > player->x &&
                current_fire->y < player->y + player->height &&
                current_fire->y + HEIGHT_FIRE > player->y) {
                int enemies_dead;
                // Effacer graphiquement le tir de l'ennemi
                MLV_draw_filled_rectangle(current_fire->x, current_fire->y, WIDTH_FIRE, HEIGHT_FIRE, MLV_COLOR_BLACK);
                MLV_actualise_window();
                
                // Réduire la vie du joueur
                player->nb_lives--;
                
                // Vérifier si le joueur n'a plus de vie
                if (player->nb_lives <= 0) {
                    // Affiche un message à l'écran indiquant que le joueur a perdu
                    // et écrit le score final dans le fichier
                    // L'opération d'écriture du score dans le fichier est supposée être définie ailleurs
                    game_over(player);
                }
                
                // Supprimer le tir de l'ennemi
                current_fire = delete_fire(current_fire, &(enemies[i]->enemy_fire));
            } else if (current_fire != NULL) {
                current_fire = current_fire->next;
            }
        }
    }
}

void check_collision_player_heart(Player* player) {
    Heart** current_heart = &(player->hearts);
    while (*current_heart != NULL) {
        if (collides_with((*current_heart)->x, (*current_heart)->y, (*current_heart)->width, (*current_heart)->height,
                          player->x, player->y, player->width, player->height)) {
            // augmente vie de la liste
            player->nb_lives += 1;

            // supprime coeur de la liste
            Heart* heart_to_remove = *current_heart;
            *current_heart = (*current_heart)->next;
            MLV_free_image(heart_to_remove->image);
            free(heart_to_remove);
        } else {
            current_heart = &((*current_heart)->next);
        }
    }
}
