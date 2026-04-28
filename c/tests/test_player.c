#include "check.h"
#include <stdlib.h>
#include "player.h"

/* ==================================================================
 * Setup And Teardown Fixtures
 * ================================================================== */

// Initialize A Player For Testing
static Player *p = NULL;

// Create A Player (Static Values: Room ID 1, Position (10, 10))
static void setup_player(void){
    player_create(1, 10, 15, &p);
    ck_assert_ptr_nonnull(p);
}

// Destroy The Player After Testing
static void teardown_player(void){
    player_destroy(p);
    p = NULL;
}

// Helper Function To Initialize Treasure For Testing
static Treasure *create_treasure(int id){
    Treasure *t = malloc(sizeof(Treasure));
    if(t != NULL){
        t->id = id;
        t->collected = false;
    }
    return t;
} 

/* =========================================================
 * Test 1: Player Creation
 * Tests player_create Successfully Creates A Player
 * ========================================================= */
START_TEST(test_player_create){

    // Verify That The Player Was Correctly Created In Setup
    ck_assert_int_eq(p->room_id, 1);
    ck_assert_int_eq(p->x, 10);
    ck_assert_int_eq(p->y, 15);
}
END_TEST

/* =================================================================
 * Test 2: Independent Player Creation
 * Tests player_create Successfully Creates A Player Inside The Test
 * ================================================================= */
START_TEST(test_player_create_independent){

    // Create Two New Players Locally
    Player *localPlayer = NULL;
    Player *localPlayer2 = NULL;
    ck_assert_int_eq(player_create(2, 20, 25, &localPlayer), OK);
    ck_assert_int_eq(player_create(-3, -30, -35, &localPlayer2), OK);

    // Verify That Both The Players Were Correctly Created
    ck_assert_ptr_nonnull(localPlayer);
    ck_assert_int_eq(localPlayer->room_id, 2);
    ck_assert_int_eq(localPlayer->x, 20);
    ck_assert_int_eq(localPlayer->y, 25);

    ck_assert_ptr_nonnull(localPlayer2);
    ck_assert_int_eq(localPlayer2->room_id, -3);
    ck_assert_int_eq(localPlayer2->x, -30);
    ck_assert_int_eq(localPlayer2->y, -35);

    // Distroy The Locally Created Player
    player_destroy(localPlayer);
    player_destroy(localPlayer2);
}
END_TEST

/* ===================================================================
 * Test 3: Player Creation With NULL player_out
 * Tests player_create Successfully Detects A NULL player_out Argument
 * =================================================================== */
