#include "game_engine.h"
#include "game_engine_helper.h"
#include "world_loader.h"
#include "types.h"
#include "graph.h"
#include "player.h"
#include "room.h"

#include <stdlib.h>
#include <string.h>

/* ==================================================================
 * Helper Function To Get A Room By ID
 * ================================================================== */
static Room *game_engine_helper_find_room_by_id(Graph *graph, int room_id){

    const void * const *payloads = NULL;
    int count = 0;

    // Get All Rooms In The Graph
    graph_get_all_payloads(graph, &payloads, &count);

    // Iterate Through All Rooms
    for(int i = 0; i < count; i++){
        
        Room *r = (Room *)payloads[i];

        // Check If The Room Matches The Input ID And Return It If So
        if(r->id == room_id){
            return r;
        }
    }
    return NULL;
}

/* ==================================================================
 * Helper Function To Print Out The Buffer
 * ================================================================== */
static Status game_engine_helper_buffer_to_string(const char *buffer, 
                                                  int width, 
                                                  int height, 
                                                  char **rendered){

    // Handle NULL Inputs
    if(buffer == NULL || rendered == NULL){
        return INVALID_ARGUMENT;
    }

    // Get The Total And Line Length And Accomidate For The \n At The End
    int line_length = width + 1;
    int total_length = height * line_length + 1;

    // Allcoate Space To Render The Room
    char *render = malloc(total_length);
    if(render == NULL){
        return NO_MEMORY;
    }

    int pos = 0;

    // Iterate Through Each Charcter In The Buffer
    for(int row = 0; row < height; row++){
        for(int col = 0; col < width; col++){

            // Add The Given Charcter To The Rendered String
            render[pos++] = buffer[row * width + col];
        }

        // Move To The Next Line
        render[pos++] = '\n'; 
    }

    // End The Render String
    render[pos] = '\0';

    *rendered = render;
    return OK;
}

// Function To Build The Game State
Status game_engine_create(const char *config_file_path, GameEngine **engine_out){

    // Verify Neither Parameter Is NULL
    if(config_file_path == NULL || engine_out == NULL){
        return INVALID_ARGUMENT;
    }

    // Allocate Space For The Game Engine
    GameEngine *eng = calloc(1, sizeof(GameEngine)); 
    if(eng == NULL){
        return NO_MEMORY;
    }

    // Declare First Room That Will Be Initialized By The World Loader
    Room *first_room = NULL;

    // Call The World Load Function
    Status s = loader_load_world(config_file_path, &eng->graph, &first_room, &eng->room_count, &eng->charset);
    if(s != OK){
        free(eng);
        return s;
    }
    if(first_room == NULL){
        graph_destroy(eng->graph);
        free(eng);
        return INTERNAL_ERROR;
    }

    // Initialize Initial Room IDs And X And Y Coordinates
    eng->initial_room_id = first_room->id;
    if(room_get_start_position(first_room, &eng->initial_player_x, &eng->initial_player_y) != OK){
        graph_destroy(eng->graph);
        free(eng);
        return INTERNAL_ERROR;
    }


    // Call The Player Create Function
    s = player_create(eng->initial_room_id, eng->initial_player_x, eng->initial_player_y, &eng->player);
    if(s != OK){
        graph_destroy(eng->graph);
        free(eng);
        return s;
    }

    // engine_out Receives An Owning Pointer To The Newly Created Game Engine
    *engine_out = eng;
    
    return OK;
}

// Function To Destroy The Allocated Game Engine
void game_engine_destroy(GameEngine *eng){

    if(eng == NULL){
        return;
    }

    // Destroy The Player
    player_destroy(eng->player);

    const void * const *payloads = NULL;
    int count = 0;
    graph_get_all_payloads(eng->graph, &payloads, &count);

    for(int i = 0; i < count; i++){

        // Destroy Each Room
        Room *r = (Room *)payloads[i];
        free(r->name);
        free(r->floor_grid);
        free(r->portals);
        free(r->treasures);
        free(r->pushables);
        r->name = NULL;
        r->floor_grid = NULL;
        r->portals = NULL;
        r->treasures = NULL;
        r->pushables = NULL;
    }

    // Destory The Graph
    graph_destroy(eng->graph);

    // Free The Game Engine
    free(eng);
}

