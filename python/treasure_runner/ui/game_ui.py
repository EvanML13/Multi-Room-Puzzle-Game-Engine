"""Curses UI Rendering For Python Bindings"""

import curses
import os
from typing import Tuple
from ..bindings import Direction
from ..models.player import Player
from ..models.game_engine import GameEngine
from ..models.exceptions import GameError, ImpassableError

class GameUI:
    """
    Handles all curses-based rendering and player-facing messages

    Responsibilities:
        • Initialize curses environment
        • Render the game board and player state
        • Display transient messages and status information
    """

    # Color Pairs
    COLOR_DEFAULT = 1
    COLOR_WALL = 2
    COLOR_GOLD = 3
    COLOR_EXIT = 4
    COLOR_PLAYER = 5
    COLOR_CURRENT_ROOM = 6
    COLOR_VISITED_ROOM = 7
    COLOR_UNVISITED_ROOM = 8

    def __init__(self, stdscr) -> None:
        self._stdscr = stdscr

    # Function To Initialize The Screen
    def initialize_screen(self) -> None:
        curses.curs_set(0) # Hide Cursor
        curses.cbreak() # Don't Wait For Enter
        curses.noecho() # Don't Show Typed Charcters
        self._stdscr.keypad(True) # Enable Arrow Keys
        curses.start_color() # Enable Color Support
        curses.use_default_colors() # Use Terminal Defaults
        self._init_colors() # Initialize Colors
        self._stdscr.clear() # Clear Terminal

    @classmethod
    def _init_colors(cls) -> None:
        try:
            curses.init_pair(cls.COLOR_DEFAULT, curses.COLOR_WHITE, -1)
            curses.init_pair(cls.COLOR_WALL, curses.COLOR_RED, -1)
            curses.init_pair(cls.COLOR_GOLD, curses.COLOR_YELLOW, -1)
            curses.init_pair(cls.COLOR_EXIT, curses.COLOR_GREEN, -1)
            curses.init_pair(cls.COLOR_PLAYER, curses.COLOR_CYAN, -1)
            curses.init_pair(cls.COLOR_CURRENT_ROOM, curses.COLOR_YELLOW, -1)
            curses.init_pair(cls.COLOR_VISITED_ROOM, curses.COLOR_GREEN, -1)
            curses.init_pair(cls.COLOR_UNVISITED_ROOM, curses.COLOR_WHITE, -1)
        except curses.error:
            pass

    # Function To Render The Entire Game UI
    def render(self, engine, message, rooms_visited, visited_ids) -> bool:

        self._stdscr.clear()

        # Stop Rendering If The Message Bar Does Not Fit In The Terminal
        if not self._draw_message_bar(message):
            self._stdscr.refresh()
            self._stdscr.getch()
            return False

        # Stop Rendering If The Room Header Does Not Fit In The Terminal
        if not self._draw_room_header(engine):
            self._stdscr.refresh()
            return False

        # Stop Rendering If The Room Does Not Fit In The Terminal
        bellow_game = self._draw_room(engine)
        if not bellow_game:
            self._stdscr.refresh()
            return False

        # Stop Rendering If The Legend Does Not Fit In The Terminal
        if not self._draw_legend(engine):
            self._stdscr.refresh()
            return False

        # Stop Rendering If The Minimap Does Not Fit In The Terminal
        bellow_minimap = self._draw_minimap(engine, visited_ids)
        if bellow_minimap is False:
            self._stdscr.refresh()
            return False

        # Update Bellow Game With The Lowest Part Of The Map
        if bellow_game < bellow_minimap:
            bellow_game = bellow_minimap


        # Stop Rendering If The Game Instructions Do Not Fit In The Terminal
        if not self._draw_instructions(engine, bellow_game):
            self._stdscr.refresh()
            return False

        # Stop Rendering If The Player Stats Do Not Fit In The Terminal Window
        if not self._draw_player_stats(engine, rooms_visited, bellow_game):
            self._stdscr.refresh()
            return False

        # Stop Renderinf If The Title Bar Does Not Fit In The Terminal
        if not self._draw_title_bar(engine, bellow_game):
            self._stdscr.refresh()
            return False

        # Render The Entire Game
        self._stdscr.refresh()
        return True

    # Function To Draw The Message Bar On Line 0
    def _draw_message_bar(self, message) -> bool:

        height, width = self._stdscr.getmaxyx()

        # Clear The Message Bar
        self._stdscr.addstr(0, 0, " ")

        # If The Terminal Window Is Not Wide Enough Add ... To The Message
        if len(message) > width:
            message = message[:width - 3] + "..."
            return True

        # Print Error If The Terminal Is Not Tall Enough
        if height <= 1:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # Draw Message Bar On Row 0
        self._stdscr.addstr(0, 0, message)
        return True

    # Function To Draw The Room Header On Line 1
    def _draw_room_header(self, engine) -> bool:

        # Get The Room ID And Room Name To Be Printed In The Header
        room_id = engine.player.get_room()
        header = f"Room {room_id}"

        height, width = self._stdscr.getmaxyx()

        # If The Terminal Window Is Not Wide Enough Add ... To The Message
        if len(header) > width:
            header = header[:width - 3] + "..."
            return True

        # Print Error If The Terminal Is Not Tall Enough
        if height <= 2:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # Draw The Room Header On Row 1
        self._stdscr.addstr(1, 0, header)
        return True

    # Function To Draw The Entire Room The Player Is Currently In
    def _draw_room(self, engine) -> int:

        # Get The Rendered Room
        rendered = engine.render_current_room()
        lines = rendered.splitlines()

        height, width = self._stdscr.getmaxyx()

        # Print Error If The Terminal Is Not Tall Enough
        if len(lines) + 3 > height:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # Draw The Rendered Room Line By Line
        for i, line in enumerate(lines):

            # Print Error If The Terminal Is Not Wide Enough
            if len(line) > width:
                self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
                return False

            # Get The Color For Each Tile And Add It To The Terminal
            for j, tile in enumerate(line):
                color = self._get_color_for_tile(tile)
                self._stdscr.addstr(3 + i, j, tile, curses.color_pair(color))

        return len(lines)

    # Function To Draw The Game Legend To The Right Of The Room
    def _draw_legend(self, engine) -> bool:

        legend = ["Game Elements:", " ", "@ - player", "# - wall", "$ - gold", "X - portal"] # Change And Add Additional Elements
        dimentions = engine.get_room_dimensions()
        width = dimentions[0]

        for i, line in enumerate(legend):

            height, term_width = self._stdscr.getmaxyx()

            # If The Terminal Window Is Not Wide Enough Add ... To The Message
            if width + 5 + len(line) > term_width:
                line = line[:term_width - width - 8] + "..."
                self._stdscr.addstr(3 + i, width + 5, line)
                continue

            # Print Error If The Terminal Is Not Tall Enough
            if height <= 3 + i:
                self._stdscr.addstr(3 + i, width + 5, "Terminal Window Too Small!")
                return False

            self._stdscr.addstr(3 + i, width + 5, line)
        return True

    # Function To Draw The Game Instructions Bellow The Game
    def _draw_instructions(self, engine, bellow_game: int) -> bool:

        controls = f"Game Controls: Move: Arrow Keys or a/w/s/d  |  Enter Portal: >  |  Quit: q  |  Reset: r"
        height, width = self._stdscr.getmaxyx()

        # Print Error If The Terminal Window Is Not Tall Enough
        if height <= bellow_game + 4:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # If The Terminal Window Is Not Wide Enough Add ... To The Message
        if len(controls) > width:
            controls = controls[:width - 3] + "..."
            self._stdscr.addstr(bellow_game + 4, 0, controls)
            return True

        # Draw The Movement Instructions Bellow The Game
        self._stdscr.addstr(bellow_game + 4, 0, controls)
        return True

    # Function To Draw The Players Statistics
    def _draw_player_stats(self, engine, rooms_visited: int, bellow_game: int) -> bool:

        collected = engine.player.get_collected_count()
        total_rooms = engine.get_room_count()
        rooms_left = total_rooms - rooms_visited
        stats = f"Gold Collected: {collected}  |  Rooms Visited: {rooms_visited}  |  Rooms Remaining: {rooms_left}" # Implement Logic To Calculate Rooms Visited And Rooms Left

        height, width = self._stdscr.getmaxyx()

        # Print Error If The Terminal Window Is Not Tall Enough
        if height <= bellow_game + 6:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # If The Terminal Window Is Not Wide Enough Add ... To The Message
        if len(stats) > width:
            stats = stats[:width - 3] + "..."
            self._stdscr.addstr(bellow_game + 6, 0, stats)
            return True

        self._stdscr.addstr(bellow_game + 6, 0, stats)
        return True

    # Function To Draw The Title Bar At The Bottom Of The Gamm
    def _draw_title_bar(self, engine, bellow_game: int) -> bool:

        title = f"Treasure Runner                      eleona04@uoguelph.ca"
        height, width = self._stdscr.getmaxyx()

        # Print Error If The Terminal Window Is Not Tall Enough
        if height <= bellow_game + 7:
            self._stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
            return False

        # If The Terminal Window Is Not Wide Enough Add ... To The Message
        if len(title) > width:
            title = title[:width - 3] + "..."
            self._stdscr.addstr(bellow_game + 7, 0, title)
            return True

        # Draw The Game Title And Email
        self._stdscr.addstr(bellow_game + 7, 0, title)
        return True

    # Funtion To Return The Color Pair For A Given Tile
    def _get_color_for_tile(self, tile: str) -> int:
        if tile == '#':
            return self.COLOR_WALL
        elif tile == '$':
            return self.COLOR_GOLD
        elif tile == 'X':
            return self.COLOR_EXIT
        elif tile == '@':
            return self.COLOR_PLAYER
        else:
            return self.COLOR_DEFAULT

    # Function To Draw The Mini Map To The Right Of The Legend
    def _draw_minimap(self, engine, visited_ids: set) -> bool:

        # Get The List Of Room IDs And Adjacency Matrix From The Function
        try:
            room_ids, matrix = engine.get_adjacency_matrix()
        except RuntimeError:
            return True

        # Verify The Room IDs Are Valid
        if not room_ids:
            return True

        # Get The Number Of Rooms In The Game Engine
        n = len(room_ids)

        # Get The Room The Player Is Currently In
        current_room = engine.player.get_room()

        # Get The Current Dimentions Of The Terminal
        screen_height, screen_width = self._stdscr.getmaxyx()

        # Prevent The Minimap From Being Larger Than The Game Board
        base_width, base_height = engine.get_room_dimensions()

        # Start The Minimap To The Left Of The Legend
        map_col = base_width + 18 # Width Is The Width Of The Room And +18 Moves The Minimap Past The Legend
        map_row = 3 # Height Is Just Beyond The Message Bar And Room Bar (Save Height As Game)

        # Value To Scale Down The Rooms Actual Height And Width
        scale = 0.25

        # Spacing Between Rooms In The Minimap
        gap_x = 5
        gap_y = 2

        # Number Of Columns To Display Rooms
        cols = 2

        pos = {}
        bellow_minimap = 0

        # Iteratte Through Each Room
        for i, room_id in enumerate(room_ids):

            # Get The Given Rooms Width And Height And Scale It Down To Display On The Mini Map
            room_width, room_height = engine.get_room_dimensions_by_id(room_id)
            box_width = max(8, min(15, int(room_width * scale)))
            box_height = max(4, min(8, int(room_height * scale)))
            # Full Width And Height Of The Room To Include Corner Pieces
            full_width = box_width + 2
            full_height = box_height + 2

            # Spacing Between Rooms In The Minimap
            next_x = 14
            next_y = 6

            # Exit If The Mini Room Wont Fit
            map_rows_needed = map_row + 2 + ((n - 1) // cols + 1) * next_y
            map_cols_needed = map_col + cols * next_x
            if map_cols_needed >= screen_width or map_rows_needed > screen_height:
                return True

            # Draw The World Map Header
            try:
                self._stdscr.addstr(map_row, map_col, " World Map:")
            except curses.error:
                return True

            # Determine Which Row And Column The Rooms Mini Representation Will Be In The Mini Map (Columns = 2) ~ Set Above
            grid_row = i // cols
            grid_col = i % cols

            pos[i] = (map_row + 2 + grid_row * next_y, map_col + grid_col * next_x)

            row, column = pos[i]

            # Skip If The Room Would Go Off Screen
            if row + full_height >= screen_height or column + full_width >= screen_width:
                continue

            # Color The Room On The Minimap Yellow If The Player Is In It
            if room_id == current_room:
                color = curses.color_pair(self.COLOR_CURRENT_ROOM)
            # Color The Room On The Minimap Green If The Player Has Visited It
            elif room_id in visited_ids:
                color = curses.color_pair(self.COLOR_VISITED_ROOM)
            # Color The Room On The Minimap White If It Is Unvisited
            else:
                color = curses.color_pair(self.COLOR_UNVISITED_ROOM)

            # Get The Amount Of Uncollected Treasure In The Room
            try:
                uncollected = engine.get_room_uncollected_treasures(room_id)
            except Exception:
                uncollected = -1

            # Create The Mini Room
            top = f"╔" + "═" * box_width + "╗"
            bottom = f"╚" + "═" * box_width + "╝"
            inner = f"║" + " " * box_width + "║"

            raw_label = f"R:{room_id} T:{uncollected}"
            raw_label = raw_label[:box_width]

            pad_total = box_width - len(raw_label)
            pad_left = pad_total // 2
            pad_right = pad_total - pad_left

            label = "║" + " " * pad_left + raw_label + " " * pad_right + "║"

            # Add The Player Marker In The Box Just Above The Label
            middle_column = box_width // 2
            player_label = "║" + " " * middle_column + "@" + " " * (box_width - middle_column - 1) + "║"

            # Add The Lable In The Middle Of The Box
            label_inner = box_height // 2
            player_inner = label_inner - 1

            # Draw The Top Bar
            self._stdscr.addstr(row, column, top, color)

            # Draw The Middle Lines Of The Box
            for j in range(box_height):

                line_row = row + 1 + j

                # Draw The Room ID And Treasure Collected In The Middle Of The Box
                if j == label_inner:
                    self._stdscr.addstr(line_row, column, label, color)

                # Draw The Player Icon Just Above The Label If The Player Is In The Room
                elif room_id == current_room and j == player_inner:
                    self._stdscr.addstr(line_row, column, player_label, color)

                # Leave All Other Middle Line Blank
                else:
                    self._stdscr.addstr(line_row, column, inner, color)

            # Draw The Bottom Bar
            self._stdscr.addstr(row + full_height - 1, column, bottom, color)

            # Get The Bottom Of The Mini Map
            if row + full_height > bellow_minimap:
                bellow_minimap = row + full_height

            # Iterate Through Each Room
            for i in range(n):

                if i not in pos:
                    continue

                # Gets Where The Mini Room Is On The Mini Map
                src_row, src_col = pos[i]
                src_grid_row = i // cols
                src_grid_col = i % cols

                # Recalculate The Boxes Size
                src_width, src_height = engine.get_room_dimensions_by_id(room_ids[i])
                src_box_width = max(8, min(15, int(src_width * scale)))
                src_box_height = max(4, min(8, int(src_height * scale)))
                src_full_width = src_box_width + 2
                src_full_height = src_box_height + 2

                # Iterate Through Each Room
                for j in range(n):

                    # Verify The Connection Is In The Matrix
                    if not matrix[i][j] or j not in pos:
                        continue

                    # Gets Where The Destination Mini Room Is On The Mini Map
                    dst_row, dst_col = pos[j]
                    dst_grid_row = j // cols
                    dst_grid_col = j % cols

                    # Recalculate The Destination Boxes Size
                    dst_width, dst_height = engine.get_room_dimensions_by_id(room_ids[j])
                    dst_box_width = max(8, min(15, int(dst_width * scale)))
                    dst_box_height = max(4, min(8, int(dst_height * scale)))
                    dst_full_width = dst_box_width + 2
                    dst_full_height = dst_box_height + 2

                    # Arrow To Connect Room With Room To The Right
                    if src_grid_row == dst_grid_row and dst_grid_col == src_grid_col + 1:
                        arrow_row = src_row + src_full_height // 2
                        arrow_col = src_col + src_full_width
                        arrow = "-" * (gap_x - 1) + "▶"
                        # Draw The Rightward Arrow
                        if arrow_row < screen_height and arrow_col + len(arrow) < screen_width:
                            self._stdscr.addstr(arrow_row, arrow_col, arrow)

                    # Arrow To Connect Room With Rooms Bellow
                    elif src_grid_col == dst_grid_col and dst_grid_row == src_grid_row + 1:
                        arrow_col = src_col + src_full_width // 2
                        arrow_row = src_row + src_full_height
                        # Draw The Downward Arrow Shaft
                        for gap_row in range(gap_y - 1):
                            tr  = arrow_row + gap_row
                            if tr < screen_height and arrow_col < screen_width:
                                self._stdscr.addstr(tr, arrow_col, "|")
                        # Draw The Arrow Point On The Final Row
                        final_row = arrow_row + gap_y -1
                        if final_row < screen_height and arrow_col < screen_width:
                            self._stdscr.addstr(final_row, arrow_col, "▼")

                    # For Rooms That Are Not Bellow Or To The Right
                    else:
                        mini_arrow = f"→R{room_ids[j]}"
                        mini_arrow_row = src_row + 1 + (j % max(1, src_box_height -1))
                        mini_arrow_col = src_col + src_full_width
                        # Draw The Mini Arrow
                        if mini_arrow_row < src_row + src_full_height - 1 and mini_arrow_row < screen_height and mini_arrow_col + len(mini_arrow) < screen_width:
                            self._stdscr.addstr(mini_arrow_row, mini_arrow_col, mini_arrow)

        return bellow_minimap if bellow_minimap > 0 else None

def game_loop(stdscr, config_path: str) -> Tuple[int, int]:

    victory = False

    # Create An Instance Of GameUI Class
    ui = GameUI(stdscr)

    # Initialize Scrren
    ui.initialize_screen()

    mapping = {curses.KEY_UP: Direction.NORTH, curses.KEY_RIGHT: Direction.EAST, curses.KEY_DOWN: Direction.SOUTH, curses.KEY_LEFT: Direction.WEST, ord('w'): Direction.NORTH, ord('d'): Direction.EAST, ord('s'): Direction.SOUTH, ord('a'): Direction.WEST,}

    # Create The Game Engine
    engine = GameEngine(config_path)

    # For The First Room And Move
    message = f"Welcome To Treasure Runner! Collect Gold & Vist Rooms Via Portals!"
    rooms_visited = 1
    visited_ids ={engine.player.get_room()}

    try:
        while True:

            # Do Now Allow Move Untill The Terminal Is Large Enough
            while not ui.render(engine, message, rooms_visited, visited_ids):
                curses.napms(200)
                stdscr.clear()
                stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
                stdscr.refresh()

            key = stdscr.getch()

            # If The User Attempts To Quit
            if key == ord('q'):
                message = "Thanks For Playing!"
                ui.render(engine, message, rooms_visited, visited_ids)
                break

            # If The User Attempts To Reset
            if key == ord('r'):
                engine.reset()
                message = "Game Reset."
                visited_ids = {engine.player.get_room()}
                rooms_visited = 1
                continue

            # If User Attempts To Move
            if key in mapping:
                direction = mapping[key]
                direction_name = direction.name

                try:
                    # Try To Move The Player
                    before_collected = engine.player.get_collected_count()
                    engine.move_player(direction)
                    after_collected = engine.player.get_collected_count()
                    current_room = engine.player.get_room()
                    total = engine.get_total_treasures()

                    # Update Visited Set
                    if current_room not in visited_ids:
                        visited_ids.add(current_room)
                        rooms_visited += 1

                    # Display Message For Collecting Gold
                    if after_collected > before_collected:
                        delta = after_collected - before_collected
                        message = f"Collected {delta} Gold! Total: {after_collected}"

                        if after_collected >= total:
                            victory = True
                            return after_collected, rooms_visited, victory

                    # Update The Current Room If It Has Not Already Been Entered
                    elif current_room not in visited_ids:
                        total = engine.get_total_treasures()
                        message = f"Entering New Room {current_room}! {after_collected}/{total} treasures collected"

                    # Implement > For Entering Portals

                    # Otherwise Move The Player
                    else:
                        x, y = engine.player.get_position()
                        message = f"Moved {direction_name}: X={x}, Y={y}"

                # Handle Exceptions
                except ImpassableError:
                    message = f"Can't Move {direction_name}: Block By A Wall Or Obstacle!"
                except GameError:
                    message = f"Game Engine Error Moving {direction_name}"

            else:
                message = 'Invalid Input. See Game Controls!'

    finally:
        treasure_collected = engine.player.get_collected_count()
        engine.destroy()

    return treasure_collected, rooms_visited, victory
