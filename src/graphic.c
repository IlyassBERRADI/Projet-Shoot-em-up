#include "../include/graphic.h"
#include "../include/jeu.h"
#include <MLV/MLV_all.h>

/* Fonction pour initialiser une image avec un chemin, une largeur et une hauteur donnés */
MLV_Image* initialize_image (char* image_path, int width, int height){
    MLV_Image* player_image = MLV_load_image(image_path); /* Charge l’image depuis le chemin donné */
    MLV_resize_image(player_image, width, height);
    return player_image;
}

/* Dessine le joueur en fonction de sa position et des commandes du joueur */
void draw_player(int index, Player* new_player, int up, int down, int right, int left){
    new_player->x += right - left;
    new_player->y += down - up;
    MLV_draw_image(new_player->positions[index%3], new_player->x, new_player->y);
}

/* Déplace le joueur en fonction des touches pressées */
void move_player(int index, Player* new_player){
    if (MLV_get_keyboard_state(MLV_KEYBOARD_UP) == MLV_PRESSED) {
        if (new_player->y >= 3)
            draw_player(index, new_player, 3, 0, 0, 0);
        else
            draw_player(index, new_player, 0, 0, 0, 0);
    }
    else if (MLV_get_keyboard_state(MLV_KEYBOARD_DOWN) == MLV_PRESSED){
        if (new_player->y + new_player->height <= WINDOW_HEIGHT - 3)
            draw_player(index, new_player, 0, 3, 0, 0);
        else
            draw_player(index, new_player, 0, 0, 0, 0);
    }
    else if (MLV_get_keyboard_state(MLV_KEYBOARD_RIGHT) == MLV_PRESSED){
        if (new_player->x + new_player->width <= WINDOW_WIDTH - 3)
            draw_player(index, new_player, 0, 0, 3, 0);
        else
            draw_player(index, new_player, 0, 0, 0, 0);
    }
    else if (MLV_get_keyboard_state(MLV_KEYBOARD_LEFT) == MLV_PRESSED){
        if (new_player->x >= 3)
            draw_player(index, new_player, 0, 0, 0, 3);
        else
            draw_player(index, new_player, 0, 0, 0, 0);
    }
    else {
        draw_player(index, new_player, 0, 0, 0, 0);
    }
}

/* Fonction pour créer un nouveau coeur */
Heart* create_new_heart() {
    Heart* new_heart = (Heart*)malloc(sizeof(Heart));
    new_heart->x = rand() % WINDOW_WIDTH;  // position aléatoire
    new_heart->y = 0;  // commence en haut de la fenêtre
    new_heart->width = 50;  
    new_heart->height = 50;  
    new_heart->image = MLV_load_image("./data/heart.png");  
    MLV_resize_image(new_heart->image, new_heart->width, new_heart->height);
    new_heart->next = NULL;
    return new_heart;
}

/* Initialise un objet Player */
Player* initialize_player(int width, int height, int lives){
    Player* new_player = (Player*)malloc(sizeof(Player));
    new_player->positions[0] = initialize_image("./data/player.png", WIDTH_PLAYER, HEIGHT_PLAYER);
    new_player->positions[1] = initialize_image("./data/player2.png", WIDTH_PLAYER, HEIGHT_PLAYER);
    new_player->positions[2] = initialize_image("./data/player3.png", WIDTH_PLAYER, HEIGHT_PLAYER); 
    new_player->x = WINDOW_WIDTH / 2 - WIDTH_PLAYER / 2;
    new_player->y = WINDOW_HEIGHT - HEIGHT_PLAYER;
    new_player->width = width;
    new_player->height = height;
    new_player->nb_lives = lives;
    new_player->shots = NULL;
    new_player->is_fire_shot = 0;
    new_player->time_shoot = 0;
    new_player->score = 0;
    new_player->kill_enemies = 0;

    return new_player;
}

