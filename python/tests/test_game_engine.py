import unittest
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.bindings import Direction

class TestGameEngine(unittest.TestCase):

    # Create The Game Engine Before Each Test
    def setUp(self):
        self.engine = GameEngine("../assets/starter.ini")

    # Destroy The Game Engine After Each Test
    def tearDown(self):
        self.engine.destroy()

    # Verify The Setup Finction Successfully Initializes The Game Engine
    def test_engine_intializes(self):
        self.assertIsNotNone(self.engine)

    # Verify The Player Is Provided With The Game Engine
    def test_player_exists(self):
        player = self.engine.player
        self.assertIsNotNone(player)

    # Verify A Positive Room Count Is Provided From The get_room_count Function
    def test_get_room_count(self):
        count = self.engine.get_room_count()
        self.assertIsInstance(count, int)
        self.assertGreater(count, 0)

    # Verify Room Dimentions Are Positive From get_room_dimensions
    def test_get_room_dimensions(self):
        width, height = self.engine.get_room_dimensions()
        self.assertIsInstance(width, int)
        self.assertIsInstance(height, int)
        self.assertGreater(width, 0)
        self.assertGreater(width, 0)

    # Verify render_current_room Returns A String Of Length Greater Than 0
    def test_render_current_room(self):
        rendered_room = self.engine.render_current_room()
        self.assertIsInstance(rendered_room, str)
        self.assertGreater(len(rendered_room), 0)

    # Verify The move_player Moves The Player Around The Room
    def test_move_player(self):

        # Try To Move NORTH
        try:
            self.engine.move_player(Direction.NORTH)


        # Handle Errors
        except Exception:
            pass

        # Verify Room Remains The Same
        room = self.engine.player.get_room()
        self.assertIsInstance(room, int)

        # Try To Move EAST
        try:
            self.engine.move_player(Direction.EAST)
        
        # Handel Errors
        except Exception:
            pass

        # Verify Room Remains The Same
        room = self.engine.player.get_room()
        self.assertIsInstance(room, int)

        # Try To Move SOUTH
        try:
            self.engine.move_player(Direction.SOUTH)
        
        # Handel Errors
        except Exception:
            pass

        # Verify Room Remains The Same
        room = self.engine.player.get_room()
        self.assertIsInstance(room, int)

        # Try To Move WEST
        try:
            self.engine.move_player(Direction.WEST)
        
        # Handle Error
        except Exception:
            pass

        # Verify Room Remains The Same
        room = self.engine.player.get_room()
        self.assertIsInstance(room, int)

    # Verify The reset Function Resets The Players Position
    def test_reset(self):
        self.engine.reset()
        room = self.engine.player.get_room()
        self.assertIsInstance(room, int)

if __name__ == "__main__":
    unittest.main()