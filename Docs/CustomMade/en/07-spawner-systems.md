# Spawner Systems and UI

This page describes runtime systems that are not controlled by a single INI
key: save games, statistics, custom MIX files, observers, video modes and UI
fixes.

## Save games

The spawner redirects every save operation through `SavedGameDir`:

- Saving, loading, deleting and listing save files all use the configured
  directory.
- Nested paths are supported, for example `Saved Games\Yuri's Revenge`.
- The directory is created automatically before a save is written.
- If `SavedGameDir` is empty, the original behaviour is preserved.

Additional save metadata is written alongside the game save:

- The custom mission ID is stored so third-party campaigns can be filtered.
- A `CustomMissionID` value of `0` keeps the save outside the custom mission
  filter.

### Automatic saves

When `AutoSaveCount > 0` and `AutoSaveInterval > 0`, the spawner writes a
rotating save every `AutoSaveInterval` frames:

- Files are named `AUTOSAVE1.SAV` through `AUTOSAVEn.SAV`.
- The next index is `NextAutoSaveNumber`, and wraps back to 0 after the last
  slot.
- The counter and frame timer reset when a new game starts.
- Automatic saving only runs after the session has started.

The multiplayer pause menu shows a save button when `ShowMPSaveGameButton`
is `yes`.

## Statistics

Statistics are written only for multiplayer sessions and never for campaign
games.

### `stats.dmp`

Written when the statistics packet is sent. It contains the raw packet with
extra fields:

- `SCEN`: `UIMapName`.
- `HASH`: `MapHash`.
- `MYID`, `NKEY`, `SKEY`: player identity fields.
- `ALY*`, `BSP*`: per-player alliances and spawn positions.

### `debug\Statistic.log`

A human-readable summary written when the score dialog is prepared:

```
Name: Winner
 Lost = N
 Kills = N
 Built = N
 Score = N
...
Game loop finished. Average FPS = N
```

Behaviour:

- Human houses use their displayed name. AI houses are always written as
  "Computer".
- Passive and observer houses are skipped, matching the in-game score screen.
- The score values come from the same array that fills the score dialog.
- The file is written only when `GenerateStatistics=yes`.

Both files are controlled by `WriteStatistics`, which may be enabled in
`SPAWN.INI` or `RA2MD.INI`.

## Custom MIX files

In CnCNet YR builds, the spawner mounts two extra MIX archives during startup:

- `cncnet.mix` is always loaded.
- `ra2mode.mix` is loaded when RA2 mode is active.

Both archives are released when the game shuts down.

## Random maps

When a scenario is marked as a random map, the spawner:

- Reads the `[Basic] RandomMap` flag from the scenario file.
- Applies the configured seed so all players generate the same map.
- Fixes starting locations and unit placement after generation.

This keeps random maps synchronized across multiplayer clients.

## Observer support

Observers are supported in multiplayer sessions:

- A player with `IsSpectator=yes` becomes an observer house.
- The observer's starting point is removed from the scenario house list.
- The sidebar can list AI houses when `Observer.ShowAIOnSidebar=yes`.
- Defeated players may become observers when `DefeatedBecomesObserver=yes`.
- Skirmish observers may change the game speed; multiplayer observers may do
  so once no human player is still playing.

## Video and window modes

When the spawner is enabled:

- High-resolution modes are unlocked in the options dialog.
- The resolution check is skipped, so all supported 16-bit modes are accepted.
- The shell layout is recalculated for the selected resolution, including the
  map preview position.
- `Video.Windowed=yes` runs the game in a window, and `NoWindowFrame=yes`
  removes the border.
- The menu background is not drawn during loading.
- Windows blit is replaced by the ddraw blit, which is thread safe.

## UI fixes

### Pause menu and dialogs

- The multiplayer pause menu gains a save button when enabled.
- Escape closes several dialogs as if the back button were pressed.

### Radar

- Dragging on the radar moves the tactical camera.
- The radar view rectangle is clamped to the radar image, so a small map
  cannot draw a view box outside the radar.

### Loading screen

- When `ShowLoadingProgress=yes`, each player name is followed by a percentage.
- The loading screen no longer waits on cursor operations that do nothing while
  the mouse is uncaptured. This removes roughly two seconds from every
  scenario load.
- The window title is patched to "Red Alert 2" in RA2 mode.

### Score screen

- `SkipScoreScreen=yes` closes the score screen immediately.
- Button animations are skipped when the spawner is active.

## Input fixes

- Spacebar no longer closes the "waiting for players" window.
- Tab opens the campaign briefing dialog, and Tab presses are accepted even
  while input is locked.
- Escape releases are ignored while gameplay input is locked, but movies can
  still be skipped.
- Type-select key spam no longer drops the frame rate.
- Edge scrolling can be disabled.

## Game speed

The in-game speed slider can be disabled with `DisableGameSpeed`. Observers
receive the exceptions described above. The `GameSpeed` value in
`[Settings]` defines the starting speed index.

## No-CD and copy protection

- `-CD`, `SPAWN.INI`, or the absence of a CD drive enables No-CD mode.
- Copy protection checks are neutralized, so the game can start without the
  original launcher.
- The CD-ROM file path is skipped and secondary MIX checks are not performed.

## Network interfaces

The spawner enumerates local IPv4 interfaces with `GetAdaptersAddresses`
instead of the legacy host-name lookup. This produces a reliable list of
addresses and broadcast addresses, including machines with several adapters.

## Miscellaneous

- `SkipCreateAppMutex=yes` allows more than one game instance.
- `QuickExit=yes` skips shutdown waits.
- `MPDebug=yes` draws multiplayer debug statistics.
- `AllowTaunts=no` also mutes the house cheer sound.
- Open-topped transports do not decloak when a passenger fires.
- The Blowfish replacement removes the export restriction of the original
  implementation.
- The application icon and the `cncnet.fnt` font are installed by CnCNet
  builds.
