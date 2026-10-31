# CnCNet Spawner Documentation

This folder documents the CnCNet Spawner: a Syringe-compatible extension DLL for
Command & Conquer: Yuri's Revenge that prepares the game session, network
settings and UI for launcher-driven multiplayer.

The documentation is split by configuration surface. Almost every setting belongs
to exactly one of two files, so you can read only the page you need.

## Configuration surfaces

| Surface | File | Loaded by | Scope |
|---|---|---|---|
| Game options | `RA2MD.INI` | The game itself, read by the spawner at startup | Chat, taunts, video, window, debug, statistics |
| Session options | `SPAWN.INI` | The spawner only | Game mode, scenario, players, houses, network, extensions |

Both files are plain INI files. They are read once, before the scenario starts.
A key that is not present keeps the documented default value.

## Document index

| Document | Contents |
|---|---|
| [01-command-line-arguments.md](01-command-line-arguments.md) | Command-line switches the spawner understands |
| [02-ra2md-ini.md](02-ra2md-ini.md) | Every `RA2MD.INI` key the spawner reads |
| [03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md) | `SPAWN.INI` game mode, save game and scenario keys |
| [04-spawn-ini-players-and-houses.md](04-spawn-ini-players-and-houses.md) | `SPAWN.INI` player and house sections, alliances, AI slots |
| [05-spawn-ini-network.md](05-spawn-ini-network.md) | `SPAWN.INI` protocol, latency, tunnel and frame-rate control |
| [06-spawn-ini-extended.md](06-spawn-ini-extended.md) | `SPAWN.INI` extended switches for RA2 mode, chat, observers and statistics |
| [07-spawner-systems.md](07-spawner-systems.md) | Save games, statistics, random maps, custom MIX files, UI systems |
| [08-fixed-bugs.md](08-fixed-bugs.md) | Engine crashes, desyncs and input bugs fixed by the spawner |
| [09-csf-strings.md](09-csf-strings.md) | CSF string table format and the labels the spawner reads |

## Conventions used in these pages

- **Default** is the value used when the key is absent or empty.
- **Type** is the INI type the parser expects:
  - `bool` accepts `yes`/`no`, `true`/`false`, `1`/`0`.
  - `int` is a decimal or `0x` prefixed number.
  - `float` is a decimal number.
  - `string` is raw text. Names and UI strings are read as UTF-8.
- Keys are matched case-insensitively by the game INI parser, but the pages show
  the canonical spelling used by the source.
- Order matters only where it is stated. Otherwise keys can appear in any order
  inside their section.

## How the spawner starts

1. The game reads `RA2MD.INI` and the spawner applies the static options
   (affinity, window mode, chat masks, `ddraw.dll` settings).
2. The spawner reads `SPAWN.INI` when the session is created.
3. The configured players, houses, scenario and network modes are injected into
   the game session.
4. The scenario starts. The spawner keeps running hooks for auto-save,
   statistics, observers and UI behaviour until the game exits.

## Which file should I edit?

- Ask yourself whether the option must exist before a match starts
  (paths, player slots, protocol) or whether it is a client or user preference
  (chat, window, video, debug logging).
- Session content goes in `SPAWN.INI`. Personal and machine preferences go in
  `RA2MD.INI`.
- Launchers usually generate both files. Edit them only when you run the game
  directly, or when you are building a fixed distribution.

## Build variants

The spawner is compiled in three flavours:

- **CnCNet YR / Hardened**: `-SPAWN` is mandatory. The game refuses to start
  without it.
- **Development build**: `-SPAWN` is optional. Without it the spawner stays
  disabled and normal game behaviour is preserved.

See [01-command-line-arguments.md](01-command-line-arguments.md) for details.

## Version note

These pages describe the source tree in this repository. If you use a build from
another branch or an older release, compare the key list with the source files
`src/Main.Config.cpp` and `src/Spawner/Spawner.Config.cpp` before relying on a
setting.
