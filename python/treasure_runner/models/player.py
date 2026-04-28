import ctypes
from typing import Tuple, List, Dict
from ..bindings import lib, Treasure

class Player:

    # Initializes self._ptr To ptr
    def __init__(self, ptr) -> None:
        # Instance Variable
        self._ptr = ptr

    # Calls player_get_room
    def get_room(self) -> int:
        return lib.player_get_room(self._ptr)

    # Calls player_get_position And Returns (x,y)
    def get_position(self) -> Tuple[int, int]:
        x = ctypes.c_int()
        y = ctypes.c_int()
        lib.player_get_position(self._ptr, ctypes.byref(x), ctypes.byref(y))
        return (x.value, y.value)

    # Calls player_get_collected_count
    def get_collected_count(self) -> int:
        return lib.player_get_collected_count(self._ptr)

    # Calls has_collected_treasure
    def has_collected_treasure(self, treasure_id: int) -> bool:
        return bool(lib.player_has_collected_treasure(self._ptr, treasure_id))

    # Calls player_get_collected_treasures And Converts Each Returned Treasure Into Dict With id, name, starting_room_id, initial_x, intial_y, x, y, collected
    def get_collected_treasures(self) -> List[Dict]:

        count = ctypes.c_int()

        lib.player_get_collected_treasures(self._ptr, ctypes.byref(count))

        # Get The Array Of Collected Treasures
        treasure_array = lib.player_get_collected_treasures(self._ptr, ctypes.byref(count))

        treasures: List[Dict] = []

        for i in range(count.value):

            # Iterate Throught Each Collected Treasure And Append Its Infromation Into The treasures Array
            treasure = treasure_array[i]
            treasures.append({
                "id": treasure.id,
                "name": treasure.name.decode("utf-8"),
                "starting_room_id": treasure.starting_room_id,
                "initial_x": treasure.initial_x,
                "initial_y": treasure.initial_y,
                "x": treasure.x,
                "y": treasure.y,
                "collected": bool(treasure.collected),
            })

        return treasures
