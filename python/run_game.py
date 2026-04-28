"""
Load JSON With Player Profile To Tack Player Statistics
Used https://patorjk.com/software/taag/#p=display&f=Big+Money-se&t=Treasure+Runner%21&x=none&v=4&h=4&w=80&we=false To Generate ASCII Text Art
"""

import os
import argparse
import json
import curses 
from datetime import datetime, timezone
from pathlib import Path
from treasure_runner.ui.game_ui import game_loop

class PlayerProfile:

    # Initialize The Player Profile With Default Values For A New Player
    def __init__(self, player_name, games_played=0, max_treasure=0, most_rooms=0, last_played=None):
        self.player_name = player_name
        self.games_played = games_played
        self.max_treasure = max_treasure
        self.most_rooms = most_rooms
        self.last_played = datetime.now(timezone.utc).isoformat()

    # Return A JSON Safe Snapshot Of The Player Stats
    def to_dict(self):
        return {
            "player_name":                  self.player_name,
            "games_played":                 self.games_played,
            "max_treasure_collected":       self.max_treasure,
            "most_rooms_world_completed":   self.most_rooms,
            "timestamp_last_played":        self.last_played,          
        }
    
    # Create A Player Profile Object From A Dictionary Loaded From JSON
    @classmethod
    def from_dict(cls, data):
        return cls(
            player_name     = data["player_name"],
            games_played    = data["games_played"],
            max_treasure    = data["max_treasure_collected"],
            most_rooms      = data["most_rooms_world_completed"],
            last_played     = data.get("timestamp_last_played"),
        )
    
    # Update The Player Profile With The New Stats After Each Game
    def update_stats(self, treasure_collected: int, rooms_completed: int):
        self.games_played += 1
        if treasure_collected > self.max_treasure:
            self.max_treasure = treasure_collected
        if rooms_completed > self.most_rooms:
            self.most_rooms = rooms_completed
    
# Function To Save A Profile In The JSON
def save(path, profile):

    # Ensure The Directory Exists Before Saving
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    
    # Update The Last Played Timestamp Before Saving
    profile.last_played = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    
    # Save The Profile To The JSON File With Indentation For Readability
    with open(path, "w") as f:
        json.dump(profile.to_dict(), f, indent=2)
    print(f"Saved to {path}.")

# Function To Load The JSON File If It Already Exists And Create A New One If Not
def load(path):

    # Return None To Create A New File If It Does Not Already Exist
    if not Path(path).exists():
        return None

    # If The File Exists Return It 
    try:
        with open(path) as f:
            data = json.load(f)
        return PlayerProfile.from_dict(data)

    # Return None If An Error Occurs When Trying To Find The File (Just Create A New One)
    except json.JSONDecodeError:
        return None

