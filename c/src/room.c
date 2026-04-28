#include "room.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

// Helper Function To Render A Single Base Tile (Wall Or Floor)
static char room_render_base_tile(const Room *r, int col, int row, const Charset *charset){

    if(r->floor_grid == NULL){
        if(col == 0 || col == r->width - 1 || row == 0 || row == r->height - 1){
            return charset->wall;
        }
        return charset->floor;
    }
    if(r->floor_grid[row * r->width + col] == true){
        return charset->floor;
    }
    return charset->wall;
}

// Function To Create A New Room
Room *room_create(int id, const char *name, int width, int height){

    // Satisfy Preconditions
    if(width <= 0 || height <= 0){

        // Clamp Dimensions To 1
        width = 1;
        height = 1;
    }

    // Allocate Memory For The Room Struct
    Room *r = (Room *)malloc(sizeof(Room));
    if(r == NULL){
        return NULL;
    }

    // Initialize Room Fields
    r->id = id;
    if(name != NULL){
        r->name = strdup(name); // Name Can Be NULL
    }
    else{
        r->name = NULL;
    }
    r->width = width;
    r->height = height;

    // Initialize All Other Pointers In The Room Are Initalized To NULL
    r->floor_grid = NULL;
    r->portals = NULL;
    r->portal_count = 0;
    r->treasures = NULL;
    r->treasure_count = 0;
    r->pushables = NULL;
    r->pushable_count = 0;

    // Returns The Newly Created Room On Success
    return r;
}

// Function To Get The Rooms Width
int room_get_width(const Room *r){

    if(r == NULL){
        return 0;
    }

    return r->width;
}

// Function To Get The Rooms Height
int room_get_height(const Room *r){

    if(r == NULL){
        return 0;
    }

    return r->height;
}

// Function To Set The Rooms Floor Grid 
Status room_set_floor_grid(Room *r, bool *floor_grid){
    
    // Satisfy Precondition
    if(r == NULL){
        return INVALID_ARGUMENT;
    }

    // Free The Old Floor Grid If It Exists
    if(r->floor_grid != NULL){
        free(r->floor_grid);
    }

    // Set The New Floor Grid 
    r->floor_grid = floor_grid;

    // Return Success
    return OK;
}

// Function To Set Portals In The Room
Status room_set_portals(Room *r, Portal *portals, int portal_count){

    // Satisfy Preconditions
    if(r == NULL || (portal_count > 0 && portals == NULL)){
        return INVALID_ARGUMENT;
    }

    // Free The Old Portals If They Exist
    if(r->portals != NULL){
        free(r->portals);
    }

    // Set The New Portals And Portal Count
    r->portals = portals;
    r->portal_count = portal_count;

    // Return Success
    return OK;
}

// Function To Set Treasure In The Room
Status room_set_treasures(Room *r, Treasure *treasures, int treasure_count){

    // Satisfy Preconditions
    if(r == NULL || (treasure_count > 0 && treasures == NULL)){
        return INVALID_ARGUMENT;
    }

    // Free The Old Treasures If They Exist
    if(r->treasures != NULL){
        free(r->treasures);
    }

    // Set The New Treasures And Treasure Count
    r->treasures = treasures;
    r->treasure_count = treasure_count;

    // Return Success
    return OK;
}

// Function To Add Treasure To The Room
Status room_place_treasure(Room *r, const Treasure *treasure){

    // Satisfy Preconditions
    if(r == NULL || treasure == NULL){
        return INVALID_ARGUMENT;
    }

    // Reallocate The Treasure Array To Accommodate For The New Treasure
    Treasure *new_treasure = (Treasure *)realloc(r->treasures, (r->treasure_count + 1)*sizeof(Treasure));
    if(new_treasure == NULL){
        return NO_MEMORY;
    }

    // Copy The New Treasure Into The Reallocated Array At The End
    new_treasure[r->treasure_count] = *treasure; 

    // Set New Treasure Array And Treasure Count
    r->treasures = new_treasure;
    r->treasure_count += 1;

    // Return Success
    return OK;
}

// Function To Check If Treasure Exists At A Given Position
int room_get_treasure_at(const Room *r, int x, int y){

    // Verify The Room And Treasure Array Both Exist
    if(r == NULL || r->treasures == NULL){
        return -1;
    }

    // Iterate Through All The Treasure In The Room
    for(int i = 0; i < r->treasure_count; i++){
        
        // Checks If A Given Treasure Has Matching X And Y Coordinates With The Input 
        if(r->treasures[i].x == x && r->treasures[i].y == y && r->treasures[i].collected == false){

            // Return The Treasure ID If Found
            return r->treasures[i].id;
        }
    }

    // Return -1 If Treasure Is Not Found
    return -1;
}

