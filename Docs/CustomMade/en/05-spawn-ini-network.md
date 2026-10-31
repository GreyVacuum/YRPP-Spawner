# SPAWN.INI: Network and Frame Rate

This page covers the network keys in `[Settings]`, the `[Tunnel]` section, and
the `MP.*` frame-rate control system.

All frame-rate keys only affect multiplayer sessions. Campaign and skirmish
games keep the engine's native pacing.

## Network keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `Protocol` | int | `2` | Network protocol. `2` is the standard protocol. `0` enables the dynamic-latency protocol. |
| `FrameSendRate` | int | `4` | Frames between network sends, protocol 2 only. Protocol 0 forces this to `2`. |
| `ReconnectTimeout` | int | `2400` | Reconnect timeout passed to the network layer. |
| `ConnTimeout` | int | `3600` | Timeout written into the scenario load wait. |
| `MaxAhead` | int | `-1` | Maximum look-ahead frames. `-1` means `FrameSendRate * 6`. |
| `PreCalcMaxAhead` | int | `0` | Initial look-ahead used by protocol 0. Ignored by protocol 2. |
| `MaxLatencyLevel` | int | `255` | Highest protocol-0 latency level players may reach. Clamped to 1 through 9. The default `255` therefore becomes 9. |
| `ForceMultiplayer` | bool | `no` | Forces multiplayer session type. Also enabled by `RA2MD.INI [Debug] ForceMultiplayer`. |

## Protocol choice

### Protocol 2 (default)

The standard synchronous protocol. `FrameSendRate` is used as configured, and
`MaxAhead` defaults to `FrameSendRate * 6`.

### Protocol 0

Adds dynamic latency adaptation. When enabled:

- `FrameSendRate` is forced to `2`.
- `PreCalcMaxAhead` is used as the initial value.
- A latency-level system raises the look-ahead when a player falls behind.

Latency levels run from 0 to 9 and use this look-ahead table:

| Level | MaxAhead |
|---|---|
| 0 | 1 |
| 1 | 4 |
| 2 | 6 |
| 3 | 12 |
| 4 | 16 |
| 5 | 20 |
| 6 | 24 |
| 7 | 28 |
| 8 | 32 |
| 9 | 36 |

`MaxLatencyLevel` caps how high the adaptation may go. Levels are announced in
game through the `TXT_CNCNET_LATENCY_0` through `TXT_CNCNET_LATENCY_9` CSF
strings. If a string is missing, a built-in English fallback is used.

## Tunnel keys

The tunnel wraps every outgoing packet with a 4-byte header so a relay can
forward traffic between players behind NAT.

| Section | Key | Type | Default | Meaning |
|---|---|---|---|---|
| `[Settings]` | `Port` | int | `1234` / `0` | In the `[Settings]` section this key initializes `ListenPort` (default `1234`) and `TunnelId` (default `0`). Providing the key sets both to the same value. |
| `[Tunnel]` | `Ip` | string | `0.0.0.0` | Tunnel server address. |
| `[Tunnel]` | `Port` | int | `0` | Tunnel server port. `0` disables the tunnel. |

When `[Tunnel] Port` is non-zero:

- Outgoing packets are sent to the tunnel server with the player ID and the
  original destination port in the header.
- Incoming packets are matched back to the originating player.
- Port matching is required, so the UDP listen port is not used.

When the tunnel is disabled, packets go directly to the configured player
addresses.

## Frame-rate control

The `MP.*` keys tune the in-game frame pacing. They are implemented by writing
the engine's per-frame wait budget. The netcode globals `RequestedFPS` and
`PreCalcFrameRate` are never pinned, because writing them desynchronizes the
game and locks the in-game speed slider.