// Getter For The Player
const Player *game_engine_get_player(const GameEngine *eng){

    // Return NULL If The Game Engine Does Not Exist
    if(eng == NULL){
        return NULL;
    }

    return eng->player;
}

// Function To Move The Player
Status game_engine_move_player(GameEngine *eng, Direction dir){

    // Return Invalid Argument If Inputs Are Invalid 
    if(eng == NULL || eng->player == NULL){
        return INVALID_ARGUMENT;
    }

    int col = 0; 
    int row = 0;
    // Get The Players Position If It Is Not Found There Is Some Internal Error
    if(player_get_position(eng->player, &col, &row) != OK){
        return INTERNAL_ERROR;
    }

    // Get The Room ID The Player Is Currently In
    int room_id = player_get_room(eng->player);
    
    // Get The Room Based On The ID Of The Players Room
    Room *room = game_engine_helper_find_room_by_id(eng->graph, room_id);
    if(room == NULL){
        return GE_NO_SUCH_ROOM;
    }
    
    int new_x = col;
    int new_y = row;

    // Update Player Position Based On Input Direction
    if(dir==DIR_NORTH){
        new_y--; 
    }
    else if(dir==DIR_SOUTH){
        new_y++;
    }
    else if(dir==DIR_EAST){
        new_x++;
    }
    else if(dir==DIR_WEST){
        new_x--;
    }
    else{
        return INVALID_ARGUMENT;
    }

    // Reject Out Of Bounds
    if(new_x < 0 || new_x >= room->width || new_y < 0 || new_y >= room->height){
        return ROOM_IMPASSABLE;
    }

    int target_id = -1;
    
    // Get The Type Of Tile The Player Is Standing On
    RoomTileType tile = room_classify_tile(room, new_x, new_y, &target_id);

    switch(tile){

        // Return Error If Attempted Move Is A Wall
        case ROOM_TILE_WALL:
            return ROOM_IMPASSABLE;

        // Update Position And Return OK If Player Moves Onto A Floor Tile
        case ROOM_TILE_FLOOR:
            player_set_position(eng->player, new_x, new_y);
            return OK;

        // Update Position And Return OK If Player Moves Onto A Treasure Tile
        case ROOM_TILE_TREASURE: {

            Treasure *picked_up = NULL;

            // Pickup The Treasure
            Status s = room_pick_up_treasure(room, target_id, &picked_up);
            if(s != OK){
                return s;
            }

            s = player_try_collect(eng->player, picked_up);
            if(s != OK){
                return s;
            }

            player_set_position(eng->player, new_x, new_y);
            return OK;
        }

        // Handle If The Player Tries To Push A Pushable Tile
        case ROOM_TILE_PUSHABLE: {

            // Try To Push The Tile
            Status push_result = room_try_push(room, target_id, dir);
            if(push_result != OK){
                return ROOM_IMPASSABLE;
            }
    
            // Update The Player Position
            player_set_position(eng->player, new_x, new_y);
            return OK;
        }
        
        // Handle If The Player Steps Into A Portal
        case ROOM_TILE_PORTAL: {

            // Get The Room That The Portal Goes To From The ID 
            Room *to_room = game_engine_helper_find_room_by_id(eng->graph, target_id);
            if(to_room == NULL){
                return GE_NO_SUCH_ROOM;
            }

            int spawn_x = 0; 
            int spawn_y = 0;

            // Get The Starting Postion For The Player In The New Room
            if(room_get_start_position(to_room, &spawn_x, &spawn_y) != OK){
                return INTERNAL_ERROR;
            }

            // Update The Players Room And Position
            player_move_to_room(eng->player, target_id);
            player_set_position(eng->player, spawn_x, spawn_y);

            return OK;
        }

        default:
            // Unknown Tile Is Impassable
            return ROOM_IMPASSABLE;
    }
}

// Function To Get The Number Of Rooms
Status game_engine_get_room_count(const GameEngine *eng, int *count_out){

    // Handle NULL Values
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }
    if(count_out == NULL){
        return NULL_POINTER;
    }

    // Get The Room Count From The Game Engine
    *count_out = eng->room_count;

    return OK;
}