// Function To Check If A Portal Exists At A Given Position
int room_get_portal_destination(const Room *r, int x, int y){

    // Verify The Room And Portal Array Exist
    if(r == NULL || r->portals == NULL){
        return -1;
    }

    // Iterate Through All The Portals In The Room 
    for(int i = 0; i < r->portal_count; i++){

        // Checks If A Given Portal Has Matching X And Y Coordinates With The Input
        if(r->portals[i].x == x && r->portals[i].y == y){
            
            // Return The Portal ID If Found
            return r->portals[i].target_room_id;
        }
    }

    // Return -1 If Portal Is Not Found
    return -1;
}

// Function To Determine Weither A Tile Is Walkable
bool room_is_walkable(const Room *r, int x, int y){

    // Verify The Room Exists
    if(r == NULL){
        return false;
    }

    // Verify The Given Coordinates Are Within Bounds
    if(x < 0 || x >= r->width || y < 0 || y >= r->height){
        return false;
    }

    // A Pushable Tile Is Not Walkable
    if(room_has_pushable_at(r, x, y, NULL)){
        return false;
    }

    // Verify The Grid Exists
    if(r->floor_grid == NULL){
        if(x == 0 || y == 0 || x == r->width-1 || y == r->height-1){
            return false;
        }
        return true;
    }

    return r->floor_grid[y * r->width + x];
}

// Function To Classify What Type A Tile Is
RoomTileType room_classify_tile(const Room *r, int x, int y, int *out_id){

    // Verify The Room Exists
    if(r == NULL){
        return ROOM_TILE_INVALID;
    }

    // Verify The Coordinates Are Within Bounds
    if(x < 0 || x >= r->width || y < 0 || y >= r ->height){
        return ROOM_TILE_INVALID;
    }

    int id = -1;

    // Check For Treasure Using The room_get_treasure_at Function
    id = room_get_treasure_at(r, x, y);

    // If Treasure Is Found The Tile Must Be Treasure 
    if(id != -1){

        // Return The Treasure ID And Tile Classification
        if(out_id != NULL){
            *out_id = id;
        }
        return ROOM_TILE_TREASURE;
    }

    // Check For A Portal Using The room_get_portal_destination Function
    id = room_get_portal_destination(r, x, y);

    // If Portal Is Found The Tile Must Be Portal 
    if(id != -1){

        // Return The Portal ID And Tile Classification
        if(out_id != NULL){
            *out_id = id;
        }
        return ROOM_TILE_PORTAL;
    }

    int push_index = 0;

    // Check If The Tile Is Pushable
    if(room_has_pushable_at(r, x, y, &push_index)){
        
        // Return The Push Index In The Array Of Pushable Elements And Tile Classification
        if(out_id != NULL){
            *out_id = push_index;
        }
        return ROOM_TILE_PUSHABLE;
    }

    // If The Tile Is Walkable Then It Is A Floor Tile
    if(room_is_walkable(r, x, y) == true){
        return ROOM_TILE_FLOOR;
    }

    // Otherwise The Tile Must Be A Wall
    return ROOM_TILE_WALL;   
}

// Function To Render The Room Into A Pre-Allocated Buffer
Status room_render(const Room *r, const Charset *charset, char *buffer, int buffer_width, int buffer_height){

    // Verify The Room, Charset, And Buffer Are Ready To Be Rendered
    if(r == NULL || charset == NULL || buffer == NULL){
        return INVALID_ARGUMENT;
    }

    // Verify The Buffer Dimensions Match The Room Dimensions
    if(buffer_width != r->width || buffer_height != r->height){
        return INVALID_ARGUMENT;
    }

    // Base Layer Rendering: Using The Floor Grid To Render Walls And Floors
    
    // Itterate For The Height Of The Room
    for(int row = 0; row < r->height; row++){

        // Itterate For The Width Of The Room
        for(int col = 0; col < r->width; col++){

            if(room_has_pushable_at(r, col, row, NULL)){
                buffer[row * r->width + col] = charset->pushable;
            }
            else if(room_get_treasure_at(r, col, row) != -1){
                buffer[row * r->width + col] = charset->treasure;
            }
            else if(room_get_portal_destination(r, col, row) != -1){
                buffer[row * r->width + col] = charset->portal;
            }
            else{
                buffer[row * r->width + col] = room_render_base_tile(r, col, row, charset);
            }
        }
    }

    // Return Success
    return OK;
}