| Key | Type | Default | Meaning |
|---|---|---|---|
| `MP.MaxFPS` | int | `-2` | Base preset for multiplayer. |
| `MP.Protocol0.MaxFPS` | int | inherit `MP.MaxFPS` | Preset used when protocol 0 is active. |
| `MP.Protocol2.MaxFPS` | int | inherit `MP.MaxFPS` | Preset used when protocol 2 is active. |
| `MP.MinFPS` | int | `0` | FPS floor. `0` disables the floor. |
| `MP.Protocol0.MinFPS` | int | inherit `MP.MinFPS` | Floor used when protocol 0 is active. |
| `MP.Protocol2.MinFPS` | int | inherit `MP.MinFPS` | Floor used when protocol 2 is active. |
| `MP.AdaptiveFPS` | bool | `yes` | Respect the engine's adaptive lag compensation. |
| `MP.SpeedTableMode` | bool | `no` | Replace `MP.MaxFPS` and `MP.MinFPS` with the speed table. |
| `MP.SpeedTable0` | int | `60` | FPS target for the fastest engine slot. |
| `MP.SpeedTable1` | int | `45` | FPS target for slot 1. |
| `MP.SpeedTable2` | int | `30` | FPS target for slot 2. |
| `MP.SpeedTable3` | int | `20` | FPS target for slot 3. |
| `MP.SpeedTable4` | int | `15` | FPS target for slot 4. |
| `MP.SpeedTable5` | int | `12` | FPS target for slot 5. |
| `MP.SpeedTable6` | int | `10` | FPS target for the slowest engine slot. |

### `MP.MaxFPS` presets

| Value | Behaviour |
|---|---|
| `-2` | Native 60 FPS. No pacing override is applied. This is the default and the safest option. |
| `-1` | No ceiling. The loop free-runs above 60 FPS. |
| `0` | Same as `-1`. |
| `N > 0` | Target `N` FPS. `N` may be below or above 60. |

The preset is applied at session start and re-enforced every frame.

### Sub-key inheritance

`MP.Protocol0.*` and `MP.Protocol2.*` inherit the matching `MP.*` value when
their key is absent. Inheritance is resolved while parsing, so a missing
sub-key behaves exactly like the base key.

### `MP.MinFPS`

Sets a floor that engine-driven slowdowns may not cross. The speed slider and
adaptive lag compensation can reduce the FPS, but never below this value.
`0` means "no floor".

### `MP.AdaptiveFPS`

- `yes` (default): the speed slider scales the `MP.MaxFPS` target, and adaptive
  lag compensation may slow the game further so lagging players can catch up.
  This is the recommended setting for online play.
- `no`: pacing is held as steady as possible. The slider still scales the
  target, but lag compensation is ignored. Use only for testing, recording or
  streaming.

### `MP.SpeedTableMode`

When enabled, `MP.SpeedTable0` through `MP.SpeedTable6` fully replace
`MP.MaxFPS` and `MP.MinFPS`:

- Slot `0` is the fastest engine speed, slot `6` the slowest.
- Each value may be `-1` or `0` (uncapped for that slot) or `1` through `1000`
  (explicit FPS).
- Values outside the valid range fall back to the engine's native value for
  that slot and a warning is written to the log.
- The renderer cap follows `MP.SpeedTable0`.
- `MP.MaxFPS` and `MP.MinFPS` are ignored completely in this mode, including
  the `-2` kill switch. Turn the mode off to restore preset behaviour.

The default table `60, 45, 30, 20, 15, 12, 10` matches the engine's native
speeds, so enabling the mode alone changes nothing.

## Examples

### Standard protocol 2 with no pacing changes

```ini
[Settings]
Protocol=2
FrameSendRate=4
MaxAhead=-1
MP.MaxFPS=-2
MP.MinFPS=0
MP.AdaptiveFPS=yes
```

### Protocol 0 with a latency cap of 6

```ini
[Settings]
Protocol=0
PreCalcMaxAhead=4
MaxLatencyLevel=6
MP.Protocol0.MaxFPS=-2
```

### Unlock the frame rate in multiplayer

```ini
[Settings]
MP.MaxFPS=-1
MP.AdaptiveFPS=yes
```

### Explicit speed table

```ini
[Settings]
MP.SpeedTableMode=yes
MP.SpeedTable0=120
MP.SpeedTable1=60
MP.SpeedTable2=45
MP.SpeedTable3=30
MP.SpeedTable4=20
MP.SpeedTable5=15
MP.SpeedTable6=10
```
