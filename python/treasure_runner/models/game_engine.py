import ctypes
from typing import Tuple, List
from ..bindings import lib, Status, Direction
from .player import Player
from .exceptions import GameError, ImpassableError

class GameEngine:

    def __init__(self, config_path: str) -> None:

        eng_ptr = ctypes.c_void_p()

        # Calls lib.game_engine_create To Set self._eng
        status = lib.game_engine_create(config_path.encode("utf-8"), ctypes.byref(eng_ptr))

        # Raises Mapped Python Exception If lib.game_engine_create Non Ok
        if status != Status.OK:
            raise RuntimeError(Status(status).name)

        # Pointer To The C GameEngine Type
        self._eng = eng_ptr

        # Calls game_engine_get_player To Set self.__player
        player_ptr = lib.game_engine_get_player(self._eng)

        # Runtime Error If game_engine_get_player Fails To Create Player
        if not player_ptr:
            raise RuntimeError("Failed to get player")

        # Reference To The Python Player Class We Have Created
        # player -> Player
        self._player = Player(player_ptr)

    @property
    def player(self) -> Player:
        return self._player

    def destroy(self) -> None:
        if self._eng:
            # Call game_engine_destory
            lib.game_engine_destroy(self._eng)
            self._eng = None

    def move_player(self, direction: Direction) -> None:
        # Calls game_engine_move_player
        status = lib.game_engine_move_player(self._eng, direction)

        # Raises Mapped Exception If Not Status.OK
        if status != Status.OK:
            if status == Status.ROOM_IMPASSABLE:
                raise ImpassableError()
            raise GameError(Status(status).name)

    def render_current_room(self) -> str:
        out = ctypes.c_char_p()

        # Calls game_engine_render_current_room
        status = lib.game_engine_render_current_room(self._eng, ctypes.byref(out))
        # Raises mapped exception if not Status.OK
        if status != Status.OK:
            raise RuntimeError(Status(status).name)

        # Decodes returned C string as UTF-8
        result = out.value.decode("utf-8")

        # Frees C heap memory using game_engine_free_string
        lib.game_engine_free_string(out)

        return result

    def get_room_count(self) -> int:
        count = ctypes.c_int()

        # Calls game_engine_get_room_count
        status = lib.game_engine_get_room_count(self._eng, ctypes.byref(count))

        # Raises Mapped Exception If Not Status.OK
        if status != Status.OK:
            raise RuntimeError(Status(status).name)

        return count.value

    def get_room_dimensions(self) -> Tuple[int, int]:
        width = ctypes.c_int()
        height = ctypes.c_int()

        # Calls game_engine_get_room_dimensions
        lib.game_engine_get_room_dimensions(self._eng, ctypes.byref(width), ctypes.byref(height))

        return (width.value, height.value)

    def get_room_ids(self) -> list[int]:
        ids = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_int()

        # Calls game_engine_get_room_ids
        lib.game_engine_get_room_ids(self._eng, ctypes.byref(ids), ctypes.byref(count))

        # Copies IDs From C Array Into Python List
        ids_list = [ids[i] for i in range(count.value)]

        # Frees Returned C Array Via game_engine_free_string
        lib.game_engine_free_string(ids)

        return ids_list

    def get_adjacency_matrix(self):

        matrix_ptr = ctypes.POINTER(ctypes.c_int)()
        ids_ptr = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_int()

        # Calls game_engine_get_adjacency_matrix
        status = lib.game_engine_get_adjacency_matrix(self._eng, ctypes.byref(matrix_ptr), ctypes.byref(ids_ptr), ctypes.byref(count))

        # Raises Mapped Exception If Not Status.OK
        if status != Status.OK:
            raise RuntimeError(Status(status).name)

        # Get The Room Count, Room IDs, And Adjacency Matrix
        n = count.value
        room_ids = [ids_ptr[i] for i in range(n)]
        matrix = [[matrix_ptr[i * n + j] for j in range(n)] for i in range (n)]

        lib.game_engine_free_string(ctypes.cast(matrix_ptr, ctypes.c_void_p))
        lib.game_engine_free_string(ctypes.cast(ids_ptr, ctypes.c_void_p))

        return room_ids, matrix

    def get_total_treasures(self)  -> int:
        return lib.game_engine_get_total_treasures(self._eng)

    def get_room_uncollected_treasures(self, room_id: int) -> int:
        return lib.game_engine_get_room_uncollected_treasures(self._eng, room_id)

    def get_room_dimensions_by_id(self, room_id: int) -> Tuple[int, int]:
        width = ctypes.c_int()
        height = ctypes.c_int()

        # Calls game_engine_get_room_dimensions
        lib.game_engine_get_room_dimensions_by_id(self._eng, room_id, ctypes.byref(width), ctypes.byref(height))

        return (width.value, height.value)

    def reset(self) -> None:
        lib.game_engine_reset(self._eng)
