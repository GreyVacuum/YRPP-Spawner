# CSF String Tables

This page describes the `CSF` string table used by Yuri's Revenge, how the
spawner reads text from it, and which labels the spawner can be customized with.
It is aimed at mod and client packagers who need to change on-screen text
without recompiling the DLL.

## What a CSF file is

`CSF` stands for **Command & Conquer String File**. It is the binary string
table the engine loads at startup. Every label such as `TXT_OK` or
`GUI_AIHard` is stored once, and the game looks up the same string from
every dialog, tooltip, EVA message and HUD element.

The spawner never ships its own string table. It calls the engine's `StringTable`
class, which is already loaded when the spawner's hooks run:

- `StringTable::LoadString(label)` - looks up a label and returns
  `L"MISSING:<label>"` when the label is absent.
- `StringTable::TryFetchString(label, fallback)` - looks up a label and returns
  the caller's fallback when the result starts with `MISSING:` or is blank.
  The spawner uses this form so a missing label never breaks the UI.

Both functions are declared in `YRpp/StringTable.h`.

## File and load order

| Product | File the engine loads |
|---|---|
| Yuri's Revenge | `ra2md.csf` |
| Red Alert 2 (classic mode) | `ra2.csf` |

The engine resolves the file through its normal file system. A loose file in the
game directory takes precedence over the same file packed in a MIX archive, so
the practical options are:

1. Place the edited `ra2md.csf` next to `gamemd.exe` (launcher distributions
   usually do this through their own MIX or loose-file package).
2. Repack the MIX archive that contains the stock file.

The table is read once at startup. Restarting the game is required for any
change to appear. Loading code lives at `0x7346A0` (`LoadFile`) and `0x734990`
(`ReadFile`); the loaded file name is stored in `StringTable::FileName`.

## Binary format

The layout below matches the structures in `YRpp/StringTable.h`.

### Header (24 bytes)

| Offset | Type | Field | Notes |
|---|---|---|---|
| `0x00` | `uint32` | Signature | Must be `0x43534620` (`" FSC"`, i.e. `CSF` + space). |
| `0x04` | `int` | Version | Yuri's Revenge uses `3`. |
| `0x08` | `int` | Label count | Number of label records that follow. |
| `0x0C` | `int` | Value count | Number of value records that follow. |
| `0x10` | `int` | Unused | Keep `0`. |
| `0x14` | `int` | Language | `0` US, `1` UK, `2` German, `3` French, `4` Spanish, `5` Italian, `6` Japanese, `7` Jabberwockie, `8` Korean, `9` Chinese. |

When the version field is lower than `2`, the engine forces the language to US.

### Label record (signature + 40 bytes)

| Offset | Type | Field | Notes |
|---|---|---|---|
| `0x00` | `uint32` | Signature | `0x4C424C20` (`" LBL"`). |
| `0x04` | `char[32]` | Name | Label text, at most 31 characters. |
| `0x24` | `int` | Value count | Entries owned by this label. |
| `0x28` | `int` | First value index | Index of the first value in the global value array. |

### Value record

| Offset | Type | Field | Notes |
|---|---|---|---|
| `0x00` | `uint32` | Signature | `0x53545220` (`" RTS"`). `0x53545257` (`"WRTS"`) marks a value with an extra string. |
| `0x04` | `uint32` | Character count | Number of UTF-16 code units stored, excluding any terminator. |
| `0x08` | `wchar_t[]` | Text | UTF-16LE characters. A null terminator is not stored. |

The first value record starts right after the last label record:
`0x18 + label_count * 0x2C`. Extra strings (`WRTS`) follow the main text and are
used by a few engine labels; the spawner labels below use plain `RTS` values.

## Limits that matter in practice

- **Label length**: 31 characters. Longer names are truncated or rejected.
- **Value length**: the engine copies a loaded value into an in-memory
  `CSFString` holding `wchar_t Text[102]`, so keep strings under about 100
  characters. Long descriptions should be split or shortened.
- **Counts**: `Label count` and `Value count` in the header must match the
  records actually present. A wrong count makes the table unreadable or
  truncated.
