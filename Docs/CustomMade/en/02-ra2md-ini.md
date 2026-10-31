# RA2MD.INI Reference

`RA2MD.INI` is the game's own settings file. The spawner reads three sections
from it and applies them at startup, before `SPAWN.INI` is read.

All keys below are optional. When a key is absent the default shown in the
table is used.

## `[Options]`

| Key | Type | Default | Meaning |
|---|---|---|---|
| `AllowChat` | bool | `yes` | When `no`, every player's chat mask is cleared, so in-game chat is disabled for all slots. |
| `AllowTaunts` | bool | `yes` | Copied to `SessionClass::LANTaunts`. When `no`, the house cheer sound is muted as well as taunts. |
| `DDrawHandlesClose` | bool | `no` | The spawner writes the inverted value into the `ddraw.dll` wrapper's `GameHandlesClose` export. With the default `no`, the wrapper handles window close; with `yes`, the game does. |
| `DisableEdgeScrolling` | bool | `no` | Disables scrolling when the mouse touches the screen edge. |
| `QuickExit` | bool | `no` | Skips normal shutdown waits. Use on dedicated or automated setups. |
| `SingleProcAffinity` | bool | `yes` | Pins the process to the first CPU core. This is the safe default for the old engine. |
| `SkipScoreScreen` | bool | `no` | Skips the multiplayer score dialog after a match. |
| `SpeedControl` | bool | `no` | Enables the engine's in-game speed control global. |
| `MPDEBUG` | bool | `no` | Legacy alias for `[Debug] MPDebug`. The `[Debug]` key takes precedence. |

## `[Video]`

| Key | Type | Default | Meaning |
|---|---|---|---|
| `DDrawTargetFPS` | int | `-1` | Target FPS handed to the `ddraw.dll` wrapper. `-1` leaves the wrapper untouched. |
| `NoWindowFrame` | bool | `no` | Removes the window border when windowed mode is on. Only applied when `Video.Windowed=yes`. |
| `Video.Windowed` | bool | `no` | Runs the game in a window instead of fullscreen. |

`NoWindowFrame` works by patching the main-window style constant. It has no
effect unless windowed mode is enabled.

## `[Debug]`

| Key | Type | Default | Meaning |
|---|---|---|---|
| `DumpTypes` | bool | `no` | Writes `DumpRulesTypes.ini`. Equivalent to the `-DumpTypes` switch. |
| `ForceMultiplayer` | bool | `no` | Forces multiplayer session behaviour even in a single-player launch. |
| `MPDebug` | bool | `no` | Enables multiplayer debug and sync statistics. |
| `MPDebug.Show` | bool | `yes` | Draws the on-screen MP debug overlay. Only used when `MPDebug=yes`. |
| `SkipCreateAppMutex` | bool | `no` | Skips the single-instance mutex check. Allows more than one game instance. |
| `WriteStatistics` | bool | `no` | Enables statistics collection. Also enabled by the same key in `SPAWN.INI`. |

## Precedence and interactions

- `WriteStatistics` is an OR across `RA2MD.INI` and `SPAWN.INI`. Setting it in
  either file enables the feature.
- `SkipScoreScreen` in `SPAWN.INI` defaults to the value of
  `RA2MD.INI [Options] SkipScoreScreen`, so a global setting can be inherited
  by the session config.
- `MPDEBUG` in `[Options]` is only a compatibility fallback. Prefer
  `[Debug] MPDebug`.
- `-CD` and `-DumpTypes` on the command line behave like setting `NoCD` and
  `DumpTypes` respectively. Command-line switches win because they are parsed
  before the INI is applied.

## Example

```ini
[Options]
AllowChat=yes
AllowTaunts=no
DDrawHandlesClose=yes
DisableEdgeScrolling=yes
QuickExit=no
SingleProcAffinity=yes
SkipScoreScreen=no
SpeedControl=yes

[Video]
DDrawTargetFPS=60
NoWindowFrame=yes
Video.Windowed=yes

[Debug]
DumpTypes=no
ForceMultiplayer=no
MPDebug=no
MPDebug.Show=yes
SkipCreateAppMutex=no
WriteStatistics=no
```
