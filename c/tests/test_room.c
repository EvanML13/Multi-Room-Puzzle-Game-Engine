#include "check.h"
#include <stdlib.h>
#include "room.h"

/* ==================================================================
 * Setup And Teardown Fixtures
 * ================================================================== */

// Initialize A Room For Testing
static Room *r = NULL;

// Create A Room (Static Values: ID 1, Name "Test Room")
static void setup_room(void){
    r = room_create(1, "Test Room", 28, 29);
    ck_assert_ptr_nonnull(r);
}

// Destroy The Room After Testibg
static void teardown_room(void){
    room_destroy(r);
    r = NULL;
}

// Helper Function To Initialize Treasure For Testing
static Treasure *create_treasure(int id, int x, int y){
    Treasure *t = malloc(sizeof(Treasure));
    if(t != NULL){
        t->id = id;
        t->x = x;
        t->y = y;
        t->collected = false;
    }
    return t;
} 

// Helper Function To Initialize A Pushable For Testing
static void create_pushable(Room *r, int x, int y){
    Pushable *p = malloc(sizeof(Pushable));
    if(p != NULL){
        p[0].x = x;
        p[0].y = y;

        r->pushables = p;
        r->pushable_count = 1;
    }
}



/* =============================================
 * Test 1: Room Creation
 * Tests room_create Successfully Creates A Room
 * ============================================= */
START_TEST(test_room_create){

    // Verify That The Room Was Correctly Created In Setup
    ck_assert_int_eq(r->id, 1);
    ck_assert_str_eq(r->name, "Test Room");
    ck_assert_int_eq(r->width, 28);
    ck_assert_int_eq(r->height, 29);
    
    ck_assert_ptr_eq(r->floor_grid, NULL);
    ck_assert_ptr_eq(r->portals, NULL);
    ck_assert_int_eq(r->portal_count, 0);
    ck_assert_ptr_eq(r->treasures, NULL);
    ck_assert_int_eq(r->treasure_count, 0);
}
END_TEST

/* =============================================================
 * Test 2: Independent Room Creation
 * Tests room_create Successfully Creates A Room Inside The Test
 * ============================================================= */
START_TEST(test_room_create_independent){

    // Create Two New Rooms Locally
    Room *localRoom = NULL;
    Room *localRoom2 = NULL;
    localRoom = room_create(3, NULL, 0, 0);
    localRoom2 = room_create(100000, "ROOM!", 2, 2);
    
    // Verify That Both The Rooms Were Correctly Created
    ck_assert_ptr_nonnull(localRoom);
    ck_assert_int_eq(localRoom->id, 3);
    ck_assert_ptr_eq(localRoom->name, NULL);
    ck_assert_int_eq(localRoom->width, 1);
    ck_assert_int_eq(localRoom->height, 1);

    ck_assert_ptr_eq(localRoom->floor_grid, NULL);
    ck_assert_ptr_eq(localRoom->portals, NULL);
    ck_assert_int_eq(localRoom->portal_count, 0);
    ck_assert_ptr_eq(localRoom->treasures, NULL);
    ck_assert_int_eq(localRoom->treasure_count, 0);

    ck_assert_ptr_nonnull(localRoom2);
    ck_assert_int_eq(localRoom2->id, 100000);
    ck_assert_str_eq(localRoom2->name, "ROOM!");
    ck_assert_int_eq(localRoom2->width, 2);
    ck_assert_int_eq(localRoom2->height, 2);

    ck_assert_ptr_eq(localRoom2->floor_grid, NULL);
    ck_assert_ptr_eq(localRoom2->portals, NULL);
    ck_assert_int_eq(localRoom2->portal_count, 0);
    ck_assert_ptr_eq(localRoom2->treasures, NULL);
    ck_assert_int_eq(localRoom2->treasure_count, 0);
}
END_TEST

/* =========================================================
 * Test 3: Room Get Width
 * Tests room_get_width Successfully Returns The Rooms Width
 * ========================================================= */
START_TEST(test_room_get_width){

    // Verify The Rooms Width Is Correct
    ck_assert_int_eq(room_get_width(r), 28);
}
END_TEST

/* =====================================================
 * Test 4: Room Get Width With NULL Room
 * Tests room_get_width Successfully Detects A NULL Room
 * ===================================================== */
START_TEST(test_room_get_width_null_room){

    // Attempt To Get A The Room Width With A NULL Room
    ck_assert_int_eq(room_get_width(NULL), 0);
}