START_TEST(test_player_create_null_player_out){

    // Attempt To Create A Player With A NULL player_out
    ck_assert_int_eq(player_create(1, 10, 15, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ======================================================
 * Test 4: Player Get Room ID
 * Tests player_get_room Successfully Returns The Room ID
 * ====================================================== */
START_TEST(test_player_get_room_id){

    // Verify That The Players Room ID Is Correct
    ck_assert_int_eq(player_get_room(p), 1);
}
END_TEST

/* ========================================================
 * Test 5: Player Get Room ID With NULL Player
 * Tests player_get_room Successfully Detects A NULL Player
 * ======================================================== */
START_TEST(test_player_get_room_id_null_player){

    // Attempt To Get The Room ID With A NULL Player
    ck_assert_int_eq(player_get_room(NULL), -1);
}
END_TEST

/* ===================================================================
 * Test 6: Player Get Position 
 * Tests player_get_position Successfully Retruns The Players Position
 * =================================================================== */
START_TEST(test_player_get_position){

    int x = 0, y = 0;

    // Get The Players Position And Verify It Is Correct
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, 10);
    ck_assert_int_eq(y, 15);
}
END_TEST

/* ==========================================================
 * Test 7: Player Get Position With NULL Values 
 * Tests player_get_position Successfully Detects NULL Values
 * ========================================================== */
START_TEST(test_player_get_position_null_values){

    int x, y;

    // Attempt To Get The Position Of The Player With Verying NULL Arguments And Verify INVALID_ARGUMENT Is Returned
    ck_assert_int_eq(player_get_position(NULL, &x, &y), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(p, NULL, &y), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(p, &x, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(NULL, NULL, &y), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(NULL, &x, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(p, NULL, NULL), INVALID_ARGUMENT);
    ck_assert_int_eq(player_get_position(NULL, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* ================================================================
 * Test 8: Player Set Position 
 * Tests player_set_position Successfully Sets The Players Position
 * ================================================================ */
START_TEST(test_player_set_position){

    int x, y;

    // Set The Players Position To New Values
    ck_assert_int_eq(player_set_position(p, -3, 0), OK);

    // Verify The Players Position Was Correctly Set
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, -3);
    ck_assert_int_eq(y, 0);
}
END_TEST

/* ============================================================
 * Test 9: Player Set Position With NULL Player 
 * Tests player_set_position Successfully Detects A NULL Player
 * ============================================================ */
START_TEST(test_player_set_position_null_player){

    // Attempt To Set The Position Of A NULL Player
    ck_assert_int_eq(player_set_position(NULL, 0, 0), INVALID_ARGUMENT);
}
END_TEST

/* =====================================================================
 * Test 10: Player Move To Room
 * Tests player_move_to_room Successfully Moves The Player To A New Room
 * ===================================================================== */
START_TEST(test_player_move_to_room){

    // Move The Player To A New Room
    ck_assert_int_eq(player_move_to_room(p, -3), OK);

    // Verify The Players Room ID Is Correctly Updated
    ck_assert_int_eq(player_get_room(p), -3);
}
END_TEST

/* =====================================================================
 * Test 11: Player Move To Room With NULL Player
 * Tests player_move_to_room Successfully Detects A NULL Player
 * ===================================================================== */
START_TEST(test_player_move_to_room_null_player){

    // Attempt To Move A NULL Player To A New Room
    ck_assert_int_eq(player_move_to_room(NULL, 0), INVALID_ARGUMENT);
}
END_TEST

/* ===========================================================================
 * Test 12: Player Reset To Start
 * Tests player_reset_to_start Successfully Resets The Player To Initial State
 * =========================================================================== */
START_TEST(test_player_reset_to_start){

    // Reset The Player To The Start Position
    ck_assert_int_eq(player_reset_to_start(p, -3, -3, -3), OK);

    int x, y;

    // Verify The Players Position And Room ID Were Reset Correctly
    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, -3);
    ck_assert_int_eq(y, -3);
    ck_assert_int_eq(player_get_room(p), -3);
}
END_TEST

/* ==============================================================
 * Test 13: Player Reset To Start With NULL Player
 * Tests player_reset_to_start Successfully Detects A NULL Player
 * ============================================================== */
START_TEST(test_player_reset_to_start_null_player){

    // Attempt To Reset A NULL Player To The Start Position
    ck_assert_int_eq(player_reset_to_start(NULL, 0, 0, 0), INVALID_ARGUMENT);
}
END_TEST

/* ==================================================================================
 * Test 14: Player Try To Collect Treasure
 * Tests player_try_collect Successfully Collects Treasures And Updates Player Struct
 * ================================================================================== */
START_TEST(test_player_try_collect_success){

    Treasure *t = create_treasure(20);

    // Try To Collect The Treasure And Verify It Succeeds
    ck_assert_int_eq(player_try_collect(p, t), OK);

    // Verify The Treasure Is Marked As Collected And Player Struct Is Updated
    ck_assert_int_eq((t->collected), true);
    ck_assert_int_eq(player_has_collected_treasure(p, 20), true);
    ck_assert_int_eq(player_get_collected_count(p), 1);

    int count = -1;
    const Treasure * const *collected = player_get_collected_treasures(p, &count);

    // Verify The Collected Treasures Array Is Correct
    ck_assert_ptr_nonnull(collected);
    ck_assert_int_eq(count, 1);
    ck_assert_ptr_eq(collected[0], t);

    free(t);
}
END_TEST

/* =================================================================
 * Test 15: Player Try To Collect Treasure With NULL Values
 * Tests player_try_collect Successfully Detects NULL Values
 * ================================================================= */
START_TEST(test_player_try_collect_null_values){

    Treasure *t = create_treasure(40);

    // Verify That player_try_collect Detects NULL Values
    ck_assert_int_eq(player_try_collect(NULL, t), NULL_POINTER);
    ck_assert_int_eq(player_try_collect(p, NULL), NULL_POINTER);
    ck_assert_int_eq(player_try_collect(NULL, NULL), NULL_POINTER);

    ck_assert_int_eq((t->collected), false);
    ck_assert_int_eq(player_has_collected_treasure(p, 40), false);
    ck_assert_int_eq(player_get_collected_count(p), 0);

    free(t);
}
END_TEST

/* ========================================================================
 * Test 16: Player Try To Collect Treasure That Has Already Been Collected
 * Tests player_try_collect Successfully Detects Collected Treasure
 * ======================================================================== */
START_TEST(test_player_try_collect_collected){

    Treasure *t = create_treasure(1);

    // Collect The Treasure
    ck_assert_int_eq(player_try_collect(p, t), OK);

    // Verify That The Treasure Is Collected And Player State Is Updated
    ck_assert_int_eq((t->collected), true);
    ck_assert_int_eq(player_has_collected_treasure(p, 1), true);
    ck_assert_int_eq(player_get_collected_count(p), 1);

    // Try To Collect The Treasure Again
    ck_assert_int_eq(player_try_collect(p, t), INVALID_ARGUMENT);

    // Verify Nothing Has Changed
    ck_assert_int_eq((t->collected), true);
    ck_assert_int_eq(player_has_collected_treasure(p, 1), true);
    ck_assert_int_eq(player_get_collected_count(p), 1);

    int count = -1;
    const Treasure * const *collected = player_get_collected_treasures(p, &count);

    // Verify The Collected Treasures Array Is Still Correct
    ck_assert_int_eq(count, 1);
    ck_assert_ptr_eq(collected[0], t);

    free(t);
}
END_TEST

/* =================================================================
 * Test 17: Player Has Collected Treasure Functions Correctly
 * Tests player_has_collected_treasure Returns Correct Values
 * ================================================================= */
START_TEST(test_player_has_collected_treasure){

    Treasure *t = create_treasure(5);

    // Verify That The Player Has Not Collected The Treasure
    ck_assert_int_eq(player_has_collected_treasure(p, 5), false);

    // Collect The Treasure
    ck_assert_int_eq(player_try_collect(p, t), OK);

    // Verify That The Player Has Now Collected The Treasure
    ck_assert_int_eq(player_has_collected_treasure(p, 5), true);

    // Verify No Othere Treasure Has Been Collected
    ck_assert_int_eq(player_has_collected_treasure(p, 6), false);

    free(t);
}
END_TEST

/* ===========================================================================
 * Test 18: Player Has Collected NULL Values
 * Tests player_has_collected_treasure Handles NULL Values 
 * =========================================================================== */
START_TEST(test_player_has_collected_treasure_null_values){

    // Verify That player_has_collected_treasure Returns False On NULL Player
    ck_assert_int_eq(player_has_collected_treasure(NULL, 1), false);
    ck_assert_int_eq(player_has_collected_treasure(p, -1), false);
}
END_TEST

/* ===========================================================================
 * Test 19: Player Get Collected Count Functions Correctly
 * Tests player_get_collected_count Returns Correct Values 
 * =========================================================================== */
START_TEST(test_player_get_collected_count){

    Treasure *t1 = create_treasure(25);
    Treasure *t2 = create_treasure(50);

    // Verify That The Player Has No Collected Treasure
    ck_assert_int_eq(player_get_collected_count(p), 0);

    // Collect The First Treasure
    ck_assert_int_eq(player_try_collect(p, t1), OK);
    ck_assert_int_eq(player_get_collected_count(p), 1);

    // Collect The Second Treasure
    ck_assert_int_eq(player_try_collect(p, t2), OK);
    ck_assert_int_eq(player_get_collected_count(p), 2);

    free(t1);
    free(t2);
}
END_TEST

/* ===========================================================================
 * Test 20: Player Get Collected Count With NULL Player
 * Tests player_get_collected_count Successfully Detects A NULL Player
 * =========================================================================== */
START_TEST(test_player_get_collected_count_null_player){

    // Verify That player_get_collected_count Returns 0 On NULL Player
    ck_assert_int_eq(player_get_collected_count(NULL), 0);
}
END_TEST

/* =========================================================================================
 * Test 21: Player Get Collected Treasures Functions Correctly
 * Tests player_get_collected_treasures Successfully Returns The Array Of Collected Treasures
 * ========================================================================================== */
START_TEST(test_player_get_collected_treasures){

    Treasure *t1 = create_treasure(125);
    Treasure *t2 = create_treasure(150);

    // Collect Two Treasures
    ck_assert_int_eq(player_try_collect(p, t1), OK);
    ck_assert_int_eq(player_try_collect(p, t2), OK);

    // Get The Array Of Collected Treasures And Verify It Is Correct
    int count = -1;
    const Treasure * const *collected = player_get_collected_treasures(p, &count);
    ck_assert_ptr_nonnull(collected);
    ck_assert_int_eq(count, 2);
    ck_assert_ptr_eq(collected[0], t1);
    ck_assert_ptr_eq(collected[1], t2);

    free(t1);
    free(t2);
}
END_TEST

/* ===========================================================================
 * Test 22: Player Get Collected Treasures With NULL Values
 * Tests player_get_collected_treasures Successfully Detects NULL Values
 * =========================================================================== */
START_TEST(test_player_get_collected_treasures_null_values){

    // Verify That player_get_collected_treasures Returns NULL On NULL Player
    int count = -1;
    ck_assert_ptr_null(player_get_collected_treasures(NULL, &count));

    // Verify That player_get_collected_treasures Returns NULL On NULL Count
    Player *localPlayer = NULL;
    ck_assert_int_eq(player_create(0, 0, 0, &localPlayer), OK);
    ck_assert_ptr_null(player_get_collected_treasures(localPlayer, NULL));

    // Clean Up Local Player
    player_destroy(localPlayer);
}
END_TEST

/* ===============================================================
 * Test Suite Creation
 * =============================================================== */
Suite *player_test_suite(void){

    // Create A New Suite
    Suite *s = suite_create("Player");

    // Create A Test Case For Player Functions
    TCase *tc = tcase_create("PlayerTests");

    // Run Setup And Teardown Functions For Each Test
    tcase_add_checked_fixture(tc, setup_player, teardown_player);

    // Add Individual Test Function To The Test Case
    tcase_add_test(tc, test_player_create);
    tcase_add_test(tc, test_player_create_independent);
    tcase_add_test(tc, test_player_create_null_player_out);
    tcase_add_test(tc, test_player_get_room_id);
    tcase_add_test(tc, test_player_get_room_id_null_player);
    tcase_add_test(tc, test_player_get_position);
    tcase_add_test(tc, test_player_get_position_null_values);
    tcase_add_test(tc, test_player_set_position);
    tcase_add_test(tc, test_player_set_position_null_player);
    tcase_add_test(tc, test_player_move_to_room);
    tcase_add_test(tc, test_player_move_to_room_null_player);
    tcase_add_test(tc, test_player_reset_to_start);
    tcase_add_test(tc, test_player_reset_to_start_null_player);
    tcase_add_test(tc, test_player_try_collect_success);
    tcase_add_test(tc, test_player_try_collect_null_values);
    tcase_add_test(tc, test_player_try_collect_collected);
    tcase_add_test(tc, test_player_has_collected_treasure);
    tcase_add_test(tc, test_player_has_collected_treasure_null_values);
    tcase_add_test(tc, test_player_get_collected_count);
    tcase_add_test(tc, test_player_get_collected_count_null_player);
    tcase_add_test(tc, test_player_get_collected_treasures);
    tcase_add_test(tc, test_player_get_collected_treasures_null_values);

    // Add The Test Case To The Test Suite
    suite_add_tcase(s, tc);

    // Return The Test Suite To Main To Be Ran
    return s;
}
