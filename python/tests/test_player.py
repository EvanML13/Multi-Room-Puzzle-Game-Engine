import unittest
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.bindings import Direction

class TestPlayer(unittest.TestCase):

    # Create A Game Engine And Player Before Each Test
    def setUp(self):
        self.engine = GameEngine("../assets/starter.ini")
        self.player = self.engine.player

    # Destroy The Game Engine With The Player After Each Test
    def tearDown(self):
        self.engine.destroy()

    # Verify player_get_room Returns A Positive Integer Room ID
    def test_get_room(self):
        room = self.player.get_room()
        self.assertIsInstance(room, int)

    # Verify player_get_position Returns The Player Position
    def test_get_position(self):
        x, y = self.player.get_position()
        self.assertIsInstance(x, int)
        self.assertIsInstance(y, int)

    # Verify player_get_collected_count Returns A Positive Number Of Collected Treasures
    def test_get_collected_count(self):
        count = self.player.get_collected_count()
        self.assertIsInstance(count, int)
        self.assertGreaterEqual(count, 0)

    # Verify player_has_collected_treasure Return A Boolean Of Either True Or False
    def test_has_collected_treasure(self):
        result = self.player.has_collected_treasure(0)
        self.assertIsInstance(result, bool)
    
    # Verify player_get_collected_treasures Returns A List Of Collected Treasures
    def test_get_collected_treasures(self):
        treasures = self.player.get_collected_treasures()
        self.assertIsInstance(treasures, list)

        # Verify Treasure Struct Contains Info
        for t in treasures:
            self.assertIn("id", t)
            self.assertIn("name", t)
            self.assertIn("x", t)
            self.assertIn("y", t)
            self.assertIn("collected", t)

if __name__ == "__main__":
    unittest.main()