// Function To Get The Room Dimesnsions
Status game_engine_get_room_dimensions(const GameEngine *eng, int *width_out, int *height_out){

    // Handle NULL Values
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }
    if(width_out == NULL || height_out == NULL){
        return NULL_POINTER;
    }

    // Get The ID Of The Players Room
    Room *room = game_engine_helper_find_room_by_id(eng->graph, player_get_room(eng->player));
    if(room == NULL){
        return GE_NO_SUCH_ROOM;
    }

    // Get The Width And Height From The Room
    *width_out = room_get_width(room);
    *height_out = room_get_height(room);

    return OK;
}


// Function To Reset The Game Engine
Status game_engine_reset(GameEngine *eng){

    // Handle NULL Inputs
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }

    const void * const *payloads = NULL;
    int count = 0;

    // Get The Number Of Rooms
    graph_get_all_payloads(eng->graph, &payloads, &count);
    
    // Iterate Through All Rooms
    for(int i = 0; i < count; i++){

        Room *r = (Room *)payloads[i];
        
        // Iterate Through All Treasure In The Given Room And Set The To Uncollected
        for(int j = 0; j < r->treasure_count; j++){
            r->treasures[j].collected = false;
        }
    }

    // Reset The Player To The Start
    player_reset_to_start(eng->player, eng->initial_room_id, eng->initial_player_x, eng->initial_player_y);

    return OK;
}

// Function To Render The Players Current Room
Status game_engine_render_current_room(const GameEngine *eng, char **str_out){

    // Handel NULL Inputs 
    if(eng == NULL || str_out == NULL){
        return INVALID_ARGUMENT;
    }

    // Get The Room ID Of The Players Current Room
    int room_id = player_get_room(eng->player);

    // Gets The Players Current Room From The ID
    Room *room = game_engine_helper_find_room_by_id(eng->graph, room_id);
    if(room == NULL){
        return INTERNAL_ERROR;
    }

    int width = room->width;
    int height = room->height;

    char *buffer = malloc(((size_t)width * (size_t)height));
    if(buffer == NULL){
        return INTERNAL_ERROR;
    }

    // Render The Current Room
    Status s = room_render(room, &eng->charset, buffer, width, height);
    if(s != OK){
        free(buffer);
        return s;
    }

    int col = 0; 
    int row = 0;
    // Get The Players Position
    if(player_get_position(eng->player, &col, &row) != OK){
        free(buffer);
        return INTERNAL_ERROR;
    }

    // Add The Player If It Is Within The Bounds
    if(col >= 0 && col < width && row >= 0 && row < height){
        buffer[row * width + col] = eng->charset.player;
    }

    // Get The Fully Rendered String Of The Room
    s = game_engine_helper_buffer_to_string(buffer, width, height, str_out);
    
    free(buffer);
    return s;
}

// Function To Render The Room
Status game_engine_render_room(const GameEngine *eng, int room_id, char **str_out){

    // Handle NULL Values
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }
    if(str_out == NULL){
        return NULL_POINTER;
    } 

    // Gets The Room From The Input ID
    Room *room = game_engine_helper_find_room_by_id(eng->graph, room_id);
    if(room == NULL){
        return GE_NO_SUCH_ROOM;
    }

    int width = room->width;
    int height = room->height;

    // Allocate Space For The Buffer
    char *buffer = malloc((size_t)width * (size_t)height);
    if(buffer == NULL){
        return NO_MEMORY;
    }

    // Render The Current Room
    Status s = room_render(room, &eng->charset, buffer, width, height);
    if(s != OK){
        free(buffer);
        return s;
    }

    // Get The Fully Rendered String Of The Room
    s = game_engine_helper_buffer_to_string(buffer, width, height, str_out);

    free(buffer);
    return s;
}

// Function To Get The Room IDs
Status game_engine_get_room_ids(const GameEngine *eng, int **ids_out, int *count_out){

    // Handle NULL Values 
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }
    if(ids_out == NULL || count_out == NULL){
        return NULL_POINTER;
    }

    const void * const *payloads = NULL;
    int count = 0;

    // Get All The Rooms
    graph_get_all_payloads(eng->graph, &payloads, &count);

    // Allocate Space To Store The IDs
    int *ids = malloc(sizeof(int) * count);
    if(ids == NULL){
        return NO_MEMORY;
    }

    // Iterate Through All Rooms 
    for(int i = 0; i < count; i++){

        // Add An Individual Rooms ID To The Array
        const Room *r = (const Room *)payloads[i];
        ids[i] = r->id;
    }

    *ids_out = ids;
    *count_out = count;
    return OK;
}