# Function To Get The Players Name For The JSON Player Statistics
def player_name(stdscr) -> str:

    curses.curs_set(1) # Allow User To See Their Cursor
    curses.echo() # Show Typed Charcters
    stdscr.clear()

    # Verify The Terminal Is A Valid Size
    min_terminal_size(stdscr, 15, 153)

    # Prompt User To Enter Their Name
    stdscr.addstr(0, 0, "Welcome To...")

    stdscr.addstr(1, 0, " ________                                                                           _______                                                           __")
    stdscr.addstr(2, 0, "/        |                                                                         /       \                                                         /  |")
    stdscr.addstr(3, 0, "$$$$$$$$/______    ______    ______    _______  __    __   ______    ______        $$$$$$$  | __    __  _______   _______    ______    ______        $$ |")
    stdscr.addstr(4, 0, "   $$ | /      \  /      \  /      \  /       |/  |  /  | /      \  /      \       $$ |__$$ |/  |  /  |/       \ /       \  /      \  /      \       $$ |")
    stdscr.addstr(5, 0, "   $$ |/$$$$$$  |/$$$$$$  | $$$$$$  |/$$$$$$$/ $$ |  $$ |/$$$$$$  |/$$$$$$  |      $$    $$< $$ |  $$ |$$$$$$$  |$$$$$$$  |/$$$$$$  |/$$$$$$  |      $$ |")
    stdscr.addstr(6, 0, "   $$ |$$ |  $$/ $$    $$ | /    $$ |$$      \ $$ |  $$ |$$ |  $$/ $$    $$ |      $$$$$$$  |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$    $$ |$$ |  $$/       $$/")
    stdscr.addstr(7, 0, "   $$ |$$ |      $$$$$$$$/ /$$$$$$$ | $$$$$$  |$$ \__$$ |$$ |      $$$$$$$$/       $$ |  $$ |$$ \__$$ |$$ |  $$ |$$ |  $$ |$$$$$$$$/ $$ |             __")
    stdscr.addstr(8, 0, "   $$ |$$ |      $$       |$$    $$ |/     $$/ $$    $$/ $$ |      $$       |      $$ |  $$ |$$    $$/ $$ |  $$ |$$ |  $$ |$$       |$$ |            /  |")
    stdscr.addstr(9, 0, "   $$/ $$/        $$$$$$$/  $$$$$$$/ $$$$$$$/   $$$$$$/  $$/        $$$$$$$/       $$/   $$/  $$$$$$/  $$/   $$/ $$/   $$/  $$$$$$$/ $$/             $$/")

    stdscr.addstr(11, 0, "No profile found. Enter your player name: ")
    stdscr.addstr(12, 0, "╔══════════════════════════════════════════════════╗")
    stdscr.addstr(13, 0, "║                                                  ║")
    stdscr.addstr(14, 0, "╚══════════════════════════════════════════════════╝")
    stdscr.refresh()

    # Get The User Input Name
    name = stdscr.getstr(13, 2, 50).decode("utf-8").strip()
    curses.curs_set(0) # Hide Cursor
    curses.noecho() # Don't Show Typed Charcters

    # If No Name Is Input Set Player Name To "Player"
    return name if name else "Player"

# Function To Display The Players Stats
def player_profile(stdscr, profile: PlayerProfile, message: str, bellow: int):
    stdscr.addstr(bellow + 1, 0, message)
    stdscr.addstr(bellow + 2, 0, f"Player: {profile.player_name}")
    stdscr.addstr(bellow + 3, 0, f"Games Played: {profile.games_played}") 
    stdscr.addstr(bellow + 4, 0, f"Max Treasure Collected: {profile.max_treasure}")
    stdscr.addstr(bellow + 5, 0, f"Most Rooms World Completed: {profile.most_rooms}")
    stdscr.addstr(bellow + 6, 0, f"Timestamp Last Played: {profile.last_played}")
    stdscr.addstr(bellow + 7, 0, f"Press Any Key To Continue...")
    stdscr.refresh()
    stdscr.getch()

def parse_args():
    parser = argparse.ArgumentParser(description="Treasure Runner")
    parser.add_argument("--config",  required=True, help="Path to the .ini config file")
    parser.add_argument("--profile", required=True, help="Path to the player profile JSON file")
    return parser.parse_args()

