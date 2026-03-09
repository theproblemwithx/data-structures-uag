#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define TABLE_SIZE 10

// =====================
// TYPE ENUM
// Used to index into the adjacency matrix
// =====================
typedef enum {
    FIRE,       // 0
    WATER,      // 1
    GRASS,      // 2
    ELECTRIC,   // 3
    GROUND,     // 4
    ROCK,       // 5
    ICE,        // 6
    FLYING,     // 7
    NORMAL,     // 8
    PSYCHIC,    // 9
    POISON,     // 10
    FIGHTING,   // 11
    DRAGON,     // 12
    NUM_TYPES   // 13 - total count
} PokemonType;

// =====================
// STRUCTS
// =====================

typedef struct {
    char move[100];
    int power;
    char type[50];
    int accuracy;
} Moves;

typedef struct {
    char name[100];
    char type[40];
    int level;
    int max_hp;
    int current_hp;
    Moves *moves;
    char weaknesses[3][20];
} pokemon;

// Hash table node
typedef struct Node {
    pokemon *poke;
    struct Node *next;
} Node;

// Queue node
typedef struct QNode {
    char message[200];
    struct QNode *next;
} QNode;

// Queue struct
typedef struct {
    QNode *front;
    QNode *back;
    int size;
} Queue;

// =====================
// MOVES
// =====================

Moves pikachu_moves[4] = {
    {"Thundershock", 40, "electric", 100},
    {"Quick Attack", 40, "normal", 100},
    {"Thunder", 80, "electric", 70},
    {"Body Slam", 85, "normal", 100}
};

Moves charizard_moves[4] = {
    {"Rage", 20, "normal", 100},
    {"Flamethrower", 95, "fire", 100},
    {"Dragon Rage", 90, "dragon", 100},
    {"Earthquake", 100, "ground", 100}
};

Moves blastoise_moves[4] = {
    {"Hydro Pump", 95, "water", 100},
    {"Skull Bash", 95, "normal", 100},
    {"Blizzard", 95, "ice", 90},
    {"Seismic Toss", 100, "fighting", 100}
};

Moves venusaur_moves[4] = {
    {"Razor Leaf", 55, "grass", 95},
    {"Solar Beam", 95, "grass", 100},
    {"Poison Powder", 80, "poison", 85},
    {"Mega Drain", 40, "grass", 100}
};

Moves pidgeotto_moves[4] = {
    {"Wing Attack", 50, "flying", 100},
    {"Quick Attack", 45, "normal", 100},
    {"Sand Attack", 0, "normal", 95},
    {"Sky Attack", 95, "flying", 90}
};

Moves psyduck_moves[4] = {
    {"Confusion", 50, "psychic", 100},
    {"Hydro Pump", 90, "water", 80},
    {"Blizzard", 90, "ice", 90},
    {"Rest", 0, "psychic", 100}
};

// =====================
// POKEMON
// =====================

pokemon pikachu = {
    .name = "Pikachu",
    .type = "electric",
    .level = 50,
    .max_hp = 450,
    .current_hp = 450,
    .moves = pikachu_moves,
    .weaknesses = {"ground", "", ""}
};

pokemon charizard = {
    .name = "Charizard",
    .type = "fire",
    .level = 50,
    .max_hp = 460,
    .current_hp = 460,
    .moves = charizard_moves,
    .weaknesses = {"water", "ground", "rock"}
};

pokemon blastoise = {
    .name = "Blastoise",
    .type = "water",
    .level = 50,
    .current_hp = 430,
    .max_hp = 430,
    .moves = blastoise_moves,
    .weaknesses = {"grass", "electric", ""}
};

pokemon venusaur = {
    .name = "Venusaur",
    .type = "grass",
    .level = 50,
    .max_hp = 465,
    .current_hp = 465,
    .moves = venusaur_moves,
    .weaknesses = {"fire", "flying", "ice"}
};

