#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // used for sleep
#include <ctype.h>

#define TABLE_SIZE 10 // 10 slots for the hash table

// Struct for the moves
typedef struct {
    char move[100];
    int power;
    char type[50]; // used for check to exert more damage
    int accuracy;
} Moves;

// Pokemon structure that has most of the information
typedef struct {
    char name[100];
    char type[40];
    int level;
    int max_hp;
    int current_hp;
    Moves *moves;
    char weaknesses[3][20]; // This will hold the types against which the pokemon is weak, attacks should do x1.3 damage
} pokemon;

typedef struct Node {
    pokemon *poke;
    struct Node *next;
} Node;

// Move, Power, Type, Accuracy. These go first since pokemon struct requires them
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
    {"Sand Attack", 0, "norml", 95},
    {"Sky Attack", 95, "flying", 90}
};

Moves psyduck_moves[4] = {
    {"Confusion", 50, "psychic", 100},
    {"Hydro Pump", 90, "water", 80},
    {"Blizzard", 90, "ice", 90},
    {"Rest", 0, "psychic", 100}
};

// Empty weaknesses to make it easier for checking loop, all max 3
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

// Hash table functions

int hash(string name)
{
    int sum = 0;
    for (int i = 0; i < strlen(name); i++)
    {
        sum += tolower(name[i]);
    }
    return sum % TABLE_SIZE;
}

// add pokemon to hash table
void add_pokemon(Node *pokedex[], pokemon *poke)
{
    int index = hash(poke->name);

    Node *new_node = malloc(sizeof(Node));
    new_node->poke = poke;

    //insertar al frente de la lista
    new_node->next = pokedex[index];
    pokedex[index] = new_node;

    printf("Added %s to the Pokedex!\n", poke->name);
}

// search for pokemon by name, returns NULL pointer if not found
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
        }
    } 
    printf("%s not found in the Pokedex!\n", name);
}

// Game functions
void get_user_input(string* cpu_name);
int display_options();
int pokemon_selection();
void print_stats(pokemon* selected_pokemon, string prompt);
pokemon* set_pokemon(int pokemon_choice, pokemon* available_pokemon);
int show_moves(pokemon* current_pokemon);
int super_effective(string move_type, pokemon* opponent);
int is_name_valid(string name);
int validate_selection(int min, int max, string prompt);

int main()
{
    srand(time(NULL));

    // Hash Table setup
    // Initialize all slots in table to NULL, then we add the pokemon
    Node *pokedex[TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        pokedex[i] = NULL;
    }

    // declare available pokemon, then add pointers to table
    pokemon available_pokemon[6] = {pikachu, charizard, blastoise, venusaur, pidgeotto, psyduck};

    printf("Initializing Pokedex...\n");
    for (int i = 0; i < 6; i++) 
    {
        add_pokemon(pokedex, &available_pokemon[i]);
    }

    // List of possible opponents
    char names[9][10] = {"Ash", "Misty", "Brock", "Erika", "Koga", "Sabrina", "Blaine", "Giovanni","Gary"};

    string global_cpu_name = names[(rand() % 9)];

    int choice, cpu_choice, item_choice;


    // Create Pokemon for user and cpu
    pokemon* player_pokemon;
    pokemon* cpu_pokemon;

    system("clear");

    // Greeting and get user's name
    get_user_input(&global_cpu_name);

    // Pokemon selection
    choice = pokemon_selection();
    player_pokemon = set_pokemon(choice, available_pokemon);
    system("clear");

    // Print user's Pokemon information
    print_stats(player_pokemon, "You picked the following Pokemon: ");

    // CPU Pick: Generate random value, input to pick. At this point they can pick the same pokemon, ideally the CPU would need to pick a pokemon different from the player's.
    cpu_choice = (rand() % 4) + 1;
    cpu_pokemon = set_pokemon(cpu_choice, available_pokemon);

    // Print CPU stats. TODO create print_cpu_stats()
    printf("\n%s picked %s!\n", global_cpu_name, cpu_pokemon->name);
    printf("HP: %d\nType: %s\nLevel: %i\n\n", cpu_pokemon->max_hp, cpu_pokemon->type, cpu_pokemon->level);

    printf("Get ready to battle!\n\n");

    // Battle Loop

    // Declare variables needed for battle loop
    int player_battle_choice, cpu_move_choice, fight_choice, damage, cpu_damage;

    // The condition that keeps the loop going is that both player's HP must be > 0. If it is <= 0 it means one of them has lost the battle and loop should end.
    // Chose to leave the general structure here instead of inside of a function since this is the main objective of the game. Helper functions used throughout.
    while (player_pokemon->current_hp > 0 && cpu_pokemon->current_hp > 0)
    {
        // Display battle options to the user
        player_battle_choice = display_options();

        // User picks 1, 2, 3, or 4
        if (player_battle_choice == 1)
        {
            // Fight - clear screen, show pokemon fight moves per gameboy format, return choice
            fight_choice = show_moves(player_pokemon);
            fight_choice -= 1; // to account for 0 index

            // Attack enemy and reduce hp
            printf("%s used %s!\n", player_pokemon->name, player_pokemon->moves[fight_choice].move);

            // Check if move is effective against opponent and increase damage
            damage = player_pokemon->moves[fight_choice].power;
            if (super_effective(player_pokemon->moves[fight_choice].type, cpu_pokemon))
            {
                damage = (int)(damage * 1.3);
                printf("It's supper effective!\n");
            }

            // Decrease enemy's HP and check if they HP has depleted. If not, print current HP
            cpu_pokemon->current_hp -= damage;
            if (cpu_pokemon->current_hp < 0)
            {
                cpu_pokemon->current_hp = 0;
                printf("%s has fainted!\n", cpu_pokemon->name);
            }
            else
            {
            printf("%s's %s HP is now %i.\n", global_cpu_name, cpu_pokemon->name, cpu_pokemon->current_hp);
            }
        }
        // All of these return the player to the main options until they choose to fight (1)
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

        // Check if CPU's HP below 0
        if (cpu_pokemon->current_hp <= 0)
        {
            printf("You won this battle!\n");
            break;
        }

        // CPU turn. They will only battle. Pick a random move, reduce energy.
        cpu_move_choice = (rand() % 4);
        printf("\n%s's %s used %s!\n", global_cpu_name, cpu_pokemon->name, cpu_pokemon->moves[cpu_move_choice].move);

        cpu_damage = cpu_pokemon->moves[cpu_move_choice].power;

        // Check if attack is super effective
        if (super_effective(cpu_pokemon->moves[cpu_move_choice].type, player_pokemon))
        {
            cpu_damage = (int)(1.3 * cpu_damage);
            printf("It's super effective!\n");
        }
        // Reduce player's HP
        player_pokemon->current_hp -= cpu_damage;

        // Check HP to decide if battle keeps going
        if (player_pokemon->current_hp < 0)
        {
            player_pokemon->current_hp = 0;
            printf("%s has fainted!\n", player_pokemon->name);
            printf("%s won this battle!\n", global_cpu_name);
            break;
        }
        else
        {
            printf("%s's HP is now %i.\n\n", player_pokemon->name, player_pokemon->current_hp);
        }

    } // while loop ends

    printf("Thanks for playing! Have a great day!\n");
    return 0;
}

