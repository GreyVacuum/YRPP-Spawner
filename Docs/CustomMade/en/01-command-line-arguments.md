# Command-Line Arguments

The spawner reads the game command line very early, before the game window is
created. Switches are matched case-insensitively.

## Supported switches

| Switch | Type | Purpose |
|---|---|---|
| `-SPAWN` | flag | Marks this process as a spawner session. |
| `-CD` | flag | Forces No-CD mode. |
| `-DumpTypes` | flag | Writes the loaded type list to `DumpRulesTypes.ini`. |
| `-RA2ModeSaveID=<number>` | value | Defines the save-game version that activates RA2 mode. |

## `-SPAWN`

`-SPAWN` is the master switch.

- In **CnCNet YR** and **Hardened** builds it is required. If the switch is
  missing, the game shows a message box and terminates with
  `ERROR_BAD_ARGUMENTS`.
- In the **development** build it is optional. Without it, `Spawner::Enabled`
  is false and the spawner runs in a pass-through mode that preserves normal
  single-player behaviour.

Launchers add `-SPAWN` automatically. Add it yourself when you test the game
from a shortcut.

```
GameMD.exe -SPAWN
```

## `-CD`

Forces No-CD mode even when a CD drive is present. No-CD mode also switches on
automatically when no CD drive is detected.

What changes:

- Files are no longer read from the CD-ROM drive.
- Secondary MIX file checks are skipped.

```
GameMD.exe -SPAWN -CD
```

## `-DumpTypes`

Writes `DumpRulesTypes.ini` to the game directory. The file lists every type
loaded by the game, indexed from 1, in these sections:

```
[Countries]
[InfantryTypes]
[VehicleTypes]
[AircraftTypes]
[BuildingTypes]
[TerrainTypes]
[SmudgeTypes]
[OverlayTypes]
[Animations]
[VoxelAnims]
[Particles]
[ParticleSystems]
[WeaponTypes]
[SuperWeaponTypes]
[Warheads]
[Projectiles]
```

Each line has the form `index=ID`, optionally followed by `;DisplayName` when
the ID differs from the display name. This is useful for validating modded
rules files and for building AI or country index lists.

The same behaviour can be enabled without a switch by setting
`[Debug] DumpTypes=yes` in `RA2MD.INI`.

## `-RA2ModeSaveID=<number>`

Defines a save-game format version. When a save is loaded and its stored version
equals this value, the spawner applies its RA2-mode compatibility patches.

The number accepts decimal or `0x` prefixed input.

```
GameMD.exe -SPAWN -RA2ModeSaveID=0x1234
```

RA2 mode is also activated by these conditions:

- `[Settings] Ra2Mode=yes` in `SPAWN.INI`.
- `[Settings] Scenario` begins with the `RA2->` prefix.

See [06-spawn-ini-extended.md](06-spawn-ini-extended.md) for the full RA2 mode
trigger set.

## Combining switches

Switches are independent and may appear in any order:

```
GameMD.exe -SPAWN -CD -DumpTypes -RA2ModeSaveID=1234
```

Unknown switches are ignored by the spawner. They are left for the game or other
Syringe plugins to interpret.
