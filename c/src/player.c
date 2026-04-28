#include "player.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

// Function To Create The Player
Status player_create(int initial_room_id, int initial_x, int initial_y, Player **player_out){

    // Satisfy Precondition
    if(player_out == NULL){
        return INVALID_ARGUMENT;
    }

    // Allocate Memory For The Player Struct
    Player *p = (Player *)malloc(sizeof(Player));

    // Ensure Memory Allocation Does Not Fail
    if(p == NULL){
        return NO_MEMORY;
    }

    // Initialize Player At The Given Room And Position
    p->room_id = initial_room_id;
    p->x = initial_x;
    p->y = initial_y;

    // Initialize Player Treasure Counts
    p->collected_treasures = NULL;
    p->collected_count = 0;


    // Output The Created Player
    *player_out = p;

    // Return Success
    return OK;
}

// Function To Free The Player Struct
void player_destroy(Player *p){
    // Safe To Call On NULL
    if(p != NULL){
        free(p->collected_treasures);
        free(p);
    }
}

// Function To Get The Current Room ID Of The Player
int player_get_room(const Player *p){

    if(p == NULL){
        return -1;
    }

    return p->room_id;
}

// Function To Get The Current Position Of The Player
Status player_get_position(const Player *p, int *x_out, int *y_out){

    if(p == NULL || x_out == NULL || y_out == NULL){
        return INVALID_ARGUMENT;
    }

    *x_out = p->x;
    *y_out = p->y;
    return OK;
}

// Function To Set The Position Of The Player
Status player_set_position(Player *p, int x, int y){

    if(p == NULL){
        return INVALID_ARGUMENT;
    }

    p->x = x;
    p->y = y;
    return OK;
}

// Function To Move The Player To 
Status player_move_to_room(Player *p, int new_room_id){

    if(p == NULL){
        return INVALID_ARGUMENT;
    }

    p->room_id = new_room_id;
    return OK;
}

// Function To Reset The Player To An Initial State
Status player_reset_to_start(Player *p, int starting_room_id, int start_x, int start_y){

    if(p == NULL){
        return INVALID_ARGUMENT;
    }

    p->room_id = starting_room_id;
    p->x = start_x;
    p->y = start_y;

    // Clear Tresures
    free(p->collected_treasures);
    p->collected_treasures = NULL;
    p->collected_count = 0; 

    return OK;
}

// Function To Collect Treasure 
Status player_try_collect(Player *p, Treasure *treasure){

    // Satisfy Preconditions 
    if(p == NULL || treasure == NULL){
        return NULL_POINTER;
    }

    // Verify The Treasure Is Not Already In The Array
    for(int i = 0; i < p->collected_count; i++){
        if(p->collected_treasures[i]->id == treasure->id){
            return INVALID_ARGUMENT;
        }
    }

    // Add Space For Another Element In The Collected Treasure Array
    Treasure **new_collected_treasures = realloc(p->collected_treasures, sizeof(Treasure *) * (p->collected_count + 1));
    if(new_collected_treasures == NULL){
        return NO_MEMORY;
    }
    p->collected_treasures = new_collected_treasures;

    // Insert The Treasure Into The Player Collected Treasures Array
    p->collected_treasures[p->collected_count] = treasure;
    p->collected_count++;

    treasure->collected = true;

    return OK;
}

// Function To Check Weither The Player Has Collected A Given Treasure
bool player_has_collected_treasure(const Player *p, int treasure_id){

    // Satisfy Preconditions 
    if(p == NULL || treasure_id < 0){
        return false;
    }

    // Iterate Through All Treasure In The Player Collected Treasure Array
    for(int i = 0; i < p->collected_count; i++){

        // If The Given Treasure ID Is In The List Then It Has Been Collected
        if(p->collected_treasures[i]->id == treasure_id){
            return true;
        }
    }

    return false;
}

// Function To Get The Number Of Treasures Collected By The Player
int player_get_collected_count(const Player *p){
    
    // Satisfy Preconditions
    if(p == NULL){
        return 0;
    }

    return p->collected_count; 
} 

// Function To Retrive The Array Of The Collected Treasure 
const Treasure * const * 
player_get_collected_treasures(const Player *p, int *count_out){

    // Satisfy Preconditions 
    if(p == NULL || count_out == NULL){
        return NULL;
    }

    *count_out = p->collected_count;

    return (const Treasure * const *)p->collected_treasures;
}