/* ==========================================================
 * Test 5: Room Get Height
 * Tests room_get_heigh Successfully Returns The Rooms height
 * ========================================================== */
START_TEST(test_room_get_height){

    // Verify The Rooms Height Is Correct
    ck_assert_int_eq(room_get_height(r), 29);
}
END_TEST

/* =====================================================
 * Test 6: Room Get Height With NULL Room
 * Tests room_get_height Successfully Detects A NULL Room
 * ===================================================== */
START_TEST(test_room_get_height_null_room){

    // Attempt To Get A The Room Width With A NULL Room
    ck_assert_int_eq(room_get_height(NULL), 0);
}
END_TEST

/* ==========================================================
 * Test 7: Room Set Floor Grid
 * Tests room_set_floor_grid Successfully Sets The Floor Grid
 * ========================================================== */
START_TEST(test_room_set_floor_grid){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);
}

/* ==========================================================
 * Test 8: Room Set Floor Grid With NULL Room
 * Tests room_set_floor_grid Successfully Detects A Null Room
 * ========================================================== */
START_TEST(test_room_set_floor_grid_null_room){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);
    
    // Attempt To Set The Floor Grid With A NULL Room
    ck_assert_int_eq(room_set_floor_grid(NULL, floor_grid), INVALID_ARGUMENT);
}

/* =========================================================
 * Test 9: Room Set Portals
 * Tests room_set_portals Successfully Sets The Room Portals
 * ========================================================= */
START_TEST(test_room_set_portals){

    // Create A Portal 
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = "Test Portal";
    p->x = 5;
    p->y = 5;
    p->target_room_id = 2;

    // Set The Portal In The Room
    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    // Verify The Portal Was Added Correctly
    ck_assert_int_eq(r->portal_count, 1);
    ck_assert_int_eq(r->portals[0].id, 1);
    ck_assert_str_eq(r->portals[0].name, "Test Portal");
    ck_assert_int_eq(r->portals[0].x, 5);
    ck_assert_int_eq(r->portals[0].y, 5);
    ck_assert_int_eq(r->portals[0].target_room_id, 2);
}
END_TEST

/* =========================================================
 * Test 10: Room Set Portals With NULL Room
 * Tests room_set_portals Successfully Detects A NULL Room
 * ========================================================= */
START_TEST(test_room_set_portals_null_room){

    // Create A Portal 
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = "Test Portal";
    p->x = 5;
    p->y = 5;
    p->target_room_id = 2;

    // Attempt To Set The Portal With A NULL Room
    ck_assert_int_eq(room_set_portals(NULL, p, 1), INVALID_ARGUMENT);
}
END_TEST

/* =============================================================
 * Test 11: Room Set Treasures
 * Tests room_set_treasures Successfully Sets The Room Treasures
 * ============================================================= */
START_TEST(test_room_set_treasures){

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Set The Treasure In The Room
    ck_assert_int_eq(room_set_treasures(r, t, 1), OK);

    // Verify The Treasure Was Added Correctly
    ck_assert_int_eq(r->treasure_count, 1);
    ck_assert_int_eq(r->treasures[0].id, 1);
    ck_assert_str_eq(r->treasures[0].name, "Test Treasure");
    ck_assert_int_eq(r->treasures[0].initial_x, 10);
    ck_assert_int_eq(r->treasures[0].initial_y, 10);
    ck_assert_int_eq(r->treasures[0].x, 10);
    ck_assert_int_eq(r->treasures[0].y, 10);
    ck_assert_int_eq(r->treasures[0].collected, false);
}
END_TEST

/* ==========================================================
 * Test 12: Room Set Treasures With NULL Room
 * Tests room_set_treasures Successfully Detects A NULL Room
 * ========================================================= */
START_TEST(test_room_set_treasures_null_room){

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Attempt To Set The Treasure With A NULL Room
    ck_assert_int_eq(room_set_treasures(NULL, t, 1), INVALID_ARGUMENT);
}
END_TEST

/* ====================================================================
 * Test 13: Room Place Treasure
 * Tests room_place_treasure Successfully Places A Treasure In The Room
 * ==================================================================== */
START_TEST(test_room_place_treasure){

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Place The Treasure In The Room
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    // Verify The Treasure Was Placed Correctly
    ck_assert_int_eq(r->treasure_count, 1);
    ck_assert_int_eq(r->treasures[0].id, 1);
    ck_assert_str_eq(r->treasures[0].name, "Test Treasure");
    ck_assert_int_eq(r->treasures[0].initial_x, 10);
    ck_assert_int_eq(r->treasures[0].initial_y, 10);
    ck_assert_int_eq(r->treasures[0].x, 10);
    ck_assert_int_eq(r->treasures[0].y, 10);
    ck_assert_int_eq(r->treasures[0].collected, false);
}
END_TEST

