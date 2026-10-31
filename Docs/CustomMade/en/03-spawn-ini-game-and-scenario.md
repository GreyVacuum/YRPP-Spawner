# SPAWN.INI: Game Mode and Scenario

`SPAWN.INI` is read by the spawner when the session is created. This page
covers the `[Settings]` keys that define how a match is played and which
scenario is loaded.

Player slots and house details have their own page:
[04-spawn-ini-players-and-houses.md](04-spawn-ini-players-and-houses.md).

## Game mode keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `GameMode` | int | `1` | Index into the multiplayer game-mode list loaded from `MPModesMD.ini` (or `MPModes.ini` in RA2 mode). The selected mode decides objectives, team rules and allowed AI slots. |
| `Bases` | bool | `yes` | Start each player with a construction yard and MCV. |
| `Credits` | int | `10000` | Starting credits. |
| `BridgeDestroy` | bool | `yes` | Allow bridges to be destroyed. |
| `Crates` | bool | `no` | Enable crate pickups. |
| `ShortGame` | bool | `no` | Ends the match when a player loses their base. |
| `Superweapons` | bool | `yes` | Enable superweapons. Note the exact key spelling `Superweapons`. |
| `BuildOffAlly` | bool | `no` | Allow building on allied structures. |
| `GameSpeed` | int | `0` | Engine game-speed index. `0` is the fastest native slot, `6` the slowest. |
| `MultiEngineer` | bool | `no` | Allow more than one engineer to capture a structure. When `no`, the spawner sets `EngineerCaptureLevel` to 1.0. |
| `UnitCount` | int | `0` | Unit-count option as defined by the selected game mode. |
| `AIPlayers` | int | `0` | Number of AI opponents the mode should fill. |
| `AIDifficulty` | int | `1` | Default AI difficulty for generated AI houses. |
| `AlliesAllowed` | bool | `no` | Allow players to change alliances in game. |
| `HarvesterTruce` | bool | `no` | Harvesters ignore each other. |
| `FogOfWar` | bool | `no` | Enable fog of war. |
| `MCVRedeploy` | bool | `yes` | Allow the MCV to redeploy into a construction yard. |
| `SpecialHouseIsAlly` | bool | `yes` | Treat the special (neutral) house as allied to all players. |
| `UIGameMode` | string (UTF-8) | empty | Display name shown in the diplomacy dialog instead of the mode name from the INI. |
| `CooperativeDescription` | bool | `no` | Replace the game-mode description with the cooperative description. |
| `CooperativeAIAutoAlly` | bool | `no` | In cooperative mode, automatically ally all AI houses. |
| `CooperativePlayerAutoAlly` | bool | `no` | In cooperative mode, automatically ally all human players. |

`GameSpeed` is an engine index, not a frame rate. Frame-rate overrides are
configured with `MP.*` keys, described in
[05-spawn-ini-network.md](05-spawn-ini-network.md).

## Save game keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `LoadSaveGame` | bool | `no` | Load `SaveGameName` instead of starting a new scenario. |
| `SavedGameDir` | string | `Saved Games` | Directory that holds save files. Nested paths are allowed, for example `Saved Games\Yuri's Revenge`. |
| `SaveGameName` | string | empty | File name of the save to load. |
| `CustomMissionID` | int | `0` | Custom mission identifier used when a campaign mission is loaded. |
| `AutoSaveCount` | int | `5` | Number of rotating `AUTOSAVE*.SAV` slots. `0` disables automatic saving. |
| `AutoSaveInterval` | int | `7200` | Frames between automatic saves. At the default game pacing this is roughly two minutes. `0` disables automatic saving. |
| `NextAutoSaveNumber` | int | `0` | Index of the next automatic save slot. The value wraps back to 0 after `AutoSaveCount`. |
| `ShowMPSaveGameButton` | bool | `yes` | Shows the save button in the multiplayer pause menu. |

Automatic saving only starts when both `AutoSaveCount > 0` and
`AutoSaveInterval > 0`, and when the session has actually started. Slots are
written as `AUTOSAVE1.SAV` up to `AUTOSAVEn.SAV`, where `n` equals
`AutoSaveCount`.

## Scenario keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `Seed` | int | `0` | Random seed. `0` lets the game choose one. |
| `TechLevel` | int | `10` | Technology level for the scenario. |
| `IsSinglePlayer` | bool | `no` | Marks the session as a campaign or single-player mission. The internal field is named `IsCampaign`. |
| `Tournament` | int | `0` | Tournament mode index. |
| `GameID` | int | `0xDEADBEEF` | Game identifier used for WOL and statistics. |
| `Scenario` | string | `spawnmap.ini` | Scenario file to load. Two prefixes are understood, see below. |
| `MapHash` | string | empty | Map hash reported in statistics. |
| `UIMapName` | string (UTF-8) | empty | Map name shown in the UI and sent as the `SCEN` statistics field. |
| `ReadMissionSection` | bool | `no` | Read the `[Mission]` section from `SPAWN.INI` instead of the scenario file. |
| `ReadCoopCampSection` | bool | `no` | Read the cooperative campaign section from `SPAWN.INI` instead of the scenario file. |

### `Scenario` prefixes

`Scenario` supports two special prefixes that are handled before the file name
is used:

- `RA2-><name>` strips the prefix and enables RA2 mode.
- `PlayMovies-><name>` strips the prefix and plays the listed movies before the
  scenario starts. Multiple movie names are separated by the parser's standard
  delimiters.

```
Scenario=RA2->spawnmap.ini
Scenario=PlayMovies->intro.vqa,logo.vqa
```

`MapHash` and `UIMapName` are recommended whenever `WriteStatistics` or
`GenerateStatistics` is enabled, because the statistics packet reports them.

## Example

```ini
[Settings]
GameMode=1
Bases=yes
Credits=10000
BridgeDestroy=yes
Crates=yes
ShortGame=no
Superweapons=yes
BuildOffAlly=no
GameSpeed=0
MultiEngineer=no

UIGameMode=Standard
SpecialHouseIsAlly=yes

LoadSaveGame=no
SavedGameDir=Saved Games
SaveGameName=
AutoSaveCount=5
AutoSaveInterval=7200

Seed=0
TechLevel=10
Scenario=spawnmap.ini
MapHash=
UIMapName=
ReadMissionSection=no
ReadCoopCampSection=no
```
