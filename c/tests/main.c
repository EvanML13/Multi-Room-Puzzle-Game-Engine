#include <check.h>
#include <stdlib.h>

Suite *player_test_suite(void);
Suite *room_test_suite(void);
Suite *game_engine_test_suite(void);
Suite *world_loader_test_suite(void);

int main(void)
{
    Suite *suites[] = {
        player_test_suite(),
        room_test_suite(),
        game_engine_test_suite(),
        world_loader_test_suite(),
        //more suites
        NULL
    };

    SRunner *runner = srunner_create(suites[0]);
    for (int i = 1; suites[i] != NULL; ++i) {
        srunner_add_suite(runner, suites[i]);
    }

    srunner_run_all(runner, CK_NORMAL);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