/* ==========================================================
 * Test 14: Room Place Treasures With NULL Room
 * Tests room_place_treasure Successfully Detects A NULL Room
 * ========================================================= */
START_TEST(test_room_place_treasure_null_room){

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Attempt To Place The Treasure With A NULL Room
    ck_assert_int_eq(room_place_treasure(NULL, t), INVALID_ARGUMENT);
}
END_TEST

/* =======================================================================
 * Test 15: Room Get Treasure
 * Tests room_get_treasure_at Successfully Returns The Correct Treasure ID
 * ======================================================================= */
START_TEST(test_room_get_treasure_at){

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Place The Treasure In The Room
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    // Verify The Correct Treasure ID Is Returned For The Given Position
    ck_assert_int_eq(room_get_treasure_at(r, 10, 10), 1);
    ck_assert_int_eq(room_get_treasure_at(r, 0, 0), -1); // No Treasure At This Position
}
END_TEST

/* ============================================================
 * Test 16: Room Get Treasure With NULL Room
 * Tests room_get_treasure_at Successfully Detects A NULL Room
 * =========================================================== */
START_TEST(test_room_get_treasure_at_null_room){

    // Verify That A NULL Room Returns -1
    ck_assert_int_eq(room_get_treasure_at(NULL, 10, 10), -1);

    // Verify That Treasure Is Not Found In An Empty Room
    ck_assert_int_eq(room_get_treasure_at(r, 10, 10), -1);
}
END_TEST

/* ===============================================================
 * Test 17: Room Get Portal Destination
 * Tests room_get_portal_destination Returns The Correct Portal ID
 * =============================================================== */
START_TEST(test_room_get_portal_destination){

    // Create A Portal 
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = "Test Portal";
    p->x = 5;
    p->y = 5;
    p->target_room_id = 2;

    // Set The Portal In The Room
    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    // Verify The Correct Portal ID Is Returned For The Given Position
    ck_assert_int_eq(room_get_portal_destination(r, 5, 5), 2);
    ck_assert_int_eq(room_get_portal_destination(r, 0, 0), -1); // No Portal At This Position
}
END_TEST

/* ==================================================================
 * Test 18: Room Get Portal Destination With NULL Room
 * Tests room_get_portal_destination Successfully Detects A NULL Room
 * ================================================================== */
START_TEST(test_room_get_portal_destination_null_room){
    
    // Verify That A NULL Room Returns -1
    ck_assert_int_eq(room_get_portal_destination(NULL, 5, 5), -1);

    // Verify That Portal Is Not Found In An Empty Room
    ck_assert_int_eq(room_get_portal_destination(r, 5, 5), -1);
}
END_TEST

/* ===========================================================
 * Test 19: Room Is Walkable 
 * Tests room_is_walkable Successfully Detects A Walkable Room
 * =========================================================== */
START_TEST(test_room_is_walkable){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Check That Each Walkable Tile Is Walkable
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                ck_assert(room_is_walkable(r, x, y));
            }
            else{
                ck_assert(!room_is_walkable(r, x, y));
            }
        }
    }
}
END_TEST

/* ==========================================================
 * Test 20: Room Is Walkable With No Grid 
 * Tests room_is_walkable Successfully Detects A Missing Grid 
 * ========================================================== */
START_TEST(test_room_is_walkable_no_grid){

    Room *r = room_create(1, NULL, 5, 5);

    // Interior Tiles Should Be Walkable
    for(int y = 1; y < r->height - 1; y++){
        for(int x = 1; x < r->width - 1; x++){
            ck_assert(room_is_walkable(r, x, y));
        }
    }

    // Perimeter Tiles Should Not Be Walkable
    for(int y = 0; y < r->height; y++){
        for(int x = 0; x < r->width; x++){
            if(x == 0 || y == 0 || x == r->width - 1 || y == r->height - 1){
                ck_assert(!room_is_walkable(r, x, y));
            }
        }
    }

    room_destroy(r);
}
END_TEST

/* =========================================================================
 * Test 21: Room Is Walkable Out Of Bound
 * Tests room_is_walkable Successfully Detects Values That Are Out Of Bounds 
 * ========================================================================= */