pokemon pidgeotto = {
    .name = "Pidgeotto",
    .type = "flying",
    .level = 50,
    .max_hp = 468,
    .current_hp = 468,
    .moves = pidgeotto_moves,
    .weaknesses = {"electric", "rock", "ice"}
};

pokemon psyduck = {
    .name = "Psyduck",
    .type = "water",
    .level = 50,
    .max_hp = 480,
    .current_hp = 480,
    .moves = psyduck_moves,
    .weaknesses = {"electric", "grass", ""}
};

// =====================
// GRAPH FUNCTIONS
// Type effectiveness adjacency matrix
// chart[attack][defend] = 1 means attack type is super effective against defend type
// =====================

PokemonType get_type_index(string type)
{
    if (strcmp(type, "fire") == 0)     return FIRE;
    if (strcmp(type, "water") == 0)    return WATER;
    if (strcmp(type, "grass") == 0)    return GRASS;
    if (strcmp(type, "electric") == 0) return ELECTRIC;
    if (strcmp(type, "ground") == 0)   return GROUND;
    if (strcmp(type, "rock") == 0)     return ROCK;
    if (strcmp(type, "ice") == 0)      return ICE;
    if (strcmp(type, "flying") == 0)   return FLYING;
    if (strcmp(type, "normal") == 0)   return NORMAL;
    if (strcmp(type, "psychic") == 0)  return PSYCHIC;
    if (strcmp(type, "poison") == 0)   return POISON;
    if (strcmp(type, "fighting") == 0) return FIGHTING;
    if (strcmp(type, "dragon") == 0)   return DRAGON;
    return -1;  // unknown type
}

void build_type_chart(int chart[NUM_TYPES][NUM_TYPES])
{
    // fire is strong against grass, ice
    chart[FIRE][GRASS] = 1;
    chart[FIRE][ICE]   = 1;

    // water is strong against fire, ground, rock
    chart[WATER][FIRE]   = 1;
    chart[WATER][GROUND] = 1;
    chart[WATER][ROCK]   = 1;

    // grass is strong against water, ground, rock
    chart[GRASS][WATER]  = 1;
    chart[GRASS][GROUND] = 1;
    chart[GRASS][ROCK]   = 1;

    // electric is strong against water, flying
    chart[ELECTRIC][WATER]  = 1;
    chart[ELECTRIC][FLYING] = 1;

    // ground is strong against fire, electric, rock
    chart[GROUND][FIRE]     = 1;
    chart[GROUND][ELECTRIC] = 1;
    chart[GROUND][ROCK]     = 1;

    // rock is strong against fire, ice, flying
    chart[ROCK][FIRE]   = 1;
    chart[ROCK][ICE]    = 1;
    chart[ROCK][FLYING] = 1;

    // ice is strong against grass, flying
    chart[ICE][GRASS]  = 1;
    chart[ICE][FLYING] = 1;
}

// Replaces the old super_effective() - now uses graph lookup instead of
// looping through each pokemon's weaknesses array
int super_effective(string move_type, pokemon *opponent, int chart[NUM_TYPES][NUM_TYPES])
{
    int attack_index = get_type_index(move_type);
    int defend_index = get_type_index(opponent->type);

    if (attack_index == -1 || defend_index == -1)
        return 0;  // unknown type, no bonus

    return chart[attack_index][defend_index];  // 1 if super effective, 0 if not
}

// =====================
// HASH TABLE FUNCTIONS
// =====================

int hash(string name)
{
    int sum = 0;
    for (int i = 0; i < strlen(name); i++)
        sum += tolower(name[i]);
    return sum % TABLE_SIZE;
}

void add_pokemon(Node *pokedex[], pokemon *poke)
{
    int index = hash(poke->name);

    Node *new_node = malloc(sizeof(Node));
    new_node->poke = poke;
    new_node->next = pokedex[index];
    pokedex[index] = new_node;

    printf("Added %s to the Pokedex!\n", poke->name);
}

