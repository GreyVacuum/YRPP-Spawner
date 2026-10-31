# SPAWN.INI: Players and Houses

A session has up to eight slots. Each slot is described by two independent
configuration blocks:

- **Player config** decides whether the slot is a human player, and what name,
  colour, side, observer flag and network address it uses.
- **House config** augments the generated `HouseClass`: starting locations,
  credit multiplier, handicap and alliances.

The two blocks use different section names, so a single slot is assembled from
several sections.

## Player sections

| Section | Slot |
|---|---|
| `[Settings]` | Player 1 |
| `[Other1]` | Player 2 |
| `[Other2]` | Player 3 |
| `[Other3]` | Player 4 |
| `[Other4]` | Player 5 |
| `[Other5]` | Player 6 |
| `[Other6]` | Player 7 |
| `[Other7]` | Player 8 |

`[Settings]` is also the main options section. It can hold both the game-mode
keys from [03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md)
and the player-1 keys listed below.

### Presence means "human"

A slot becomes a human player when its section exists:

- `[Settings]` always exists, so player 1 is always treated as human.
- `[OtherN]` creates a human player in that slot.
- If an `[OtherN]` section is absent, the slot is filled by an AI house using
  the `[HouseColors]`, `[HouseCountries]` and `[HouseHandicaps]` defaults.

When a player section exists, its `Difficulty` is reset to `-1` before the
default difficulty keys are considered.

### Player keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `Name` | string (UTF-8) | empty | Display name. Stored in a 20-character wide buffer. |
| `Color` | int | `-1` | House colour index. `-1` means "use the slot default". |
| `Side` | int | `-1` | Country index. Note that the INI key is `Side`, while the internal field is `Country`. |
| `IsSpectator` | bool | `no` | Starts the player as an observer. |
| `Ip` | string | `0.0.0.0` | Network address of the player. |
| `Port` | int | `-1` | Network port of the player. |

## AI slot defaults

When a slot has no player section, the following sections provide the AI
house values. The slot tags are `Multi1` through `Multi8`.

| Section | Key | Type | Default | Meaning |
|---|---|---|---|---|
| `[HouseColors]` | `MultiN` | int | `-1` | Colour index for the AI house. |
| `[HouseCountries]` | `MultiN` | int | `-1` | Country index for the AI house. |
| `[HouseHandicaps]` | `MultiN` | int | `-1` | Difficulty for the AI house. |

Example:

```ini
[HouseColors]
Multi1=0
Multi2=1
Multi3=2

[HouseCountries]
Multi1=0
Multi2=1
Multi3=2

[HouseHandicaps]
Multi1=1
Multi2=1
Multi3=1
```

## House sections

These sections apply to every slot, human or AI. The slot tags are again
`Multi1` through `Multi8`.

| Section | Key | Type | Default | Meaning |
|---|---|---|---|---|
| `[IsSpectator]` | `MultiN` | bool | `no` | Mark the house as an observer house. |
| `[SpawnLocations]` | `MultiN` | int | `-2` | Force a starting location. `-2` means "let the game assign one". |
| `[CreditsFactor]` | `MultiN` | float | `1.0` | Multiplier applied to the starting credits. |
| `[HandicapDifficulty]` | `MultiN` | int | `-1` | Per-house handicap difficulty. `-1` keeps the default. |

`CreditsFactor` is a floating-point value. It is applied on top of `Credits`
from `[Settings]`, so `CreditsFactor=1.5` with `Credits=10000` starts the house
with 15000 credits.

## Alliances

Each slot has its own alliance section named `[MultiN_Alliances]`, where `N` is
1 through 8:

```ini
[Multi1_Alliances]
HouseAllyOne=2
HouseAllyTwo=3
HouseAllyThree=-1
HouseAllyFour=-1
HouseAllyFive=-1
HouseAllySix=-1
HouseAllySeven=-1
HouseAllyEight=-1
```

| Key | Type | Default |
|---|---|---|
| `HouseAllyOne` | int | `-1` |
| `HouseAllyTwo` | int | `-1` |
| `HouseAllyThree` | int | `-1` |
| `HouseAllyFour` | int | `-1` |
| `HouseAllyFive` | int | `-1` |
| `HouseAllySix` | int | `-1` |
| `HouseAllySeven` | int | `-1` |
| `HouseAllyEight` | int | `-1` |

A value of `-1` means "no forced ally". Positive values are house indices.
The spawner applies these alliance entries after houses are created, so they
work together with the mode's own team assignment.

Two cooperative-mode switches can also force alliances automatically:

- `CooperativeAIAutoAlly` allies every AI house in cooperative mode.
- `CooperativePlayerAutoAlly` allies every human player in cooperative mode.

See [03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md).

## Complete example

```ini
[Settings]
Name=Player One
Color=0
Side=0
Ip=0.0.0.0
Port=-1

[Other1]
Name=Player Two
Color=1
Side=1

[HouseColors]
Multi2=1
Multi3=2
Multi4=3

[HouseCountries]
Multi2=1
Multi3=2
Multi4=3

[HouseHandicaps]
Multi2=1
Multi3=1
Multi4=1

[IsSpectator]
Multi1=no
Multi2=no

[SpawnLocations]
Multi1=0
Multi2=1

[CreditsFactor]
Multi1=1.0
Multi2=1.0

[HandicapDifficulty]
Multi1=-1
Multi2=-1

[Multi1_Alliances]
HouseAllyOne=2

[Multi2_Alliances]
HouseAllyOne=1
```

## Notes

- Slot indices are zero-based internally. The INI tags `Multi1` through
  `Multi8` map to internal indices 0 through 7.
- `Side` values are country indices, not display names. Use `-DumpTypes` or
  `DumpRulesTypes.ini` to list the available `[Countries]` indices.
- If a name is longer than the buffer, the game keeps the first 19 characters.
- Observer houses are skipped when statistics are generated.