- **Encoding**: the whole file is binary UTF-16LE. Saving a value as ANSI text
  produces garbled characters.

## Labels the spawner reads

All labels are optional. When one is missing, the spawner uses the fallback
shown below, so an unmodified stock `ra2md.csf` works.

| Label | Fallback when absent | Read by | Related setting |
|---|---|---|---|
| `TXT_CNCNET_LATENCY_0` .. `TXT_CNCNET_LATENCY_9` | `CnCNet: Latency mode set to: N - ...` | Protocol 0 latency notice | `[Settings] Protocol`, `LatencyLevel` |
| `TXT_QuickMatch_PlayerNames` | `Player` | Quick match name replacement | `QuickMatch_ProgressScreen`, `QuickMatch_Diplomacy`, `QuickMatch_InGameDialog`, `QuickMatch_WaitForPlayers` |
| `TXT_LoadProgressFormat` | `%s %3d` | Loading screen percentage | `ShowLoadingProgress` |
| `TXT_AUTOSAVE_SUFFIX` | `Autosave (slot %d)` | Singleplayer autosave description | `AutoSaveCount`, `AutoSaveInterval` |
| `GUI_AIHard`, `GUI_AINormal`, `GUI_AIEasy` | Engine strings | AI player names by difficulty | `AINamesByDifficulty` |

The spawner also loads these engine labels for its own dialogs and messages:
`TXT_OK`, `TXT_UNABLE_READ_SCENARIO`, `TXT_ERROR_LOADING_GAME`,
`TXT_GAME_WAS_SAVED`, `TXT_ERROR_SAVING_GAME`, `TXT_SAVING_GAME` and
`TXT_MULTIPLAYER_GAME`. Replace their values only if you want different wording
everywhere in the game, not just in the spawner.

## Placeholder rules for format labels

The spawner passes the label value to the CRT formatter, then the game draws the
result. Two different systems interpret the percent sign:

- The **CRT formatter** understands `%s`, `%d`, `%3d` and `%%`.
- The **text renderer** treats a lone `%` as a control character and swallows it.

For `TXT_LoadProgressFormat` the spawner calls the format with two arguments:

1. the player name (`%s`);
2. the completion percentage (`%d`, or `%3d` to right-align it in three columns).

A literal percent sign must be written as `%%` in the label, or appended by code.
The spawner appends `%%` itself after formatting, which is why the default
`%s %3d` renders as `Name  42%`.

For `TXT_AUTOSAVE_SUFFIX` the value receives the slot number with `%d`; the
default already includes it.

## Editing workflow

1. Extract the stock `ra2md.csf` (from the game folder or the MIX archive that
   holds it).
2. Open it with a CSF-capable editor, for example XCC Utilities or an equivalent
   CSF/MIX tool. Plain text editors will corrupt the binary.
3. Edit existing values, or append new labels for the spawner table above.
4. Keep the version field at `3` and the language field unchanged.
5. Save, place the file where the engine will load it before the stock copy, and
   restart the game.
6. Verify: open a dialog that uses the string, or trigger the matching spawner
   feature.

Prefer patching the existing file over replacing it with a minimal one. Maps,
Ares, Phobos and the client rely on many other labels from the same table; a
stripped file breaks them.

## Troubleshooting

| Symptom | Cause |
|---|---|
| Text shows `MISSING:<label>` | Label is absent, or the spelling does not match. |
| Spawner text is missing but the game text is fine | The label was added to the wrong file (`ra2.csf` instead of `ra2md.csf`) or to a MIX that has lower priority. |
| Percent sign disappears | A lone `%` was used instead of `%%`. |
| Text is garbled or cut at the first character | The value was saved as ANSI instead of UTF-16LE, or the character count does not match the stored text. |
| Changes do not appear | The game was not restarted, or the edited file is shadowed by a MIX archive copy. |
| The whole table fails to load | Header counts do not match the number of label and value records. |
| AI names still show the old text with `AINamesByDifficulty` | `GUI_AIHard`, `GUI_AINormal` or `GUI_AIEasy` was not replaced, or the switch is off. |

Caveat: string tables are local data. Different players can run different CSF
files and still stay in sync; only the text they see differs.
