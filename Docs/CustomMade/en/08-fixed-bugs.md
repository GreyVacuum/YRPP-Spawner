# Fixed Bugs

This page lists the engine defects the spawner fixes. Most fixes are always
active when the spawner is enabled and have no INI switch.

## Crash fixes

### Exception filter

A top-level exception filter repairs a small set of known crash signatures
instead of terminating the game:

| Fault address | Cause | Recovery |
|---|---|---|
| `0x5D6C21` | Map has no waypoint 90 | Continue at `0x5D6C36` |
| `0x7BAEA1` | `DSurface::GetPixel` | Continue at `0x7BAEA8`, zero EBX |
| `0x7BC806` | `BlitPlain` path | Continue at `0x7BC80F` |
| `0x535DBC` | Keyboard command class | Continue at `0x535DCE`, adjust ESP |
| `0x000000` after `0x55E018` | Yuri Prime mind-control hotkey crash | Continue at `0x55E018`, adjust ESP |

Any other exception is handled normally.

### Individual crash fixes

- `ObjectClass::Mark` crash.
- `TriggerTypeClass::ComputeCRC` with a null house type.
- `TechnoClass::GetMyOwner` with a null capture manager.
- `TechnoClass::Revealed` with a null house.
- `TechnoClass` state update with a null tech object.
- Reinforcement placement with a null house.

## Desync fixes

### Stale cell attributes

`CellClass::RecalcAttributes` derives land type and passability from the cell
tile and overlay. Several code paths remove overlays without recalculating, so
the land type stays stale and pathfinding diverges between players. The
spawner recalculates after:

- Crate removal, in both single-player and multiplayer paths.
- Flag removal.
- Veinhole construction.
- Bridge destruction and bridge damage.

### Aircraft shadow

Aircraft shadow drawing temporarily sets height to zero, which places and
picks up the aircraft on the map and recalculates cells. Only players who can
see the aircraft run this path, so land type diverged with fog of war. The
spawner no-ops the three height changes used during shadow drawing.

### Invisible objects

The behind-animation path only runs for visually normal objects. Invisible
objects produced different results depending on ownership. The spawner skips
the animation for invisible objects.

### Shroud check

The shroud-based cheat check in `SessionClass` is bypassed to avoid a desync.

### SSE floating-point state

Some GPU drivers corrupt the MXCSR register while creating a D3D9 device inside
`SetDisplayMode`. The corruption changes rounding mode, so values such as
`100%` parse as `0.99999994` instead of `1.0`, which desynchronizes movement
speed. The spawner repairs MXCSR:

- Right after `SetDisplayMode` returns.
- At session start.
- At the beginning of every logic frame.

The repair only acts when the rounding mode actually changed, and it logs at
most ten repairs.

## Large map support

The `IsoMapPack5` decoder has a fixed buffer size. Large, detailed maps exceed
it and the bottom-left corner is filled with empty tiles at height 0. The
spawner raises the decode limits from 640 x 400 to 1024 x 768, and the buffer
limit from 512000 to 1572864 bytes.

## Small map camera fix

When the map is smaller than the screen resolution, the tactical camera and the
radar could show areas outside the map or draw glitches. The spawner:

- Clamps the tactical position to the real map rectangle.
- Fills the area outside the map with black.
- Clamps the radar view rectangle to the radar bounds.

## Network fixes

- Local IPv4 interfaces and broadcast addresses are enumerated with
  `GetAdaptersAddresses` instead of the legacy host lookup.
- Port matching is enforced when a tunnel is used or when player ports differ
  from the listen port.

## UI and input fixes

- The "A mouse is required" check is removed, so the game starts on systems
  where it would otherwise fail.
- Spacebar no longer exits the waiting-for-players window.
- Type-select key spam does not drop the frame rate.
- Tab opens the campaign briefing and works while input is locked.
- Escape releases are ignored while gameplay input is locked, but movies can
  still be skipped by Escape.
- Loading screens no longer sleep on a cursor operation that does nothing when
  the mouse is not captured.
- Score screen button animations are skipped when the spawner is active.
- The title screen background is not drawn while loading.

## Text and font fixes

CnCNet builds replace `game.fnt` with `cncnet.fnt`, and entity ID length checks
log a developer warning instead of showing a message box.

These fixes apply automatically. There is no INI key to disable them
individually, apart from `DisableGameSpeed`, `DisableChat`, `AllowChat`,
`AllowTaunts`, `DisableEdgeScrolling`, and the video keys documented elsewhere.
