#ifndef GAME_ENGINE_HELPER_H
#define GAME_ENGINE_HELPER_H

#include "types.h"   /* Direction, Status */

typedef struct Graph Graph;
typedef struct Player Player;

// ***************************************************************************                                
// ********************** HELPER FUNCTIONS ADDED FOR A3 **********************
// ***************************************************************************

Status game_engine_get_adjacency_matrix(const GameEngine *eng, int **matrix_out, int **ids_out, int *count_out);

int game_engine_get_total_treasures(const GameEngine *eng);

int game_engine_get_room_uncollected_treasures(const GameEngine *eng, int room_id);

Status game_engine_get_room_dimensions_by_id(const GameEngine *eng, int room_id, int *width_out, int *height_out);

#endif /* GAME_ENGINE_H */
