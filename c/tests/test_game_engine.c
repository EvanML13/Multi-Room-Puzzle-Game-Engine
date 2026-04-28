#include "game_engine.h"
#include "game_engine_helper.h"
#include "check.h"
#include <stdlib.h>
#include "player.h"
#include "room.h"
#include "graph.h"

/* ==================================================================
 * Setup And Teardown Fixtures
 * ================================================================== */

// Initialize A Game Engine For Testing
static GameEngine *eng = NULL;

// Create A Game Engine
static void setup_game_engine(void){
    Status s = game_engine_create("../assets/starter.ini", &eng);
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(eng);
}

// Destroy The Player After Testing 
static void teardown_game_engine(void){
    game_engine_destroy(eng);
    eng = NULL;
}

/* ===========================================================================
 * Test 1: Individual Game Engine Creation
 * Tests game_engine_create Successfully Creates A Game Engine Inside The Test
 * =========================================================================== */
START_TEST(test_game_engine_create_individual){

    // Create Another Game Engine
    GameEngine *e = NULL;
    Status s = game_engine_create("../assets/starter.ini", &e);

    // Verify The Game Engine Is Correctly Created
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(e);

    // Free The Game Engine
    game_engine_destroy(e);
}
END_TEST

/* =======================================================================================
 * Test 2: Game Engine Creation With NULL Values
 * Tests game_engine_create Successfully Detects NULL Values
 * ======================================================================================= */