/* Dessine le cadre de jeu et gère l'état du jeu */
void draw_frame(int pos, int pos2, int index, Player* new_player,
    int *duration, Enemy*** enemies_ptr, int *time_passed, int start_time){

    /* Charger l’image d’arrière-plan */
    MLV_Image* background_image = MLV_load_image("./data/ocean01.png");
    MLV_Image* background2_image = MLV_load_image("./data/cloud01.png");

    MLV_resize_image(background_image, WINDOW_WIDTH, WINDOW_HEIGHT);
    MLV_resize_image(background2_image, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    MLV_draw_image(background_image, 0, pos % WINDOW_HEIGHT - WINDOW_HEIGHT);
    MLV_draw_image(background_image, 0, pos % WINDOW_HEIGHT);
    MLV_draw_image(background2_image, 0, pos2 % WINDOW_HEIGHT - WINDOW_HEIGHT);
    MLV_draw_image(background2_image, 0, pos2 % WINDOW_HEIGHT);

    draw_fire_player(index, 5, 0, new_player);

    Heart* current_heart = new_player->hearts;
    while (current_heart != NULL) {
        current_heart->y += 2;  // move the heart down
        MLV_draw_image(current_heart->image, current_heart->x, current_heart->y);
        current_heart = current_heart->next;
    }
    update_hearts(new_player);

    generate_enemy(duration, enemies_ptr, time_passed, index, new_player);

    /* Vérifier les collisions */
    check_collision_player_heart(new_player);
    check_collision_fire_enemy(new_player->shots, *enemies_ptr, 2, new_player);
    check_collision_enemy_fire_player(*enemies_ptr, 2, new_player);
    check_collision_player_enemy(new_player, *enemies_ptr, 2);
    
    move_player(index, new_player);
    draw_UI(new_player, start_time);
    MLV_actualise_window();
    MLV_draw_rectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MLV_COLOR_BLACK);
    MLV_actualise_window();
    MLV_free_image(background_image);
    MLV_free_image(background2_image);
}

    
/* La fonction qui génére les ennemis */
void generate_enemy(int *duration, Enemy*** enemies_ptr, int *time_passed, int index, Player* new_player) {
    /* La génération d'ennemis semble être dupliquée. Un seul bloc suffit. */
    if (enemy_timer(duration)) {
        if (!(*enemies_ptr)) {
            *enemies_ptr = enemy_appears();
        }
        if (!enemy_disappears(enemies_ptr, time_passed, duration, new_player)) {
            move_enemy(*enemies_ptr, time_passed, index);
        }
    }
}

/* Dessine l'ennemi en fonction de sa position */
void draw_enemy(int index, Enemy* new_enemy, int up, int down, int right, int left) {
    new_enemy->x += right - left;
    new_enemy->y += down - up;
    MLV_draw_image(new_enemy->positions[index%3], new_enemy->x, new_enemy->y);
}

/* Fonction qui retourne un tableau de nouveaux ennemis */
Enemy** enemy_appears() {
    /* Initialisation du tableau d'ennemis */
    Enemy** enemies = malloc(2*sizeof(Enemy*));
    if (enemies == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le tableau d'ennemis.\n");
        return NULL;
    }
    for (int i = 0; i < 2; i++) {
        enemies[i] = NULL;
    }
    
    /* Génération de la position des ennemis */
    srand(time(NULL));
    int enemy_case = rand() % 1001;
    int enemy_pos;
    int enemy_pos2;
    int point_livea = 1;
    int point_liveb = 2;
    
    /* Attributs des ennemis en fonction de enemy_case */
    if (enemy_case%5 == 0 || enemy_case%5 == 1 || enemy_case%5 == 4) {
        enemy_pos = rand() % (WINDOW_WIDTH - WIDTH_PLAYER + 1);
        enemies[0] = initialize_enemy(enemy_pos, WIDTH_PLAYER, HEIGHT_PLAYER, point_liveb, 1, 0);
    } else {
        enemy_pos = rand() % (WINDOW_WIDTH - WIDTH_PLAYER + 1);
        do {
            enemy_pos2 = rand() % (WINDOW_WIDTH - WIDTH_PLAYER + 1);
        } while (enemy_pos2 > enemy_pos - WIDTH_PLAYER - 10 && enemy_pos2 < enemy_pos + WIDTH_PLAYER + 10);

        enemies[0] = initialize_enemy(enemy_pos, WIDTH_PLAYER, HEIGHT_PLAYER, point_livea, 1, 0);
        enemies[1] = initialize_enemy(enemy_pos2, WIDTH_PLAYER, HEIGHT_PLAYER, point_livea, 0, 1);
    }

    return enemies;
}