START_TEST(test_room_is_walkable_out_of_bounds){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Verify That No Width Or Height Out Of Bounds Are Walkable
    ck_assert(!room_is_walkable(r, -1, 0));
    ck_assert(!room_is_walkable(r, 0, -1));
    ck_assert(!room_is_walkable(r, r->width, 0));
    ck_assert(!room_is_walkable(r, 0, r->height));
}
END_TEST

/* =======================================================
 * Test 22: Room Classify Tile
 * Tests room_classify_tile Successfully Detects All Tiles 
 * ======================================================= */
START_TEST(test_room_classify_tile){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Create A Portal 
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = "Test Portal";
    p->x = 18;
    p->y = 18;
    p->target_room_id = 2;

    // Set The Portal In The Room
    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    // Create A Treasure 
    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->name = "Test Treasure";
    t->initial_x = 10;
    t->initial_y = 10;
    t->x = 10;
    t->y = 10;
    t->collected = false;

    // Place The Treasure In The Room
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    int out_id = 0;

    // Verify Everything Is Correctly Identified
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){

            // Verify Portal Is Correctly Classified 
            if(y == 18 && x == 18){
                ck_assert_int_eq(room_classify_tile(r, x, y, &out_id), ROOM_TILE_PORTAL);
                ck_assert_int_eq(out_id, 2);
            }

            else if(x == 10 && y == 10){
                ck_assert_int_eq(room_classify_tile(r, x, y, &out_id), ROOM_TILE_TREASURE);
                ck_assert_int_eq(out_id, 1);
            }

            else if((x + y) % 2 == 0){
                ck_assert_int_eq(room_classify_tile(r, x, y, NULL), ROOM_TILE_FLOOR);
            }
            else{
                ck_assert_int_eq(room_classify_tile(r, x, y, NULL), ROOM_TILE_WALL);
            }
        }
    }
}
END_TEST

/* ============================================================
 * Test 23: Room Classify Tile With No Grid 
 * Tests room_classify_tile Successfully Detects A Missing Grid 
 * ============================================================ */