START_TEST(test_game_engine_create_null){

    // Attempt To Create A NULL Game Engine
    GameEngine *e = NULL;

    // Verify An Error Is Thrown
    ck_assert_int_eq(game_engine_create(NULL, &e), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", NULL), INVALID_ARGUMENT);
}
END_TEST

/* =============================================================
 * Test 3: Game Engine Get Player
 * Tests game_engine_get_player Successfully Returns The Player
 * ============================================================ */
START_TEST(test_game_engine_get_player){
    
    const Player *p = game_engine_get_player(eng);
    
    // Verify The Getter Returns The Player
    ck_assert_ptr_nonnull(p);
}
END_TEST

/* ================================================================
 * Test 4: Game Engine Get Player With NULL Engine
 * Tests game_engine_get_player Successfully Detects A NULL Engine
 * =============================================================== */
START_TEST(test_game_engine_get_player_null){

    const Player *p = game_engine_get_player(NULL);

    // Verify The Getter Returns The NULL
    ck_assert_ptr_eq(p, NULL);
}
END_TEST

/* ==========================================================
 * Test 5: Game Engine Move Player No Grid
 * Tests game_engine_move_player Successfully Detects No Grid 
 * ========================================================== */
START_TEST(test_game_engine_move_player_no_grid){

    // Try To Move With No Grid Such That The Player Runs Into A Wall
    Status s = game_engine_move_player(eng, DIR_NORTH);
    
    // Verify The Outcome Is Correct
    ck_assert(s == ROOM_IMPASSABLE);
}
END_TEST

/* ==============================================================================
 * Test 6: Game Engine Move Player With NULL Engine
 * Tests game_engine_move_player Successfully Detects A NULL Engine
 * ============================================================================== */
START_TEST(test_game_engine_move_player_null){
    // Verify A NULL Engine Is Handled
    ck_assert_int_eq(game_engine_move_player(NULL, DIR_NORTH), INVALID_ARGUMENT);
}
END_TEST

/* ====================================================================
 * Test 7: Game Engine Get Room Count
 * Tests game_engine_get_room_count Successfully Returns The Room Count
 * ==================================================================== */
START_TEST(test_game_engine_get_room_count){

    int count = 0;

    // Get The Room Count
    Status s = game_engine_get_room_count(eng, &count);

    // Verify There Are More Than 0 Rooms
    ck_assert_int_eq(s, OK);
    ck_assert_int_gt(count, 0);
}
END_TEST

/* ==============================================================================
 * Test 8: Game Engine Get Room Count With NULL Engine
 * Tests game_engine_get_room_count Successfully Detects A NULL Engine
 * ============================================================================== */
START_TEST(test_game_engine_get_room_count_null){

    int count = 0;

    // Verify All NULL Values Are Handled
    ck_assert_int_eq(game_engine_get_room_count(NULL, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_get_room_count(eng, NULL), NULL_POINTER);
    ck_assert_int_eq(game_engine_get_room_count(NULL, &count), INVALID_ARGUMENT);
}
END_TEST

/* =========================================================================
 * Test 9: Game Engine Get Room Dimensions
 * Tests game_engine_get_room_dimensions Successfully Returns The Dimensions
 * ========================================================================= */
START_TEST(test_game_engine_get_room_dimensions){

    int width = 0, height = 0;

    // Get The Room Dimensions
    Status s = game_engine_get_room_dimensions(eng, &width, &height);

    // Verify The Dimensions Are Correctly Returned
    ck_assert_int_eq(s, OK);
    ck_assert_int_gt(width, 0);
    ck_assert_int_gt(height, 0);
}
END_TEST

/* ============================================================================================
 * Test 10: Game Engine Get Room Dimensions With NULL Values
 * Tests game_engine_get_room_dimensions Successfully Handles NULL Inputs
 * ============================================================================================ */
START_TEST(test_game_engine_get_room_dimensions_null){

    int width = 0, height = 0;

    // Verify NULL Inputs Are Handled 
    ck_assert_int_eq(game_engine_get_room_dimensions(NULL, NULL, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_get_room_dimensions(NULL, &width, &height), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_get_room_dimensions(eng, NULL, &height), NULL_POINTER);
    ck_assert_int_eq(game_engine_get_room_dimensions(eng, &width, NULL), NULL_POINTER);
}

/* ===========================================================
 * Test 11: Game Engine Reset
 * Tests game_engine_reset Successfully Resets The Game Engine 
 * =========================================================== */
START_TEST(test_game_engine_reset){
    // Verify The Game Engine Resets Correctly
    ck_assert_int_eq(game_engine_reset(eng), OK);
}
END_TEST

/* =============================================================
 * Test 12: Game Engine Reset With NULL Engine
 * Tests game_engine_reset Successfully Detects A NULL Engine
 * ============================================================= */
START_TEST(test_game_engine_reset_null){
    // Verify The Game Engine Handels The NULL Outputs
    ck_assert_int_eq(game_engine_reset(NULL), INVALID_ARGUMENT);
}

/* ===================================================================
 * Test 13: Game Engine Render Current Room
 * Tests game_engine_render_current_room Successfully Renders The Room
 * =================================================================== */
START_TEST(test_game_engine_render_current_room){

    char *out = NULL;

    // Render The Players Current Room
    Status s = game_engine_render_current_room(eng, &out);

    // Verify The Room Is Correctly Rendered
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(out);
    ck_assert_int_gt(strlen(out), 0);

    free(out);
}
END_TEST

/* =================================================================================
 * Test 14: Game Engine Render Current Room With NULL Values
 * Tests game_engine_render_current_room Successfully Detects NULL Values
 * ================================================================================= */
START_TEST(test_game_engine_render_current_room_null){

    char *out = NULL;
    
    // Verify NULL Values Are Handled
    ck_assert_int_eq(game_engine_render_current_room(NULL, &out), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_render_current_room(eng, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_render_current_room(NULL, NULL), INVALID_ARGUMENT);
}

/* =============================================================================
 * Test 15: Game Engine Render Room With Invalid Room ID
 * Tests game_engine_render_room Successfully Detects An Invalid Room ID
 * ============================================================================= */
START_TEST(test_game_engine_render_room_invalid){

    char *out = NULL;

    // Verify An Error Is Thrown
    ck_assert_int_eq(game_engine_render_room(eng, -999, &out), GE_NO_SUCH_ROOM);
}
END_TEST

/* =============================================================================
 * Test 16: Game Engine Render Room With NULL Values
 * Tests game_engine_render_room Successfully Detects NULL Values
 * ============================================================================= */
START_TEST(test_game_engine_render_room_null){
    // Verify An Error Is Thrown
    ck_assert_int_eq(game_engine_render_room(NULL, -999, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_render_room(eng, -999, NULL), NULL_POINTER);
}
END_TEST

/* =============================================================================
 * Test 17: Game Engine Get Room IDs With NULL Values
 * Tests game_engine_get_room_ids Successfully Detects NULL Values
 * ============================================================================= */
START_TEST(test_game_engine_get_room_id){

    int *ids = NULL;
    int count = 0;

    // Get All The Room IDs
    Status s = game_engine_get_room_ids(eng, &ids, &count);

    // Verify The Function Succesfully Returns The IDs
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(ids);
    ck_assert_int_gt(count, 0);

    free(ids);
}
END_TEST

/* ===================================================================================
 * Test 18: Game Engine Get Room IDs With NULL Values
 * Tests game_engine_get_room_ids Successfully Detects NULL Values
 * =================================================================================== */
START_TEST(test_game_engine_get_room_id_null){

    int *ids = NULL;
    int count = 0;

    // Verify NULL Values Are Handled 
    ck_assert_int_eq(game_engine_get_room_ids(NULL, &ids, &count), INVALID_ARGUMENT);
    ck_assert_int_eq(game_engine_get_room_ids(eng, &ids, NULL), NULL_POINTER);
}

/* ======================================================================================
 * Test 19: Game Engine Free String
 * Tests game_engine_free_string Successfully Frees A String Allocated By The Game Engine
 * ====================================================================================== */
START_TEST(test_game_engine_free_string){

    char *str = NULL;

    // Render The Current Room To Get A String Allocated By The Game Engine
    Status s = game_engine_render_current_room(eng, &str);

    // Verify The String Is Correctly Rendered
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(str);
    ck_assert_int_gt(strlen(str), 0);

    // Free The String With The Game Engine Function
    game_engine_free_string(str);
}
END_TEST

/* ================================================================
 * Test 20: Game Engine Free String With NULL Value
 * Tests game_engine_free_string Successfully Handles A NULL Value
 * =============================================================== */
START_TEST(test_game_engine_free_string_null){

    // Verify NULL Values Are Handled 
    game_engine_free_string(NULL);
}
END_TEST

/* ================================================================
 * Test 21: Game Engine Move Player With Invalid Direction
 * Tests game_engine_move_player Successfully Handles An Invalid Direction
 * =============================================================== */
START_TEST(test_game_engine_move_invalid_direction){

    // Verify An Invalid Direction Is Handled
    Status s = game_engine_move_player(eng, 999); 
    ck_assert_int_eq(s, INVALID_ARGUMENT);

}
END_TEST

/* ====================================================================
 * Test 22: Game Engine Render Room With Valid Room ID
 * Tests game_engine_render_room Successfully Renders A Room With A Valid ID
 * ==================================================================== */
START_TEST(test_game_engine_render_room_valid){

    int *ids = NULL;
    int count = 0;

    // Get All The Room IDs To Get A Valid Room ID
    ck_assert_int_eq(game_engine_get_room_ids(eng, &ids, &count), OK);
    ck_assert_int_gt(count, 0);

    char *out = NULL;

    // Render The Room With A Valid Room ID
    Status s = game_engine_render_room(eng, ids[0], &out);

    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(out);
    ck_assert_int_gt(strlen(out), 0);

    free(out);
    free(ids);
}
END_TEST

/* ==============================================================================
 * Test 23: Game Engine Reset After Move
 * Tests game_engine_reset Successfully Resets The Player's Position After Moving
 * ============================================================================== */
START_TEST(test_game_engine_reset_after_move){

    int x1, y1;
    player_get_position((Player*)game_engine_get_player(eng), &x1, &y1);

    // Try To Move
    game_engine_move_player(eng, DIR_SOUTH);

    // Reset
    ck_assert_int_eq(game_engine_reset(eng), OK);

    int x2, y2;
    player_get_position((Player*)game_engine_get_player(eng), &x2, &y2);

    // Player Should Be Back To The Original Position
    ck_assert_int_eq(x1, x2);
    ck_assert_int_eq(y1, y2);
}
END_TEST

/* ========================================================================
 * Test 24: Game Engine Push Blocked
 * Tests game_engine_move_player Successfully Detects When A Push Is Blocked
 * ======================================================================== */
START_TEST(test_game_engine_push_blocked){

    // Try Moving Into A Wall Repeatedly
    Status s = game_engine_move_player(eng, DIR_NORTH);
    ck_assert(s == ROOM_IMPASSABLE);

}
END_TEST

/* =================================================================================
 * Test 25: Game Engine Adjacency Matrix
 * Tests game_engine_get_adjacency_matrix Successfully Retrives The Adjacency Matrix
 * ================================================================================= */
START_TEST(test_game_engine_get_adjacency_matrix)
{
    int *matrix = NULL;
    int *ids = NULL;
    int count = 0;

    Status s = game_engine_get_adjacency_matrix(eng, &matrix, &ids, &count);
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(matrix);
    ck_assert_ptr_nonnull(ids);
    ck_assert_int_gt(count, 0);  // There should be at least one room

    // Check that the matrix is square
    for(int i = 0; i < count; i++){
        for(int j = 0; j < count; j++){
            ck_assert(matrix[i * count + j] == 0 || matrix[i * count + j] == 1);
        }
    }

    free(matrix);
    free(ids);
}
END_TEST

/* =======================================================================================
 * Test 26: Game Engine Total Treasure
 * Tests game_engine_get_total_treasures Successfully Retrives A Valid Number Of Treasures
 * ======================================================================================= */
START_TEST(test_game_engine_get_total_treasures)
{
    int total = game_engine_get_total_treasures(eng);
    ck_assert_int_ge(total, 0); // Total treasures cannot be negative
}
END_TEST

/* ===================================================================================================
 * Test 27: Game Engine Total Uncollected Treasure
 * Tests game_engine_get_total_treasures Successfully Retrives A Valid Number Of Uncollected Treasures
 * =================================================================================================== */
START_TEST(test_game_engine_get_room_uncollected_treasures)
{
    const void * const *payloads = NULL;
    int room_count = 0;
    graph_get_all_payloads(eng->graph, &payloads, &room_count);

    for(int i = 0; i < room_count; i++){
        Room *r = (Room *)payloads[i];
        int uncollected = game_engine_get_room_uncollected_treasures(eng, r->id);
        ck_assert_int_ge(uncollected, 0);
        ck_assert_int_le(uncollected, r->treasure_count);
    }

    // Test invalid room
    int invalid = game_engine_get_room_uncollected_treasures(eng, -1);
    ck_assert_int_eq(invalid, -1);
}
END_TEST

/* =========================================================================
 * Test 28: Game Engine Get Room Dimentions By ID 
 * Tests game_engine_get_dimentions_by_id Successfully Retrives A Valid Room
 * ========================================================================= */

START_TEST(test_game_engine_get_room_dimensions_by_id)
{
    const void * const *payloads = NULL;
    int room_count = 0;
    graph_get_all_payloads(eng->graph, &payloads, &room_count);

    for(int i = 0; i < room_count; i++){
        Room *r = (Room *)payloads[i];
        int w = 0, h = 0;
        Status s = game_engine_get_room_dimensions_by_id(eng, r->id, &w, &h);
        ck_assert_int_eq(s, OK);
        ck_assert_int_gt(w, 0);
        ck_assert_int_gt(h, 0);
    }

    // Test invalid room
    int w = 0, h = 0;
    Status s = game_engine_get_room_dimensions_by_id(eng, -1, &w, &h);
    ck_assert_int_eq(s, GE_NO_SUCH_ROOM);

    // Test NULL pointers
    s = game_engine_get_room_dimensions_by_id(eng, 1, NULL, &h);
    ck_assert_int_eq(s, NULL_POINTER);
    s = game_engine_get_room_dimensions_by_id(eng, 1, &w, NULL);
    ck_assert_int_eq(s, NULL_POINTER);
}
END_TEST

/* ===============================================================
 * Test Suite Creation
 * =============================================================== */
Suite *game_engine_test_suite(void){

    // Create A New Suite
    Suite *s = suite_create("GameEngine");

    // Create A Test Case For Game Engine Functions
    TCase *tc = tcase_create("GameEngineTests");

    // Run Setup And Teardown Functions For Each Test
    tcase_add_checked_fixture(tc, setup_game_engine, teardown_game_engine);

    // Add Individual Test Function To The Test Case
    tcase_add_test(tc, test_game_engine_create_individual);
    tcase_add_test(tc, test_game_engine_create_null);
    tcase_add_test(tc, test_game_engine_get_player);
    tcase_add_test(tc, test_game_engine_get_player_null);
    tcase_add_test(tc, test_game_engine_move_player_no_grid);
    tcase_add_test(tc, test_game_engine_move_player_null);
    tcase_add_test(tc, test_game_engine_get_room_count);
    tcase_add_test(tc, test_game_engine_get_room_count_null);
    tcase_add_test(tc, test_game_engine_get_room_dimensions);
    tcase_add_test(tc, test_game_engine_get_room_dimensions_null);
    tcase_add_test(tc, test_game_engine_reset);
    tcase_add_test(tc, test_game_engine_reset_null);
    tcase_add_test(tc, test_game_engine_render_current_room);
    tcase_add_test(tc, test_game_engine_render_current_room_null);
    tcase_add_test(tc, test_game_engine_render_room_invalid);
    tcase_add_test(tc, test_game_engine_render_room_null);
    tcase_add_test(tc, test_game_engine_get_room_id_null);
    tcase_add_test(tc, test_game_engine_get_room_id);
    tcase_add_test(tc, test_game_engine_free_string);
    tcase_add_test(tc, test_game_engine_free_string_null);
    tcase_add_test(tc, test_game_engine_move_invalid_direction);
    tcase_add_test(tc, test_game_engine_render_room_valid);
    tcase_add_test(tc, test_game_engine_reset_after_move);
    tcase_add_test(tc, test_game_engine_push_blocked);
    tcase_add_test(tc, test_game_engine_get_adjacency_matrix);
    tcase_add_test(tc, test_game_engine_get_total_treasures);
    tcase_add_test(tc, test_game_engine_get_room_uncollected_treasures);
    tcase_add_test(tc, test_game_engine_get_room_dimensions_by_id);
    
    // Add The Test Case To The Test Suite
    suite_add_tcase(s, tc);

    // Return The Test Suite To Main To Be Ran
    return s;
}
