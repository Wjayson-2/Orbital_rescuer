# Orbital Rescue — C++ GUI Simulation Starter

A partly completed 2D spacecraft rendezvous and docking simulator.

You pilot the damaged rescue craft **Kestrel** toward the **ARES Relay**
station. Fuel, approach speed, angle, hull integrity, and mission time all
matter. The starter already contains a playable graphical simulation so that
you are extending a real program rather than beginning from an empty file.

## Controls

- `Enter`: start mission
- `W` or `Up`: main engine
- `A` / `D`: lateral thrusters
- `Left` / `Right`: rotate
- `P`: pause
- `R`: reset

## Build in CLion

1. Open this folder as a CMake project.
2. Allow CMake to download and build raylib.
3. Select the `orbital_rescue` target.
4. Run.

The first CMake configuration requires internet access because raylib is
downloaded through `FetchContent`.

## What is already implemented

- Raylib GUI window and render loop
- Keyboard flight controls
- Spacecraft translational and rotational motion
- Fuel consumption
- Station and docking capture zone
- Speed and alignment checks
- Hull damage and mission failure
- Telemetry panel
- Mission timer and scoring
- Modular C++ source files

## Your required tasks

Complete these in order. Each task makes the program noticeably more advanced.

### Stage 1 — Core C++ correctness

1. Add input validation to a new `MissionConfig` loader.
2. Read starting position, velocity, and fuel from `missions/mission_01.txt`.
3. Replace duplicated numeric docking limits with a `DockingLimits` struct.
4. Store the last 300 spacecraft positions in `std::vector<Vec2>`.
5. Draw a fading flight-path trail.

Skills tested:
classes, structs, headers/source files, vectors, loops, file I/O, exceptions,
const-correctness, references.

### Stage 2 — Simulation engineering

6. Implement collision response properly:
   - calculate the docking-port surface normal;
   - reflect only the velocity component pointing into the station;
   - apply a restitution coefficient;
   - prevent repeated collision damage every frame.

7. Add gravity from a nearby moon:

   `a = mu * r / |r|^3`

   Include a minimum-radius guard against division by zero.

8. Add selectable time acceleration: `1x`, `2x`, `5x`.

Skills tested:
vector mathematics, numerical integration, state management, edge cases.

### Stage 3 — GUI and software design

9. Create clickable GUI buttons for:
   - Start
   - Pause
   - Restart
   - Autopilot

10. Add a mission-selection screen with at least three missions.
11. Add a results screen showing:
    - fuel used;
    - elapsed time;
    - maximum speed;
    - docking impact speed;
    - final score.

Skills tested:
GUI event handling, enums, program states, separation of model and view.

### Stage 4 — Interesting extension: autopilot

Implement a docking autopilot using three modes:

```text
APPROACH -> BRAKE -> ALIGN -> FINAL_DOCK
```

Recommended control logic:

- Approach: point toward a target 100 m before the port.
- Brake: reduce relative speed.
- Align: use a proportional controller for angular error.
- Final dock: approach slowly below the safe speed.

A simple controller may use:

```cpp
output = kp * error - kd * rate;
```

Do not directly set position or velocity. The autopilot must operate through
the same thruster commands used by the human pilot.

Skills tested:
finite-state machines, control systems, debugging, interfaces, decomposition.

## Optional fault scenarios

Add one randomly selected fault after 15 seconds:

- Main engine limited to 55%
- Left side thruster unavailable
- Fuel leak
- Gyroscope noise
- Telemetry delay
- Rotational damping failure

The fault should change the simulation, not merely display a message.

## Assessment rubric

| Area | Marks |
|---|---:|
| Correct C++ and clean compilation | 20 |
| Classes and program structure | 15 |
| Containers and file I/O | 15 |
| Physics and numerical stability | 15 |
| GUI and state management | 15 |
| Autopilot / finite-state machine | 15 |
| Testing and explanation | 5 |

Suggested interpretation:

- **Below 50**: basic syntax is present, but multi-file design and debugging need work.
- **50–65**: ready for ordinary first-year C++ exercises.
- **65–80**: strong summer preparation; can construct a medium-sized program.
- **Above 80**: comfortable with first-year project-style programming.

## Important debugging questions

While working, be able to explain:

1. Why should `dt` be capped?
2. Why is force divided by total mass?
3. Why can collision code run repeatedly across consecutive frames?
4. Why should rendering code not directly modify the spacecraft state?
5. Why is an autopilot state machine easier to debug than one enormous `if`?
6. What happens if `|r|` is almost zero in the gravity equation?

## Suggested Git milestones

1. `starter runs`
2. `mission config loader`
3. `flight trail`
4. `collision response`
5. `gravity and time warp`
6. `gui buttons`
7. `autopilot approach`
8. `autopilot docking`
9. `mission results and cleanup`