START_TEST(test_room_classify_tile_no_grid){

    // Verify That No Width Or Height Out Of Bounds Is Classified As A Tile    
    ck_assert_int_eq(room_classify_tile(r,-1, 0, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 0, -1, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, -1, -1, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(NULL, 1, 1, NULL), ROOM_TILE_INVALID);
}
END_TEST

/* =================================================================
 * Test 24: Room Classify Tile Out Of Bounds
 * Tests room_classify_tile Successfully Detects Tiles Out Of Bounds 
 * ================================================================= */
START_TEST(test_room_classify_tile_out_of_bounds){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Verify That No Width Or Height Out Of Bounds Is Classified As A Tile
    ck_assert_int_eq(room_classify_tile(r, -1, 0, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 0, -1, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, r->width, 0, NULL), ROOM_TILE_INVALID);
    ck_assert_int_eq(room_classify_tile(r, 0, r->height, NULL), ROOM_TILE_INVALID);
}
END_TEST

/* =====================================================
 * Test 25: Room Render NULL Arguments
 * Tests room_render Successfully Detects NULL Arguments 
 * ===================================================== */
START_TEST(test_room_render_null_values){

    // Charcter Set
    Charset cs = {
        .wall = '#',
        .floor = '.',
        .player = '@',
        .treasure = 'T',
        .portal = 'P',
        .pushable = 'O'
    };

    char buffer[10];

    // Verify Room Render Handels NULL Values
    ck_assert_int_eq(room_render(NULL, &cs, buffer, 10, 10), INVALID_ARGUMENT);
    ck_assert_int_eq(room_render(r, NULL, buffer, 10, 10), INVALID_ARGUMENT);
    ck_assert_int_eq(room_render(r, &cs, NULL, 10, 10), INVALID_ARGUMENT);
}
END_TEST

/* ====================================================
 * Test 26: Room Render Out Of Bounds
 * Tests room_render Successfully Detects Out Of Bounds 
 * ==================================================== */
START_TEST(test_room_render_out_of_bounds){

    // Charcter Set
    Charset cs = {
        .wall = '#',
        .floor = '.',
        .player = '@',
        .treasure = 'T',
        .portal = 'P',
        .pushable = 'O'
    };

    char buffer[100];

    // Verify That Out Of Bound Are Correctly Handled
    ck_assert_int_eq(room_render(r, &cs, buffer, room_get_width(r) - 1, room_get_height(r)), INVALID_ARGUMENT);
    ck_assert_int_eq(room_render(r, &cs, buffer, room_get_width(r), room_get_height(r) - 1), INVALID_ARGUMENT);
}
END_TEST

/* ==============================================================
 * Test 27: Room Get Start Position NULL Values
 * Tests room_get_start_position Successfully Detects NULL Values
 * ============================================================== */
START_TEST(test_room_get_start_position_null){

    int x, y;

    // Verify That NULL Values Are Correctly Handeled 
    ck_assert_int_eq(room_get_start_position(NULL, &x, &y), INVALID_ARGUMENT);
    ck_assert_int_eq(room_get_start_position(r, NULL, &y), INVALID_ARGUMENT);
    ck_assert_int_eq(room_get_start_position(r, &x, NULL), INVALID_ARGUMENT);
}
END_TEST

/* =================================================================================
 * Test 28: Room Get Start Position Portal
 * Tests room_get_start_position Successfully Detects A Portal As The Start Position
 * ================================================================================= */
START_TEST(test_room_get_start_position_portal){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Create A Portal 
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->name = "Test Portal";
    p->x = 18;
    p->y = 18;
    p->target_room_id = 2;

    // Set The Portal In The Room
    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    int x, y;

    // Verify The Portal Is Found To Be The Starting Position
    ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
    ck_assert_int_eq(x, 18);
    ck_assert_int_eq(y, 18);
}
END_TEST

/* ========================================================================================
 * Test 29: Room Get Start Position Walkable
 * Tests room_get_start_position Successfully Detects A Walkable Tile As The Start Position
 * ======================================================================================== */
START_TEST(test_room_get_start_position_walkable){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if((x + y) % 2 == 0){
                floor_grid[y * width + x] = true; // Walkable
            }
            else{
                floor_grid[y * width + x] = false; // Non-Walkable
            }
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int x, y;

    // Verify The First Walkable Tile Is Found To Be The Starting Position
    ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
    ck_assert_int_eq(x, 0);
    ck_assert_int_eq(y, 0);
}
END_TEST

/* ======================================================================================
 * Test 30: Room Get Start Position Non Valid
 * Tests room_get_start_position Successfully Detects A Room With No Valid Start Postions
 * ====================================================================================== */
START_TEST(test_room_get_start_position_non_valid){

    // Create A Sample Floor Grid
    int width = room_get_width(r);
    int height = room_get_height(r);
    bool *floor_grid = (bool *)malloc(sizeof(bool) * width * height);
    ck_assert_ptr_nonnull(floor_grid);

    // Initialize The Floor Grid With Alternating Walkable And Non-Walkable Tiles
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            floor_grid[y * width + x] = false; // Non-Walkable
        }
    }

    // Set The Floor Grid In The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    int x, y;

    // Verify There Is No Valid Starting Position
    ck_assert_int_eq(room_get_start_position(r, &x, &y), ROOM_NOT_FOUND);
}
END_TEST

/* ==================================================
 * Test 30: Room Get ID
 * Tests room_get_id Successfully Returns The Room ID
 * ================================================== */
 START_TEST(test_room_get_id){

    // Verify The Room ID Is Correct
    ck_assert_int_eq(room_get_id(r), 1);
 }
 END_TEST

/* ==================================================
 * Test 31: Room Get ID With NULL Room
 * Tests room_get_id Successfully Detects A NULL Room
 * ================================================== */
START_TEST(test_room_get_id_null_room){

    // Verify That A NULL Room Returns -1
    ck_assert_int_eq(room_get_id(NULL), -1);   
}
END_TEST

/* =========================================================================
 * Test 32: Test Room Create And Destroy   
 * Tests Create And Destroy Functions Successfully Create And Destroy A Room
 * ========================================================================= */
START_TEST(test_room_create_destroy){

    // Create A Room
    Room *r = room_create(5, "Test Room", 5, 10);
    ck_assert_ptr_nonnull(r);

    // Verify Dimensions Are Correct
    ck_assert_int_eq(room_get_id(r), 5);
    ck_assert_int_eq(room_get_width(r), 5);
    ck_assert_int_eq(room_get_height(r), 10);

    // Destroy The Room
    room_destroy(r);
}
END_TEST

/* =======================================================================================
 * Test 33: Test Room Successfully Picks Up Treasure 
 * Tests room_pick_up_treasure Successfully Picks Up A Treasure And Updates The Room State
 * ======================================================================================= */

