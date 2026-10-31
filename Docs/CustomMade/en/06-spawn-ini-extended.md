# SPAWN.INI: Extended Options

These `[Settings]` keys switch optional behaviour that does not fit the game
mode, player, or network groups.

| Key | Type | Default | Meaning |
|---|---|---|---|
| `Ra2Mode` | bool | `no` | Enable Red Alert 2 compatibility mode. |
| `DisableGameSpeed` | bool | `no` | Remove game-speed control from the in-game options dialog. |
| `QuickMatch` | bool | `no` | Enable the quick-match name hiding feature. |
| `QuickMatch.ProgressScreen` | bool | inherit `QuickMatch` | Hide player names on the loading progress screen. |
| `QuickMatch.Diplomacy` | bool | inherit `QuickMatch` | Hide player names in the diplomacy dialog. |
| `QuickMatch.InGameDialog` | bool | inherit `QuickMatch` | Hide player names in the in-game player dialog. |
| `QuickMatch.WaitForPlayers` | bool | inherit `QuickMatch` | Hide player names while waiting for players. |
| `SkipScoreScreen` | bool | inherits `RA2MD.INI` | Skip the multiplayer score screen. |
| `WriteStatistics` | bool | `no` | Enable statistics collection. |
| `GenerateStatistics` | bool | `yes` | Write the `debug\Statistic.log` summary. |
| `AINamesByDifficulty` | bool | `no` | Name AI houses by difficulty instead of "Computer". |
| `ContinueWithoutHumans` | bool | `no` | Keep the match running when no human players are left. |
| `DefeatedBecomesObserver` | bool | `no` | Turn a defeated player into an observer instead of ending their session. |
| `NoneEnemySwitchTeamCivilWarMode` | bool | `no` | Start a civil war when no enemies remain. |
| `Observer.ShowAIOnSidebar` | bool | `no` | Show AI houses on an observer's sidebar. |
| `ShowLoadingProgress` | bool | `no` | Draw a per-player loading percentage on the load screen. |
| `DisableChat` | bool | `no` | Disable all in-game chat. |

## RA2 mode

`Ra2Mode=yes` applies the Red Alert 2 compatibility patches. The mode is also
activated when either of these is true:

- `LoadSaveGame=yes` and the save's version equals the value passed with
  `-RA2ModeSaveID=`.
- `IsSinglePlayer=yes` and `Scenario` begins with `RA2->`.

The patches:

- Change the window title to "Red Alert 2".
- Allow buildings to be selected with the type-select key.
- Allow units inside or on buildings to be selected.
- Allow Dogs and Flak Troopers to receive their speed boost.
- Relax the "three base units" assumption.
- Hide the minimap on the load screen.
- Make the Psychic Sensor detect disguised units.
- Force `MaxControlNodes` to 1.
- Use the classic border style for UI items.
- Skip the `EXPANDMD*.MIX`, `CAMEOMD.MIX` and `MOVMD03.MIX` archives.
- Swap `LOCALMD.MIX` and `LOADMD.MIX` priority.
- Read the RA2 file names (`AUDIO.MIX`, `THEME.INI`, `MPModes.ini`,
  `MISSION.INI`, and so on) instead of the YR variants.
- Allow allied service depots to repair RA2 units and the BlackHawk.
- Make the Ore Purifier require power, as in RA2.
- Also check alive infantry inside an allied transport when testing whether a
  house is destroyed.

## `DisableGameSpeed`

When `yes`, the speed slider in the game options dialog is disabled for
everyone. When `no`, the spawner allows observers and defeated players to
control the speed under the conditions described below:

- Skirmish observers may always change the speed.
- In multiplayer, a defeated human may change the speed once no human player is
  still playing.

This key affects the UI only. It does not change `GameSpeed` from `[Settings]`.

## Quick match name hiding

Quick match hides player identity in the UI. Each sub-key hides a different
surface, and all four inherit `QuickMatch` when their own key is absent.

Names are replaced with the CSF string `TXT_QuickMatch_PlayerNames`, which
defaults to "Player" when the string is missing.

## Statistics

Statistics require a multiplayer session, and they are not collected in
campaign games.

- `WriteStatistics=yes` writes `stats.dmp` when the statistics packet is sent,
  and attaches extra fields (`SCEN`, `HASH`, `MYID`, `NKEY`, `SKEY`, `ALY*`,
  `BSP*`).
- `GenerateStatistics=yes` writes the human-readable `debug\Statistic.log`
  summary at the end of the match.
- `WriteStatistics` may also be enabled from `RA2MD.INI [Debug]
  WriteStatistics`. Either file can turn it on.
- Set `UIMapName` and `MapHash` in `[Settings]` so the reported scenario name
  and hash are meaningful.

See [07-spawner-systems.md](07-spawner-systems.md) for the file formats.

## `AINamesByDifficulty`

When `yes`, AI houses are renamed from the internal "Computer" name to the
localized difficulty name:

| Difficulty value | Name |
|---|---|
| `0` | Hard AI |
| `1` | Normal AI |
| `2` | Easy AI |

Other values keep the original name.

## Defeat handling

Three keys change what happens when a player or an AI is defeated.

### `ContinueWithoutHumans`

Extends the match when no human players remain, as long as other houses are
still alive. This is useful for dedicated servers and for watching AI-only
matches.

### `DefeatedBecomesObserver`

Turns a defeated house into an observer instead of removing it from the match.
The player keeps watching the game and may use observer UI.

### `NoneEnemySwitchTeamCivilWarMode`

When the original defeat check finds no enemies left, all surviving
non-passive houses are made enemies of each other and the match continues as a
civil war. This key requires at least two surviving houses.

When civil war mode is active, `DefeatedBecomesObserver` still applies to the
house that triggered the check.

## `Observer.ShowAIOnSidebar`

By default, an observer's sidebar lists human houses only. With this key set
to `yes`, non-passive AI houses are listed too. Passive houses and observer
houses are always hidden.

## `ShowLoadingProgress`

Draws a percentage next to each player's name on the loading screen. The text
format is read from the CSF label `TXT_LoadProgressFormat`, which defaults to
`%s %3d`. The spawner appends a percent sign after formatting.

If the label is missing, the default format is used.

## `DisableChat`

Turns off chat for the whole session:

- Outgoing chat messages are not sent, and the sender sees "Chat is disabled.
  Message not sent." at most once every 90 frames.
- Incoming messages are suppressed and the notification sound is skipped.
- The diplomacy dialog chat checkbox is forced off and made read-only.
- Chat masks are reset every frame so the option cannot be re-enabled from the
  alliance menu.

This key is independent of `RA2MD.INI [Options] AllowChat`, which clears chat
masks once at session start. `DisableChat` keeps enforcing the masks for the
whole match.

## Recommended baseline

```ini
[Settings]
Ra2Mode=no
DisableGameSpeed=no
QuickMatch=no
SkipScoreScreen=no
WriteStatistics=no
GenerateStatistics=yes
AINamesByDifficulty=yes
ContinueWithoutHumans=no
DefeatedBecomesObserver=no
NoneEnemySwitchTeamCivilWarMode=no
Observer.ShowAIOnSidebar=yes
ShowLoadingProgress=yes
DisableChat=no
```