pokemon* search_pokemon(Node *pokedex[], string name)
{
    int index = hash(name);
    Node *cursor = pokedex[index];

    while (cursor != NULL)
    {
        char cursor_name[100];
        char search_name[100];

        strcpy(cursor_name, cursor->poke->name);
        strcpy(search_name, name);

        for (int i = 0; i < strlen(cursor_name); i++)
            cursor_name[i] = tolower(cursor_name[i]);
        for (int i = 0; i < strlen(search_name); i++)
            search_name[i] = tolower(search_name[i]);

        if (strcmp(cursor_name, search_name) == 0)
        {
            printf("%s found in the Pokedex!\n", cursor->poke->name);
            return cursor->poke;
        }
        cursor = cursor->next;
    }

    printf("%s not found in the Pokedex!\n", name);
    return NULL;
}

void remove_pokemon(Node *pokedex[], string name)
{
    int index = hash(name);
    Node *cursor = pokedex[index];
    Node *prev = NULL;

    while (cursor != NULL)
    {
        char cursor_name[100];
        char search_name[100];

        strcpy(cursor_name, cursor->poke->name);
        strcpy(search_name, name);

        for (int i = 0; i < strlen(cursor_name); i++)
            cursor_name[i] = tolower(cursor_name[i]);
        for (int i = 0; i < strlen(search_name); i++)
            search_name[i] = tolower(search_name[i]);

        if (strcmp(cursor_name, search_name) == 0)
        {
            if (prev == NULL)
                pokedex[index] = cursor->next;
            else
                prev->next = cursor->next;

            free(cursor);
            printf("%s removed from the Pokedex!\n", name);
            return;
        }

        prev = cursor;
        cursor = cursor->next;
    }

    printf("%s not found in the Pokedex!\n", name);
}

// =====================
// QUEUE FUNCTIONS
// =====================

void enqueue(Queue *q, string message)
{
    QNode *new_node = malloc(sizeof(QNode));
    strcpy(new_node->message, message);
    new_node->next = NULL;

    if (q->back == NULL)
    {
        q->front = new_node;
        q->back = new_node;
    }
    else
    {
        q->back->next = new_node;
        q->back = new_node;
    }
    q->size++;
}

void dequeue(Queue *q)
{
    if (q->front == NULL)
        return;

    QNode *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->back = NULL;

    free(temp);
    q->size--;
}

void print_battle_log(Queue *q)
{
    printf("\n=== Battle History ===\n");

    int move_number = 1;
    while (q->front != NULL)
    {
        printf("%i. %s\n", move_number, q->front->message);
        dequeue(q);
        move_number++;
    }
    printf("=== End of Battle ===\n\n");
}

// =====================
// QUICKSORT FUNCTIONS
// Sorting an array of pointers so the original structs never move,
// keeping moves arrays intact and correctly linked to each pokemon
// =====================

void swap_ptrs(pokemon **a, pokemon **b)
{
    pokemon *temp = *a;
    *a = *b;
    *b = temp;
}