// Function To Free The Buffer
void game_engine_free_string(void *ptr){

    if(ptr == NULL){
        return;
    }

    free(ptr);
}

/* ==================================================================
 * Functions For UI And Collect All Treasures Extensions 
 * ================================================================== */

// Function To Get The Adjacency Matrix For Rooms In The Graph (Used For Mini Map)
Status game_engine_get_adjacency_matrix(const GameEngine *eng, int **matrix_out, int **ids_out, int *count_out){

    // Handle NULL Values
    if(eng == NULL || matrix_out == NULL || ids_out == NULL || count_out == NULL){
        return INVALID_ARGUMENT;
    }

    const void * const *payloads = NULL;
    int count = 0;

    // Get All Rooms In The Graph
    graph_get_all_payloads(eng->graph, &payloads, &count);

    // Allocate Space For Room IDs Array
    int *ids = malloc(sizeof(int) * count);
    if(ids == NULL){
        return NO_MEMORY;
    } 

    // Get The IDs For Each Room  
    for(int i = 0; i < count; i++){
        ids[i] = ((Room *)payloads[i])->id;
    }

    // Allocate Space For The Adjacency Matrix Use Calloc To Set All Values To 0
    int *matrix = calloc((size_t)count * (size_t)count, sizeof(int)); 
    if(matrix == NULL){
        free(ids);
        return NO_MEMORY;
    }

    // Fill The Adjacency Matrix Using Portal Connections
    for(int i = 0; i < count; i++){
        Room *r = (Room *)payloads[i];

        // Iterate Through Each Portal In The Room And Find The Room It Connects To
        for(int p = 0; p < r->portal_count; p++){
            int target_id = r->portals[p].target_room_id;

            // Find The Index In The Marix Of The Target Room And Set It In The Adjacency Matrix
            for(int j = 0; j < count; j++){
                if(ids[j] == target_id){
                    // For The ith Room In The Count Number Of Rooms + j
                    matrix[i * count + j] = 1;
                    break;
                }
            }
        }
    }

    *matrix_out = matrix;
    *count_out = count;
    *ids_out = ids;
    return OK;
}

// Function To Get The Total Number Of Treasures In All Rooms
int game_engine_get_total_treasures(const GameEngine *eng){

    // Handle NULL Values
    if(eng == NULL){
        return -1;
    } 

    const void * const *payloads = NULL;
    int count = 0;

    // Get All Rooms In The Graph
    graph_get_all_payloads(eng->graph, &payloads, &count);

    int total = 0;

    // Get The Treasure Count For Each Room
    for(int i = 0; i < count; i++){
        Room *r = (Room *)payloads[i];
        total += r->treasure_count; 
    }

    return total;
}

// Function To Get The Number Of Uncollected Treasures In A Room
int game_engine_get_room_uncollected_treasures(const GameEngine *eng, int room_id){

    // Handle NULL Values
    if(eng == NULL){
        return -1;
    }

    // Get The Room From The Input ID 
    Room *room = game_engine_helper_find_room_by_id(eng->graph, room_id);
    if(room == NULL){
        return -1;
    } 

    int count = 0;

    // Iterate Through The Treasure In The Room
    for(int i = 0; i < room->treasure_count; i++){
        
        // Increment The Count Of Uncollected Treasure If The Treasure Is Not Collected 
        if(room->treasures[i].collected == false){
            count++;
        }
    }
    return count;
}

Status game_engine_get_room_dimensions_by_id(const GameEngine *eng, int room_id, int *width_out, int *height_out){

    // Handle NULL Values
    if(eng == NULL){
        return INVALID_ARGUMENT;
    }
    if(width_out == NULL || height_out == NULL){
        return NULL_POINTER;
    }

    // Get The Input Room 
    Room *room = game_engine_helper_find_room_by_id(eng->graph, room_id);
    if(room == NULL){
        return GE_NO_SUCH_ROOM;
    }

    // Get The Width And Height From The Room
    *width_out = room_get_width(room);
    *height_out = room_get_height(room);

    return OK;
}