// Function To Determine If A Given Position Is A Valid Starting Position In The Room
Status room_get_start_position(const Room *r, int *x_out, int *y_out){

    // Verify The Room And Output Coordinates Are Not NULL
    if(r == NULL || x_out == NULL || y_out == NULL){
        return INVALID_ARGUMENT; 
    }

    // Preference 1: First Portal Location
    
    // Iterate Through All Portals In The Room
    for(int i = 0; i < r->portal_count; i++){

        // Set The Output Coordinates To The First Portal Found
        *x_out = r->portals[i].x;
        *y_out = r->portals[i].y;

        // Return Success
        return OK;
    }

    // Preference 2: Any Interior Walkable Tile
    int start_x = 0;
    int end_x = r->width;
    int start_y = 0;
    int end_y = r->height;

    if(r->floor_grid == NULL){
        start_x = 1;
        start_y = 1;
        end_x = r->width-1;
        end_y = r->height-1;
    }

    // Iterate Through All Walkable Tiles In The Room
    for(int y = start_y; y < end_y; y++){
        for(int x = start_x; x < end_x; x++){

            // If A Walkable Tile Is Found
            if(room_is_walkable(r, x, y) == true && room_get_treasure_at(r, x, y) == -1){

                // Set The Output Coordinates To The First Walkable Tile Found
                *x_out = x;
                *y_out = y;

                // Return Success
                return OK;
            }
        }
    }

    // If No Valid Starting Position Is Found Return ROOM_NOT_FOUND
    return ROOM_NOT_FOUND;
}

// Function To Free All Memory Allocated For The Room
void room_destroy(Room *r){

    // Safe To Call On NULL
    if(r == NULL){
        return;
    }

    // Free The Room Name If It Exists
    if(r->name != NULL){
        free(r->name);
    }

    // Free The Floor Grid If It Exists
    if(r->floor_grid != NULL){
        free(r->floor_grid);
    }

    // Free The Portals If They Exist
    if(r->portals != NULL){
        free(r->portals);
    }

    // Free The Treasure If It Exists
    if(r->treasures != NULL){
        free(r->treasures);
    }

    // Free The Room
    free(r);
}

// Function To Get The Room ID 
int room_get_id(const Room *r){

    if(r == NULL){
        return -1;
    }

    return r->id;
}

// Function To Remove The Treasure From A Room 
Status room_pick_up_treasure(Room *r, int treasure_id, Treasure **treasure_out){

    // Satisfy Preconditions 
    if(r == NULL || treasure_out == NULL || treasure_id < 0){
        return INVALID_ARGUMENT;
    } 

    // Iterate Throught The List Of Treasure
    for(int i = 0; i < r->treasure_count; i++){

        // Collects The Treasure If Found
        if(r->treasures[i].id == treasure_id){

            // Return Invalid If The Treasure Was Already Collected 
            if(r->treasures[i].collected == true){
                return INVALID_ARGUMENT;
            }

            // Collect And Output The Treasure
            r->treasures[i].collected = true;
            *treasure_out = &r->treasures[i];
            return OK;
        }
    }
    return ROOM_NOT_FOUND;
}

// Function To Free The Treasure
void destroy_treasure(Treasure *t){

    if (t == NULL){
        return;
    }

    free(t);
}

// Function To Deterime Weither A Given Tile Is Pushable
bool room_has_pushable_at(const Room *r, int x, int y, int *pushable_idx_out){

    // Satisfy Preconditions
    if(r == NULL || r->pushables == NULL){
        return false;
    }

    // Iterate Though All The Pushable Tiles In The List
    for(int i = 0; i < r->pushable_count; i++){

        // Check If A Pushable Tile Exists At The Given Coordinates
        if(r->pushables[i].x == x && r->pushables[i].y == y){

            if(pushable_idx_out != NULL){
                
                // Set The Index As The Output 
                *pushable_idx_out = i;
            }

            return true;
        }
    }

    return false;
}

// Function To Push The Tile In A Given Direction
Status room_try_push(Room *r, int pushable_idx, Direction dir){

    // Satisfy Preconditions
    if(r == NULL || pushable_idx < 0|| pushable_idx >= r->pushable_count){
        return INVALID_ARGUMENT;
    }

    int displace_x = 0;
    int displace_y = 0;

    // Update Tile Position Based On Input Direction
    if(dir==DIR_NORTH){
        displace_y--; 
    }
    else if(dir==DIR_SOUTH){
        displace_y++;
    }
    else if(dir==DIR_EAST){
        displace_x++;
    }
    else if(dir==DIR_WEST){
        displace_x--;
    }
    else{
        return INVALID_ARGUMENT;
    }

    // Get The New X And Y Positions 
    int new_x = r->pushables[pushable_idx].x + displace_x;
    int new_y = r->pushables[pushable_idx].y + displace_y;

    // Verify The Tile Is Walkable
    if(room_is_walkable(r, new_x, new_y) == false){
        return ROOM_IMPASSABLE;
    }

    // Verify The Pushable Object Is Not Pushed Into Another Pushable Object
    for(int i = 0; i < r->pushable_count; i++){
        if(i != pushable_idx && r->pushables[i].x == new_x && r->pushables[i].y == new_y){
            return ROOM_IMPASSABLE;
        }
    }

    // Update The Pushable Objects Location
    r->pushables[pushable_idx].x = new_x;
    r->pushables[pushable_idx].y = new_y;

    return OK;
}