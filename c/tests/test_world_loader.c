#include <check.h>
#include <stdlib.h>

#include "world_loader.h"
#include "graph.h"
#include "room.h"
#include "types.h"

/* =====================================================================================
 * Test 1: Load A Valid World File
 * Tests loader_load_world Successfully Loads A Valid World File
 * ===================================================================================== */
START_TEST(test_loader_load_world_valid){

    Graph *graph = NULL;
    Room *first = NULL;
    int count = 0;
    Charset cs;

    // Run The World Loader
    Status s = loader_load_world("../assets/starter.ini", &graph, &first, &count, &cs);

    // Verify The World Loader Correctly Initializes The Variables
    ck_assert_int_eq(s, OK);
    ck_assert_ptr_nonnull(graph);
    ck_assert_ptr_nonnull(first);
    ck_assert_int_gt(count, 0);

    graph_destroy(graph);
}
END_TEST

/* ====================================================================================
 * Test 2: Load World With NULL Values
 * Tests loader_load_world Successfully Detects NULL Values
 * ==================================================================================== */
START_TEST(test_loader_load_world_invalid_args){
    // Verify World Loader Handles NULL Values
    ck_assert_int_eq(loader_load_world(NULL, NULL, NULL, NULL, NULL), INVALID_ARGUMENT);
}
END_TEST

/* =================================================================================
 * Test 3: Load World With Bad Config File
 * Tests loader_load_world Successfully Detects A Bad Config File
 * ================================================================================= */
START_TEST(test_loader_load_world_bad_config){

    Graph *graph = NULL;
    Room *first = NULL;
    int count = 0;
    Charset cs;

    // Run The World Loader
    Status s = loader_load_world("nonexistent.ini", &graph, &first, &count, &cs);

    // Verify The World Loader Handles A Non Existing Config File
    ck_assert_int_eq(s, WL_ERR_CONFIG);
}
END_TEST

/* ===============================================================
 * Test Suite Creation
 * =============================================================== */
Suite *world_loader_test_suite(void){

    // Create A New Suite
    Suite *s = suite_create("WorldLoader");

    // Create A Test Case For Game Engine Functions
    TCase *tc = tcase_create("WorldLoaderTests");

    // Add Individual Test Function To The Test Case
    tcase_add_test(tc, test_loader_load_world_valid);
    tcase_add_test(tc, test_loader_load_world_invalid_args);
    tcase_add_test(tc, test_loader_load_world_bad_config);

    // Add The Test Case To The Test Suite
    suite_add_tcase(s, tc);

    // Return The Test Suite To Main To Be Ran
    return s;
}