/* Fonction qui bouge l'ennemi */
void move_enemy(Enemy** enemies, int* time_passed, int index) {
    int speedfire01 = 3;
    int speedfire02 = 3;

    /* Boucle à travers les ennemis et les déplace */
    for (int i = 0; i < 2; i++) {
        if (enemies[i]) {
            /* Si l'ennemi est présent, le dessine et lui permet de tirer */
            if (!enemy_present(enemies[i])) {
                draw_enemy(index, enemies[i], 0, 1, 0, 0);
            } else {
                if (*time_passed > 700) {
                    draw_fire_enemy(index, 0, speedfire01, enemies[i], *time_passed);
                    draw_enemy(index, enemies[i], 0, 1, 0, 0);
                } else {
                    draw_fire_enemy(index, 0, speedfire02, enemies[i], *time_passed);
                    if (enemies[i]->right) {
                        draw_enemy(index, enemies[i], 0, 0, 1, 0);
                    } else {
                        draw_enemy(index, enemies[i], 0, 0, 0, 1);
                    }
                    change_direction(enemies[i]);
                }
            }
        }
    }

    /* Incrémente le temps passé */
    (*time_passed)++;
}


// Initialiser une boule de feu pour un ennemi
Fire* initialize_fire_enemy(Enemy* new_enemy){
    // Allouer de la mémoire pour la nouvelle boule de feu
    Fire* new_fire = malloc(sizeof(Fire));

    // Initialiser les différentes images de la boule de feu
    MLV_Image* img = initialize_image("./data/fireball.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img2 = initialize_image("./data/fireball2.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img3 = initialize_image("./data/fireball3.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img4 = initialize_image("./data/fireball4.png", WIDTH_FIRE, HEIGHT_FIRE);
    
    // Assigner les images à la boule de feu
    new_fire->positions[0] = img;
    new_fire->positions[1] = img2;
    new_fire->positions[2] = img3;
    new_fire->positions[3] = img4;

    // Définir les coordonnées de la boule de feu
    new_fire->x = new_enemy->x + WIDTH_PLAYER/2 - WIDTH_FIRE/2;
    new_fire->y = new_enemy->y + HEIGHT_PLAYER - 30;

    // Initialiser le pointeur vers la boule de feu suivante
    new_fire->next = NULL;

    return new_fire;
}

// Initialise une boule de feu pour un joueur
Fire* initialize_fire_player(Player* new_player){
    // Allouer de la mémoire pour la nouvelle boule de feu
    Fire* new_fire = malloc(sizeof(Fire));

    // Initialiser les différentes images de la boule de feu
    MLV_Image* img = initialize_image("./data/fireballp.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img2 = initialize_image("./data/fireballp2.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img3 = initialize_image("./data/fireballp3.png", WIDTH_FIRE, HEIGHT_FIRE);
    MLV_Image* img4 = initialize_image("./data/fireballp4.png", WIDTH_FIRE, HEIGHT_FIRE);
    
    // Assigner les images à la boule de feu
    new_fire->positions[0] = img;
    new_fire->positions[1] = img2;
    new_fire->positions[2] = img3;
    new_fire->positions[3] = img4;

    // Définir les coordonnées de la boule de feu
    new_fire->x = new_player->x + WIDTH_PLAYER/2 - WIDTH_FIRE/2;
    new_fire->y = new_player->y + 5;

    // Initialiser le pointeur vers la boule de feu suivante
    new_fire->next = NULL;

    return new_fire;
}

void draw_fire_enemy(int index, int up, int down, Enemy* new_enemy, int time_passed){
    Fire* ptr;
    Fire* tmp;
    Fire* pred = NULL;

    // Parcourir la liste des boules de feu de l'ennemi
    for ( ptr = new_enemy->enemy_fire; ptr != NULL; pred = ptr, ptr = ptr->next) {
        // Dessiner la boule de feu
        MLV_draw_image(ptr->positions[index % 4], ptr->x, ptr->y);
        
        // Mettre à jour la position de la boule de feu
        ptr->y += -up + down;
        
        // Vérifier si la boule de feu est sorti de l'écran
        if (pred && ptr->y >= WINDOW_HEIGHT) {
            // Supprimer la boule de feu de la liste
            tmp = ptr;
            ptr = ptr->next;
            pred->next = ptr;

            // Libérer la mémoire des images de la boule de feu
            for (int i = 0; i < 4; i++) {
                MLV_free_image(tmp->positions[i]);
            }

            // Libérer la mémoire de la boule de feu
            free(tmp);

            if (!ptr) {
                break;
            }
        }
    }

    // Augmenter le compteur de temps de tir
    new_enemy->time_shoot++;

    // Vérifier si l'ennemi doit tirer une nouvelle boule de feu
    if(new_enemy->time_shoot % 100 == 0) {
        ptr = initialize_fire_enemy(new_enemy);
        ptr->next = new_enemy->enemy_fire;
        new_enemy->enemy_fire = ptr;
    }
} 

void draw_fire_player(int index, int up, int down, Player* new_player){
    Fire* ptr;
    Fire* tmp;
    Fire* pred = NULL;

    // Vérifier si le joueur est prêt à tirer une nouvelle boule de feu
    if(new_player->time_shoot == 30) {
        new_player->is_fire_shot = 0;
    }
    
    if (new_player->is_fire_shot == 0 && MLV_get_keyboard_state(MLV_KEYBOARD_SPACE) == MLV_PRESSED) {
        ptr = initialize_fire_player(new_player);
        ptr->next = new_player->shots;
        new_player->shots = ptr;
        new_player->is_fire_shot = 1;
        new_player->time_shoot = 0;
    }

    // Parcourir la liste des boules de feu du joueur
    for ( ptr = new_player->shots; ptr != NULL; pred = ptr, ptr = ptr->next) {
        // Dessiner la boule de feu
        MLV_draw_image(ptr->positions[index % 4], ptr->x, ptr->y);
        
        // Mettre à jour la position de la boule de feu
        ptr->y += -up + down;
        
        // Vérifier si la boule de feu est sorti de l'écran
        if (pred && ptr->y <= -HEIGHT_FIRE) {
            // Supprimer la boule de feu de la liste
            tmp = ptr;
            ptr = ptr->next;
            pred->next = ptr;

            // Libérer la mémoire des images de la boule de feu
            for (int i = 0; i < 4; i++) {
                MLV_free_image(tmp->positions[i]);
            }

            // Libérer la mémoire de la boule de feu
            free(tmp);

            if (!ptr) {
                break;
            }
        } else if (ptr->y <= -HEIGHT_FIRE) {
            // Supprimer la boule de feu de la liste si c'est le premier élément
            for (int i = 0; i < 4; i++) {
                MLV_free_image(ptr->positions[i]);
            }
            free(ptr);
        
            new_player->shots = NULL;
            
            break;
        }
    }

    // Augmenter le compteur de temps de tir
    new_player->time_shoot++;
}



Enemy* initialize_enemy(int x, int width, int height, int lives, int right, int left){
    Enemy* new_enemy= (Enemy*)malloc(sizeof(Enemy));
    if (new_enemy == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour l'ennemi.\n");
        return NULL;
    }
    
    new_enemy->positions[0]=initialize_image("./data/enemy.png", WIDTH_PLAYER, HEIGHT_PLAYER);
    new_enemy->positions[1]=initialize_image("./data/enemy2.png", WIDTH_PLAYER, HEIGHT_PLAYER);
    new_enemy->positions[2]= initialize_image("./data/enemy3.png", WIDTH_PLAYER, HEIGHT_PLAYER); 
    new_enemy->x =x;
    new_enemy->y =-HEIGHT_PLAYER;
    new_enemy->width =width;
    new_enemy->height =height;
    new_enemy->nb_lives = lives;
    new_enemy->fires_shot=0;
    new_enemy->right=right;
    new_enemy->left=left;
    new_enemy->enemy_fire=initialize_fire_enemy(new_enemy);
    if (new_enemy->enemy_fire == NULL) {
        free(new_enemy->positions[0]);
        free(new_enemy->positions[1]);
        free(new_enemy->positions[2]);
        free(new_enemy);
        return NULL;
    }
    new_enemy->enemy_fire->y+=HEIGHT_PLAYER-30;
    new_enemy->time_shoot=0;
    return new_enemy;
}

void draw_UI(Player* player, int start_time) {
    char score_str[50];
    char time_str[50];
    int current_time = MLV_get_time() - start_time;
    
    // Convertir le score en une chaîne de caractères pour pouvoir l'afficher
    sprintf(score_str, "Score: %d", player->score);
    
    // Convertir le temps écoulé en une chaîne de caractères pour pouvoir l'afficher
    sprintf(time_str, "Time: %d:%02d", current_time / 60000, (current_time / 1000) % 60);
    
    // Dessiner la barre de vie
    MLV_draw_filled_rectangle(10, 10, player->nb_lives * 20, 20, MLV_COLOR_RED);
    
    // Dessiner le score
    MLV_draw_text(10, 40, score_str, MLV_COLOR_WHITE);
    
    // Dessiner le temps écoulé
    MLV_draw_text(10, 70, time_str, MLV_COLOR_WHITE);
    
    // Mettre à jour la fenêtre pour afficher les nouvelles informations
    MLV_actualise_window();
}
