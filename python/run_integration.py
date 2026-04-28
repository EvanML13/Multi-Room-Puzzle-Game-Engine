#!/usr/bin/env python3
"""Deterministic system integration test runner for Treasure Runner."""

# below are the imports that are in the instructors solution.
# you may need different ones.  Use them if you wish.
import os
import argparse
from treasure_runner.bindings import Direction
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.models.exceptions import GameError, ImpassableError

# Helper Function To Write A Given Log On A Given Line In The File
def write_log(log, line):
    log.write(line + "\n")

# Helper Function To Return The Players State To Log
def player_state(engine):
    
    # Get The Player
    p = engine.player

    # Get The Players Values
    room = p.get_room()
    x, y = p.get_position()
    collected = p.get_collected_count()

    return f"room={room}|x={x}|y={y}|collected={collected}"

def first_move(engine):
    
    directions = [Direction.SOUTH, Direction.WEST, Direction.NORTH, Direction.EAST]
    
    # Get Start Values Before The Attempted Move
    start_room = engine.player.get_room()
    start_position = engine.player.get_position()

    # Try Directions In The Order SOUTH, WEST, NORTH, EAST
    for d in directions:

        try:

            # Attempt To Move The Player
            engine.move_player(d)

            # Get Values After The Attempted Move
            new_room = engine.player.get_room()
            new_position = engine.player.get_position()

            # Check If The Room ID Stays The Same And Player Position Changes
            if new_room == start_room and new_position != start_position:
                engine.reset()
                return d

        # Handel Any Exceptions That May Occur During The Move Attempt
        except(GameError, ImpassableError):
            continue

    # If No Valid Move Is Found, Raise An Exception
    raise RuntimeError("TERMINATED: Initial Move Error.")
            
def entry_move(engine, log, direction, phase_name, step_counter):

    # Log The Begining Of Sweep
    write_log(log, f"SWEEP_START|phase={phase_name}|dir={direction.name}")
    seen_states = set()
    moves_done = 0

    while True:
        
        # Get The Players State Before Moving 
        before = player_state(engine)

        # If State Has Already Been Seen Cycle Detected
        if before in seen_states:
            write_log(log, f"SWEEP_END|phase={phase_name}|reason=CYCLE_DETECTED|moves={moves_done}")
            return step_counter

        seen_states.add(before)
        step_counter += 1

        try:

            before_collected = engine.player.get_collected_count()

            # Move The Player And Get The After State
            engine.move_player(direction)
            after = player_state(engine)
            after_collected = engine.player.get_collected_count()
            delta = after_collected - before_collected

            write_log(log, f"MOVE|step={step_counter}|phase={phase_name}|dir={direction.name}"f"|result=OK|before={before}|after={after}|delta_collected={delta}")

            # Update Moves Done
            if delta > 0 or before != after:
                moves_done += 1 

            # Ends The Sweep If No Progress Is Made (No New States And No New Treasure Collected)
            if before == after and delta == 0:
                write_log(log, f"SWEEP_END|phase={phase_name}|reason=BLOCKED|moves={moves_done}")
                return step_counter

        # Handle Impassable Error
        except ImpassableError:

            write_log(log, f"MOVE|step={step_counter}|phase={phase_name}|dir={direction.name}"f"|result=BLOCKED|before={before}|after={before}|delta_collected=0")
            write_log(log, f"SWEEP_END|phase={phase_name}|reason=BLOCKED|moves={moves_done}")
            return step_counter

        # Handle Error
        except GameError:

            write_log(log, f"MOVE|step={step_counter}|phase={phase_name}|dir={direction.name}"f"|result=ERROR|before={before}|after={before}|delta_collected=0")
            write_log(log, f"SWEEP_END|phase={phase_name}|reason=BLOCKED|moves={moves_done}")
            return step_counter

def run_integration(config_path, log_path):

    with open(log_path, "w") as log:

        write_log(log, f"RUN_START|config={config_path}") 
        engine = GameEngine(config_path)
        step = 0

        # Spawn State
        write_log(log, f"STATE|step=0|phase=SPAWN|state={player_state(engine)}")

        # Get The Entry Move
        entry_direction = first_move(engine)
        write_log(log, f"ENTRY|direction={entry_direction.name}")

        # Execute Entry Move
        step = 1
        before = player_state(engine)

        try:
            # Get The Number Of Treasures Collected
            before_collected = engine.player.get_collected_count()

            engine.move_player(entry_direction)

            # Get The Players State And Number Of Treasures Collected After The Move
            after = player_state(engine)
            after_collected = engine.player.get_collected_count()

            # Calculate The Change In Collected Treasures
            delta = after_collected - before_collected

            write_log(log, f"MOVE|step=1|phase=ENTRY|dir={entry_direction.name}" f"|result=OK|before={before}|after={after}|delta_collected={delta}")

        except ImpassableError:
            write_log(log, f"MOVE|step=1|phase=ENTRY|dir={entry_direction.name}" f"|result=BLOCKED|before={before}|after={before}")
            write_log(log, "TERMINATED: Initial Move Error")
            write_log(log, "RUN_END|steps=1|collected_totals=0")
            return       

        except GameError:
            write_log(log, f"MOVE|step=1|phase=ENTRY|dir={entry_direction.name}" f"|result=ERROR|before={before}|after={after}") 
            write_log(log, "TERMINATED: Initial Move Error")
            write_log(log, "RUN_END|steps=1|collected_totals=0")
            return 
        
        # Sweep Phases
        sweeps = [("SWEEP_SOUTH", Direction.SOUTH), ("SWEEP_WEST", Direction.WEST), ("SWEEP_NORTH", Direction.NORTH), ("SWEEP_EAST", Direction.EAST)]

        # For Each Sweep Phase, Perform A Sweep In The Given Direction Until No Progress Is Made Or A Cycle Is Detected
        for phase_name, direction in sweeps:
            step = entry_move(engine, log, direction, phase_name, step)

        # Final State And Total Colected Treasure Count
        write_log(log, f"STATE|step={step}|phase=FINAL|state={player_state(engine)}")
        collected = engine.player.get_collected_count()

        write_log(log, f"RUN_END|steps={step}|collected_total={collected}")

def parse_args():
    parser = argparse.ArgumentParser(description="Treasure Runner integration test logger")
    parser.add_argument(
        "--config",
        required=True,
        help="Path to generator config file",
    )
    parser.add_argument(
        "--log",
        required=True,
        help="Output log path",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    config_path = os.path.abspath(args.config)
    log_path = os.path.abspath(args.log)
    run_integration(config_path, log_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