def main(stdscr):

    args = parse_args()
    config_path = args.config
    profile_path = Path(args.profile)

    # Create A New Player Profile Object If It Does Not Exist Or Load The Existing One
    profile = load(args.profile)
    if profile is None:
        name = player_name(stdscr)
        profile = PlayerProfile(player_name=name)
        save(args.profile, profile)

    # Verify The Terminal Is A Valid Size
    min_terminal_size(stdscr, 20, 153)

    # Display The Player Profile Before Starting The Game
    curses.curs_set(0)
    stdscr.clear()
    stdscr.addstr(0, 0, "Welcome Back To...")
    stdscr.addstr(1, 0, " ________                                                                           _______                                                           __")
    stdscr.addstr(2, 0, "/        |                                                                         /       \                                                         /  |")
    stdscr.addstr(3, 0, "$$$$$$$$/______    ______    ______    _______  __    __   ______    ______        $$$$$$$  | __    __  _______   _______    ______    ______        $$ |")
    stdscr.addstr(4, 0, "   $$ | /      \  /      \  /      \  /       |/  |  /  | /      \  /      \       $$ |__$$ |/  |  /  |/       \ /       \  /      \  /      \       $$ |")
    stdscr.addstr(5, 0, "   $$ |/$$$$$$  |/$$$$$$  | $$$$$$  |/$$$$$$$/ $$ |  $$ |/$$$$$$  |/$$$$$$  |      $$    $$< $$ |  $$ |$$$$$$$  |$$$$$$$  |/$$$$$$  |/$$$$$$  |      $$ |")
    stdscr.addstr(6, 0, "   $$ |$$ |  $$/ $$    $$ | /    $$ |$$      \ $$ |  $$ |$$ |  $$/ $$    $$ |      $$$$$$$  |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$    $$ |$$ |  $$/       $$/")
    stdscr.addstr(7, 0, "   $$ |$$ |      $$$$$$$$/ /$$$$$$$ | $$$$$$  |$$ \__$$ |$$ |      $$$$$$$$/       $$ |  $$ |$$ \__$$ |$$ |  $$ |$$ |  $$ |$$$$$$$$/ $$ |             __")
    stdscr.addstr(8, 0, "   $$ |$$ |      $$       |$$    $$ |/     $$/ $$    $$/ $$ |      $$       |      $$ |  $$ |$$    $$/ $$ |  $$ |$$ |  $$ |$$       |$$ |            /  |")
    stdscr.addstr(9, 0, "   $$/ $$/        $$$$$$$/  $$$$$$$/ $$$$$$$/   $$$$$$/  $$/        $$$$$$$/       $$/   $$/  $$$$$$/  $$/   $$/ $$/   $$/  $$$$$$$/ $$/             $$/")
    bellow = 10

    player_profile(stdscr, profile, "=== Player Profile ===", bellow)

    # Start The Game Loop
    treasure_collected, rooms_completed, victory = game_loop(stdscr, args.config)

    # Update The Player Profile With The New Stats
    profile.update_stats(treasure_collected, rooms_completed)
    save(args.profile, profile)

    # Verify The Terminal Is A Valid Size
    min_terminal_size(stdscr, 15, 80)
    curses.curs_set(0)
    stdscr.clear()

    if victory:
        stdscr.addstr(0, 0, " __   __   __     ______     ______   ______     ______     __  __")
        stdscr.addstr(1, 0, "/\ \ / /  /\ \   /\  ___\   /\__  _\ /\  __ \   /\  == \   /\ \_\ \ ")
        stdscr.addstr(2, 0, "\ \ \ /   \ \ \  \ \ \____  \/_/\ \/ \ \ \/\ \  \ \  __<   \ \____ \ ")
        stdscr.addstr(3, 0, " \ \__|    \ \_\  \ \_____\    \ \_\  \ \_____\  \ \_\ \_\  \/\_____\ ")
        stdscr.addstr(4, 0, "  \/_/      \/_/   \/_____/     \/_/   \/_____/   \/_/ /_/   \/_____/")
        total = treasure_collected
        stdscr.addstr(5, 0, f"You collected all {total} treasures and completed the dungeon")
        bellow = 6

    else:
        # Display The Player Profile Before Ending The Game
        stdscr.addstr(0, 0, " ██████╗  █████╗ ███╗   ███╗███████╗         ██████╗ ██╗   ██╗███████╗██████╗ ")
        stdscr.addstr(1, 0, "██╔════╝ ██╔══██╗████╗ ████║██╔════╝        ██╔═══██╗██║   ██║██╔════╝██╔══██╗")
        stdscr.addstr(2, 0, "██║  ███╗███████║██╔████╔██║█████╗          ██║   ██║██║   ██║█████╗  ██████╔╝")
        stdscr.addstr(3, 0, "██║   ██║██╔══██║██║╚██╔╝██║██╔══╝          ██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗")
        stdscr.addstr(4, 0, "╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗        ╚██████╔╝ ╚████╔╝ ███████╗██║  ██║")
        stdscr.addstr(5, 0, " ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝         ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝")
        bellow = 6

    player_profile(stdscr, profile, "=== Final Stats ==" if victory else "=== Updated Profile ===", bellow)

# Function To Determine If The String Displays On The Given Terminal Size
def min_terminal_size(stdscr, min_height: int, min_width: int):

    while True:
        height, width = stdscr.getmaxyx()

        if height >= min_height and width >= min_width:
            return

        stdscr.clear()
        stdscr.addstr(0, 0, "TERMINAL TOO SMALL - RESIZE TO CONTINUE")
        stdscr.addstr(1, 0, "Resize your terminal and press any key...")
        stdscr.refresh()
        stdscr.getch()

#
if __name__ == "__main__":
    curses.wrapper(main)