int partition_ptrs(pokemon *arr[], int low, int high)
{
    int pivot = arr[high]->max_hp;
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (arr[j]->max_hp <= pivot)
        {
            i++;
            swap_ptrs(&arr[i], &arr[j]);
        }
    }

    swap_ptrs(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort_ptrs(pokemon *arr[], int low, int high)
{
    if (low < high)
    {
        int pivot_index = partition_ptrs(arr, low, high);
        quicksort_ptrs(arr, low, pivot_index - 1);
        quicksort_ptrs(arr, pivot_index + 1, high);
    }
}

// =====================
// GAME FUNCTIONS
// =====================

void get_user_input(string* cpu_name);
int display_options();
int pokemon_selection(pokemon *sorted[], int count);
void print_stats(pokemon* selected_pokemon, string prompt);
int show_moves(pokemon* current_pokemon);
int validate_selection(int min, int max, string prompt);
int is_name_valid(string name);

int main()
{
    srand(time(NULL));

    // =====================
    // GRAPH SETUP
    // Build type effectiveness adjacency matrix
    // =====================
    int type_chart[NUM_TYPES][NUM_TYPES] = {0};
    build_type_chart(type_chart);

    // =====================
    // POKEMON SETUP
    // =====================
    pokemon available_pokemon[6] = {pikachu, charizard, blastoise, venusaur, pidgeotto, psyduck};

    // Build pointer array for sorting - original structs stay untouched
    pokemon *sorted[6];
    for (int i = 0; i < 6; i++)
        sorted[i] = &available_pokemon[i];

    // Sort pointers by HP
    quicksort_ptrs(sorted, 0, 5);

    // =====================
    // HASH TABLE SETUP
    // =====================
    Node *pokedex[TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++)
        pokedex[i] = NULL;

    printf("Initializing Pokedex...\n");
    for (int i = 0; i < 6; i++)
        add_pokemon(pokedex, &available_pokemon[i]);

    printf("\n");
    sleep(1);

    // =====================
    // QUEUE SETUP
    // =====================
    Queue battle_log;
    battle_log.front = NULL;
    battle_log.back = NULL;
    battle_log.size = 0;

    // =====================
    // GAME SETUP
    // =====================
    char names[9][10] = {"Ash", "Misty", "Brock", "Erika", "Koga", "Sabrina", "Blaine", "Giovanni", "Gary"};
    string global_cpu_name = names[(rand() % 9)];

    pokemon* player_pokemon;
    pokemon* cpu_pokemon;

    system("clear");

    get_user_input(&global_cpu_name);

    // Pokemon selection using sorted pointer array
    int choice = pokemon_selection(sorted, 6);
    player_pokemon = sorted[choice - 1];

    // Confirm pokemon exists in pokedex
    search_pokemon(pokedex, player_pokemon->name);

    system("clear");
    print_stats(player_pokemon, "You picked the following Pokemon: ");

    // CPU picks from original array
    int cpu_choice = (rand() % 4);
    cpu_pokemon = &available_pokemon[cpu_choice];

    printf("\n%s picked %s!\n", global_cpu_name, cpu_pokemon->name);
    printf("HP: %d\nType: %s\nLevel: %i\n\n", cpu_pokemon->max_hp, cpu_pokemon->type, cpu_pokemon->level);
    printf("Get ready to battle!\n\n");

    // =====================
    // BATTLE LOOP
    // =====================
    int player_battle_choice, cpu_move_choice, fight_choice, damage, cpu_damage;
    char log_message[200];

    while (player_pokemon->current_hp > 0 && cpu_pokemon->current_hp > 0)
    {
        player_battle_choice = display_options();

        if (player_battle_choice == 1)
        {
            fight_choice = show_moves(player_pokemon);
            fight_choice -= 1;

            printf("%s used %s!\n", player_pokemon->name, player_pokemon->moves[fight_choice].move);

            // Log the player's move to the queue
            sprintf(log_message, "%s used %s!", player_pokemon->name, player_pokemon->moves[fight_choice].move);
            enqueue(&battle_log, log_message);

            damage = player_pokemon->moves[fight_choice].power;

            // Use graph to check type effectiveness
            if (super_effective(player_pokemon->moves[fight_choice].type, cpu_pokemon, type_chart))
            {
                damage = (int)(damage * 1.3);
                printf("It's super effective!\n");
            }

            cpu_pokemon->current_hp -= damage;
            if (cpu_pokemon->current_hp < 0)
            {
                cpu_pokemon->current_hp = 0;
                printf("%s has fainted!\n", cpu_pokemon->name);
                remove_pokemon(pokedex, cpu_pokemon->name);
            }
            else
            {
                printf("%s's %s HP is now %i.\n", global_cpu_name, cpu_pokemon->name, cpu_pokemon->current_hp);
            }
        }
        else if (player_battle_choice == 2)
        {
            printf("You only have one Pokemon!\n");
            sleep(2);
            system("clear");
            continue;
        }
        else if (player_battle_choice == 3)
        {
            printf("No items available! Remember to stop by the Pokemon Mart next time!\n");
            sleep(2);
            system("clear");
            continue;
        }
        else if (player_battle_choice == 4)
        {
            printf("Can't run from this battle!\n");
            sleep(2);
            system("clear");
            continue;
        }

        if (cpu_pokemon->current_hp <= 0)
        {
            printf("You won this battle!\n");
            break;
        }

        // CPU turn
        cpu_move_choice = (rand() % 4);
        printf("\n%s's %s used %s!\n", global_cpu_name, cpu_pokemon->name, cpu_pokemon->moves[cpu_move_choice].move);

        // Log the CPU's move to the queue
        sprintf(log_message, "%s's %s used %s!", global_cpu_name, cpu_pokemon->name, cpu_pokemon->moves[cpu_move_choice].move);
        enqueue(&battle_log, log_message);

        cpu_damage = cpu_pokemon->moves[cpu_move_choice].power;

        // Use graph to check type effectiveness
        if (super_effective(cpu_pokemon->moves[cpu_move_choice].type, player_pokemon, type_chart))
        {
            cpu_damage = (int)(1.3 * cpu_damage);
            printf("It's super effective!\n");
        }

        player_pokemon->current_hp -= cpu_damage;

        if (player_pokemon->current_hp < 0)
        {
            player_pokemon->current_hp = 0;
            printf("%s has fainted!\n", player_pokemon->name);
            remove_pokemon(pokedex, player_pokemon->name);
            printf("%s won this battle!\n", global_cpu_name);
            break;
        }
        else
        {
            printf("%s's HP is now %i.\n\n", player_pokemon->name, player_pokemon->current_hp);
        }
    }

    // Print the full battle log from the queue
    print_battle_log(&battle_log);

    printf("Thanks for playing! Have a great day!\n");
    return 0;
}

// =====================
// GAME FUNCTION IMPLEMENTATIONS
// =====================

int is_name_valid(string name)
{
    if (strlen(name) == 0)
        return 0;
    for (int i = 0; i < strlen(name); i++)
    {
        if (name[i] >= 0 && name[i] <= 9)
            return 0;
    }
    return 1;
}

void get_user_input(string* cpu_name)
{
    printf("Welcome to the battle!\n");
    string name;
    do {
        name = get_string("Please enter the player's name: ");
    } while (strlen(name) == 0);

    printf("Welcome %s\n", name);
    printf("Your opponent today is: %s\n\n", *cpu_name);
}

int pokemon_selection(pokemon *sorted[], int count)
{
    printf("The available Pokemon today are (sorted by HP):\n");
    for (int i = 0; i < count; i++)
        printf("%i. %s (HP: %d)\n", i + 1, sorted[i]->name, sorted[i]->max_hp);

    int choice = validate_selection(1, count, "Please enter your choice (1 - 6): ");

    printf("I pick %s!\n", sorted[choice - 1]->name);
    sleep(1);

    return choice;
}

void print_stats(pokemon* selected_pokemon, string prompt)
{
    printf("%s\n", prompt);
    printf("Name: %s\nHP: %d\nType: %s\nLevel: %i\n", selected_pokemon->name, selected_pokemon->max_hp, selected_pokemon->type, selected_pokemon->level);
}

int display_options()
{
    int choice;
    printf("Below are your options: \n");
    printf("1. FIGHT   2. PKMN\n");
    printf("3. ITEMS    4. RUN \n");
    choice = validate_selection(1, 4, "What would you like to do? (1 - 4): ");
    return choice;
}

int validate_selection(int min, int max, string prompt)
{
    int choice;
    do {
        printf("%s", prompt);
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            choice = 0;
        }
        if (choice < min || choice > max)
            printf("Invalid choice. Please try again.\n");
    } while (choice < min || choice > max);
    return choice;
}

int show_moves(pokemon* current_pokemon)
{
    int choice;
    system("clear");
    printf("Below are the available moves!\n");
    for (int i = 0; i < 4; i++)
        printf("%i. %s\n", i+1, current_pokemon->moves[i].move);
    choice = validate_selection(1, 4, "Enter your choice (1 - 4): ");
    return choice;
}