int is_name_valid(string name)
{
    // Validate name input (empty space, numbers) Decided not to use this
    // Check for empty string
    int length = strlen(name);

    if (length == 0)
    {
        return 0;
    }
    // Check for numbers in the name
    for (int i = 0; i < length; i ++)
    {
        if (isdigit(name[i]) || isspace(name[i]))
        {
            return 0;
        }
    }
    return 1;
}

void get_user_input(string* cpu_name)
{
    // Greeting and get user's name

    printf("Welcome to the battle!\n");
    string name;
    do {
        name = get_string("Please enter the player's name: ");
    } while (strlen(name) == 0);

    printf("Welcome %s\n", name);
    printf("Your opponent today is: %s\n\n", *cpu_name);
}

int pokemon_selection()
{
    int choice;
    printf("The available Pokemon today are: \n");
    printf("1. Pikachu\n2. Charizard\n3. Blastoise\n4. Venusaur\n5. Pidgeotto\n6. Psyduck\n");

    choice = validate_selection(1, 6, "Please enter your choice (1 - 6): ");

    // Since I only have 4 pokemon doing this manually, but if more pokemon are added we would need a loop or another function that simplifies this.
    switch (choice) {
        case 1:
            printf("I pick Pikachu!\n");
            sleep(1);
            break;
        case 2:
            printf("I pick Charizard!\n");
            sleep(1);
            break;
        case 3:
            printf("I pick Blastoise!\n");
            sleep(1);
            break;
        case 4:
            printf("I pick Venusaur!\n");
            sleep(1);
            break;
        case 5:
            printf("I pick Pidgeotto!\n");
            sleep(1);
            break;
        case 6:
            printf("I pick Psyduck!\n");
            sleep(1);
            break;
    }
    return choice;
}

pokemon* set_pokemon(int pokemon_choice, pokemon* available_pokemon)
{
    // Take input from pokemon selection and return the poke struct
    // 1 Pikachu, 2 Charizard, 3 Blastoise, 4 Venusaur, 5 Pidgeotto, 6 Psyduck

    if (pokemon_choice >= 1 && pokemon_choice <= 6)
    {
        return &available_pokemon[pokemon_choice - 1];
    }
    // Had to include a default.
    return &available_pokemon[0];
}

void print_stats(pokemon* selected_pokemon, string prompt)
{
    // Print Pokemon's struct information

    printf("%s\n", prompt);

    printf("Name: %s\nHP: %d\nType: %s\nLevel: %i\n", selected_pokemon->name, selected_pokemon->max_hp, selected_pokemon->type, selected_pokemon->level);
}

int display_options()
{
    // Display fight options to the user and return choice

    int choice;

    printf("Below are your options: \n");
    printf("1. FIGHT   2. PKMN\n");
    printf("3. ITEMS    4. RUN \n");

    // Print message and validate user input
    choice = validate_selection(1, 4, "What would you like to do? (1 - 4): ");

    return choice;
}

int validate_selection(int min, int max, string prompt)
{
    // Validate selection for Pokemon, Fight, and Moves
    int choice;

    do {
        printf("%s", prompt);

        // Validate input
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n'); // using this to clear the buffer
            choice = 0;
        }
        if (choice < min || choice > max)
        {
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice < min || choice > max);
    return choice;
}

int show_moves(pokemon* current_pokemon)
{
    // Takes in a pokemon struct, shows available moves and returns fight choice

    int choice;

    system("clear");
    printf("Below are the available moves!\n");

    for (int i = 0; i < 4; i++)
    {
        printf("%i. %s\n", i+1, current_pokemon->moves[i].move);
    }

    choice = validate_selection(1, 4, "Enter your choice (1 - 4): ");

    return choice;
}

int super_effective(string move_type, pokemon* opponent)
{
    // Input: move type, and if it is inside the weaknesses array of the other pokemon, return 1, else return 0.
    for (int i = 0; i < 3; i++)
    {
        if(strcmp(move_type, opponent->weaknesses[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}