START_TEST(test_room_pickup_treasure_success){

    // Create Treasure And Place It In The Room
    Treasure *t = create_treasure(99, 10, 15);
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    // Verify Treasure Is Found At This Position
    ck_assert_int_eq(room_get_treasure_at(r, 10, 15), 99);

    Treasure *found = NULL;
    ck_assert_int_eq(room_pick_up_treasure(r, 99, &found), OK);

    // Verify The Correct Treasure Was Picked Up
    ck_assert_ptr_nonnull(found);
    ck_assert_ptr_eq(found, &r->treasures[0]);
    ck_assert_int_eq(found->id, 99);
    ck_assert_int_eq(found->x, 10);
    ck_assert_int_eq(found->y, 15);
    ck_assert_int_eq(found->collected, true);

    // Verify Treasure Can No Longer Be Collected
    ck_assert_int_eq(room_get_treasure_at(r, 10, 15), -1);

    destroy_treasure(t);
}
END_TEST

/* =============================================================================
 * Test 34: Test Room Pick Up Treasure With NULL Values
 * Tests room_pick_up_treasure Successfully Detects NULL Values
 * ============================================================================= */
START_TEST(test_room_pickup_treasure_null_values){

    Treasure *found = NULL;

    // Verify That NULL Values Are Correctly Handled
    ck_assert_int_eq(room_pick_up_treasure(NULL, 1, &found), INVALID_ARGUMENT);
    ck_assert_int_eq(room_pick_up_treasure(r, 1, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ==========================================================================================
 * Test 35: Test Room Pick Up With An Non Treasure Tile
 * Tests room_pick_up_treasure Successfully Detects An Attempt To Pick Up A Non Treasure Tile
 * ========================================================================================== */ 
START_TEST(test_room_pickup_treasure_non_treasure){

    Treasure *found = NULL;

    // Verify That Attempting To Pick Up A Non Treasure Tile Is Handled Correctly
    ck_assert_int_eq(room_pick_up_treasure(r, 123, &found), ROOM_NOT_FOUND);
    ck_assert_ptr_eq(found, NULL);
}
END_TEST

/* =================================================================================================================
 * Test 36: Test Room Pick Up With Treasure That Has Already Been Picked Up
 * Tests room_pick_up_treasure Successfully Detects An Attempt To Pick Up A Treasure That Has Already Been Picked Up
 * ================================================================================================================= */
START_TEST(test_room_pickup_treasure_already_collected){

    // Create Treasure And Place It In The Room
    Treasure *t = create_treasure(13, 20, 21);
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    Treasure *found = NULL;
    ck_assert_int_eq(room_pick_up_treasure(r, 13, &found), OK);

    // Verify The Treasure Was Picked Up
    ck_assert_ptr_nonnull(found);
    ck_assert_int_eq(found->id, 13);
    ck_assert_int_eq(found->collected, true);

    // Attempt To Pick Up The Same Treasure Again
    found = NULL;
    ck_assert_int_eq(room_pick_up_treasure(r, 13, &found), INVALID_ARGUMENT);
    ck_assert_ptr_eq(found, NULL);

    destroy_treasure(t);
}

/* ==============================================================
 * Test 37: Test Destroy Treasure Destroys Treasure
 * Tests destroy_treasure Successfully Destroys A Treasure
 * ============================================================= */
START_TEST(test_room_destroy_treasure){

    // Create Treasure
    Treasure *t = create_treasure(42, 5, 5);
    ck_assert_ptr_nonnull(t);

    // Destroy The Treasure
    destroy_treasure(t);
}
END_TEST

/* ============================================================
 * Test 38: Test Destroy Treasure Destroys With A NULL Treasure
 * Tests destroy_treasure Successfully Handles A NULL Treasure
 * =========================================================== */
START_TEST(test_room_destroy_treasure_null){

    // Verify That Destroying A NULL Treasure Is Handled Correctly
    destroy_treasure(NULL);
}
END_TEST

/* ===============================================================================
 * Test 39: Test Room Has Pushable At With A Valid Pushable
 * Tests room_has_pushable_at Successfully Detects A Pushable At The Given Position
 * ================================================================================ */
START_TEST(test_room_has_pushable_at_valid){

    // Create A Pushable And Place It In The Room
    create_pushable(r, 7, 8);

    int index = -1;

    // Verify That The Pushable Is Detected At This Position And Not Any Others
    ck_assert_int_eq(room_has_pushable_at(r, 7, 8, &index), true);
    ck_assert_int_eq(index, 0);
    ck_assert_int_eq(room_has_pushable_at(r, 0, 0, &index), false);
}
END_TEST

/* =====================================================================
 * Test 40: Test Room Has Pushable At With A NULL Values
 * Tests room_has_pushable_at Successfully Detects NULL Values
 * ===================================================================== */
START_TEST(test_room_has_pushable_at_null){

    // Create A Pushable And Place It In The Room
    create_pushable(r, 7, 8);

    int index = -1;

    // Verify That NULL Values Are Handled Correctly
    ck_assert_int_eq(room_has_pushable_at(NULL, 7, 8, &index), false);
    ck_assert_int_eq(room_has_pushable_at(NULL, 7, 8, NULL), false);
    ck_assert_int_eq(room_has_pushable_at(r, 0, 0, &index), false);
}
END_TEST

/* ============================================================
 * Test 41: Test Room Try Push A Pushable
 * Tests room_try_push_pushable Successfully Pushes A Pushable
 * =========================================================== */
 START_TEST(test_room_try_push_success){

    // Create An Open Floor Grid
    bool *floor_grid = (bool *)malloc(sizeof(bool) * r->width * r->height);

    // Initialize The Floor Grid With All Walkable Tiles
    for(int y = 0; y < r->height; y++){
        for(int x = 0; x < r->width; x++){
            floor_grid[y * r->width + x] = true; 
        }
    }

    // Set The Floor Grid For The Room
    room_set_floor_grid(r, floor_grid);

    // Create A Pushable And Place It In The Room
    create_pushable(r, 8, 7);

    int index = -1;


    // Attempt To Push The Pushable Down Into An Open Space
    ck_assert_int_eq(room_try_push(r, 0, DIR_EAST), OK);
    ck_assert_int_eq(room_has_pushable_at(r, 9, 7, &index), true);
    ck_assert_int_eq(index, 0);
}

/* =====================================================================================
 * Test 42: Test Room Try Push A Pushable To A Wall
 * Tests room_try_push_pushable Successfully Detects A Pushable Being Pushed Into A Wall
 * ===================================================================================== */
START_TEST(test_room_try_push_into_wall){

    // Create A Floor Grid With Walls On The Right Edge And Set It
    bool *floor_grid = (bool *)malloc(sizeof(bool) * r->width * r->height);
   
    // Initialize The Floor Grid With Walkable Tiles Except For The Right Edge
    for(int y = 0; y < r->height; y++){
        for(int x = 0; x < r->width; x++){

            if(x == r->width - 1){
                floor_grid[y * r->width + x] = false;
            }
            
            else{
                floor_grid[y * r->width + x] = true;
            }
        }
    }

    // Set The Floor Grid For The Room
    room_set_floor_grid(r, floor_grid);

    // Create A Pushable And Place It Next To The Wall
    create_pushable(r, r->width - 2, 5);

    int index = -1;

    // Attempt To Push The Pushable Into The Wall
    ck_assert_int_eq(room_try_push(r, 0, DIR_EAST), ROOM_IMPASSABLE);
    ck_assert_int_eq(room_has_pushable_at(r, r->width - 2, 5, &index), true);
    ck_assert_int_eq(index, 0);
}

/* =======================================================================
 * Test 43: Test Room Try Push With NULL Values
 * Tests room_try_push_pushable Successfully Detects NULL Values
 * ======================================================================= */
START_TEST(test_room_try_push_null_values){

    // Verify That NULL Values Are Handled Correctly
    ck_assert_int_eq(room_try_push(NULL, 0, DIR_EAST), INVALID_ARGUMENT);
    ck_assert_int_eq(room_try_push(r, -1, DIR_EAST), INVALID_ARGUMENT);
    ck_assert_int_eq(room_try_push(r, 1000, DIR_EAST), INVALID_ARGUMENT);
}
END_TEST

/* =============================================================
 * Test 44: Test Room Rendering Layers
 * Tests room_render Successfully Renders All Layers
 * ============================================================= */
START_TEST(test_room_render_layers){

    // Create The Floor Grid
    bool *floor_grid = (bool *)malloc(sizeof(bool) * r->width * r->height);
    ck_assert_ptr_nonnull(floor_grid);
    
    // Initialize The Floor Grid With All Walkable Tiles
    for(int y = 0; y < r->height; y++){
        for(int x = 0; x < r->width; x++){
            floor_grid[y * r->width + x] = true;
        }
    }

    // Set The Floor Grid For The Room
    ck_assert_int_eq(room_set_floor_grid(r, floor_grid), OK);

    // Create A Portal And Place It In The Room
    Portal *p = malloc(sizeof(Portal));
    p->id = 1;
    p->x = 1;
    p->y = 1;
    p->target_room_id = 2;
    ck_assert_int_eq(room_set_portals(r, p, 1), OK);

    // Create A Treasure And Place It In The Room
    Treasure *t = create_treasure(3, 2, 1);
    ck_assert_int_eq(room_place_treasure(r, t), OK);

    // Create A Character Set
    Charset cs = {
        .wall = '#',
        .floor = '.',
        .player = '@',
        .treasure = 'T',
        .portal = 'P',
        .pushable = 'O'
    };

    char buffer[1000];

    // Render The Room
    ck_assert_int_eq(room_render(r, &cs, buffer, r->width, r->height), OK);

    // Verify The Rendered Output Is Correct
    char expected[1000];
    for(int y = 0; y < r->height; y++){
        for(int x = 0; x < r->width; x++){
            if(x == 1 && y == 1){
                expected[y * r->width + x] = cs.portal;
            }
            else if(x == 2 && y == 1){
                expected[y * r->width + x] = cs.treasure;
            }
            else{
                expected[y * r->width + x] = cs.floor;
            }
        }
    }

    // Use Expected To Prevent Warning About Unused Variable
    ck_assert_str_eq(buffer, expected);
}
END_TEST

/* ===============================================================
 * Test Suite Creation
 * =============================================================== */
Suite *room_test_suite(void){

    // Create A New Suite
    Suite *s = suite_create("Room");

    // Create A Test Case For Player Functions
    TCase *tc = tcase_create("RoomTests");

    // Run Setup And Teardown Functions For Each Test
    tcase_add_checked_fixture(tc, setup_room, teardown_room);

    // Add Individual Test Function To The Test Case
    tcase_add_test(tc, test_room_create);
    tcase_add_test(tc, test_room_create_independent);
    tcase_add_test(tc, test_room_get_width);
    tcase_add_test(tc, test_room_get_width_null_room);
    tcase_add_test(tc, test_room_get_height);
    tcase_add_test(tc, test_room_get_height_null_room);
    tcase_add_test(tc, test_room_set_floor_grid);
    tcase_add_test(tc, test_room_set_floor_grid_null_room);
    tcase_add_test(tc, test_room_set_portals);
    tcase_add_test(tc, test_room_set_portals_null_room);
    tcase_add_test(tc, test_room_set_treasures);
    tcase_add_test(tc, test_room_set_treasures_null_room);
    tcase_add_test(tc, test_room_place_treasure);
    tcase_add_test(tc, test_room_place_treasure_null_room);
    tcase_add_test(tc, test_room_get_treasure_at);
    tcase_add_test(tc, test_room_get_treasure_at_null_room);
    tcase_add_test(tc, test_room_get_portal_destination);
    tcase_add_test(tc, test_room_get_portal_destination_null_room);
    tcase_add_test(tc, test_room_is_walkable);
    tcase_add_test(tc, test_room_is_walkable_no_grid);
    tcase_add_test(tc, test_room_is_walkable_out_of_bounds);
    tcase_add_test(tc, test_room_classify_tile);
    tcase_add_test(tc, test_room_classify_tile_no_grid);
    tcase_add_test(tc, test_room_classify_tile_out_of_bounds);
    tcase_add_test(tc, test_room_render_null_values);
    tcase_add_test(tc, test_room_render_out_of_bounds);
    tcase_add_test(tc, test_room_get_start_position_null);
    tcase_add_test(tc, test_room_get_start_position_portal);
    tcase_add_test(tc, test_room_get_start_position_walkable);
    tcase_add_test(tc, test_room_get_start_position_non_valid);
    tcase_add_test(tc, test_room_get_id);
    tcase_add_test(tc, test_room_get_id_null_room);
    tcase_add_test(tc, test_room_create_destroy);
    tcase_add_test(tc, test_room_pickup_treasure_success);
    tcase_add_test(tc, test_room_pickup_treasure_null_values);
    tcase_add_test(tc, test_room_pickup_treasure_non_treasure);
    tcase_add_test(tc, test_room_pickup_treasure_already_collected);
    tcase_add_test(tc, test_room_destroy_treasure);
    tcase_add_test(tc, test_room_destroy_treasure_null);
    tcase_add_test(tc, test_room_has_pushable_at_valid);
    tcase_add_test(tc, test_room_has_pushable_at_null);
    tcase_add_test(tc, test_room_try_push_success);
    tcase_add_test(tc, test_room_try_push_into_wall);
    tcase_add_test(tc, test_room_try_push_null_values);
    tcase_add_test(tc, test_room_render_layers);

    // Add The Test Case To The Test Suite
    suite_add_tcase(s, tc);

    // Return The Test Suite To Main To Be Ran
    return s;
}
