# SPAWN.INI：网络与帧率

本篇介绍 `[Settings]` 中的网络键、`[Tunnel]` 段，以及 `MP.*` 帧率控制系统。

所有帧率键只影响多人对局。战役与遭遇战保持引擎原有节奏。

## 网络键

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `Protocol` | int | `2` | 网络协议。`2` 为标准协议，`0` 启用动态延迟协议。 |
| `FrameSendRate` | int | `4` | 网络发送间隔帧数，仅协议 2 使用。协议 0 会强制设为 `2`。 |
| `ReconnectTimeout` | int | `2400` | 传给网络层的重连超时。 |
| `ConnTimeout` | int | `3600` | 写入场景加载等待的超时值。 |
| `MaxAhead` | int | `-1` | 最大预读帧数。`-1` 表示 `FrameSendRate * 6`。 |
| `PreCalcMaxAhead` | int | `0` | 协议 0 的初始预读值。协议 2 忽略。 |
| `MaxLatencyLevel` | int | `255` | 协议 0 允许达到的最高延迟等级，最终限制在 1 到 9。默认 `255` 实际等同 9。 |
| `ForceMultiplayer` | bool | `no` | 强制使用多人对局类型。`RA2MD.INI [Debug] ForceMultiplayer` 也能启用。 |

## 协议选择

### 协议 2（默认）

标准同步协议。`FrameSendRate` 按配置使用，`MaxAhead` 默认等于
`FrameSendRate * 6`。

### 协议 0

加入动态延迟适配。启用后：

- `FrameSendRate` 强制为 `2`。
- 使用 `PreCalcMaxAhead` 作为初始值。
- 有玩家落后时，延迟等级系统会提高预读帧数。

延迟等级为 0 到 9，对应的预读表如下：

| 等级 | MaxAhead |
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

`MaxLatencyLevel` 限制适配能达到的最高等级。等级变化会在游戏内通过
`TXT_CNCNET_LATENCY_0` 到 `TXT_CNCNET_LATENCY_9` CSF 字符串提示。
缺少字符串时使用内置英文回退文本。

## 隧道键

隧道给每个发出的数据包加上 4 字节头，让中继服务器在 NAT 环境中转发玩家流量。

| 段 | 键 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `[Settings]` | `Port` | int | `1234` / `0` | 在 `[Settings]` 中，该键同时初始化 `ListenPort`（默认 `1234`）与 `TunnelId`（默认 `0`）。填写后两者都等于该值。 |
| `[Tunnel]` | `Ip` | string | `0.0.0.0` | 隧道服务器地址。 |
| `[Tunnel]` | `Port` | int | `0` | 隧道服务器端口。`0` 表示关闭隧道。 |

`[Tunnel] Port` 非零时：

- 发出的数据包被送到隧道服务器，头部包含玩家 ID 与原目标端口。
- 收到的数据包被还原到对应玩家。
- 强制端口匹配，UDP 监听端口不再使用。

隧道关闭时，数据包直接发送到配置的玩家地址。

## 帧率控制

`MP.*` 键调整游戏内帧节奏。实现方式是写入引擎每帧的等待预算。
网络全局变量 `RequestedFPS` 与 `PreCalcFrameRate` 不会被固定，
因为固定它们会造成多人不同步，并锁死游戏内速度滑杆。

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `MP.MaxFPS` | int | `-2` | 多人基础预设。 |
| `MP.Protocol0.MaxFPS` | int | 继承 `MP.MaxFPS` | 协议 0 使用的预设。 |
| `MP.Protocol2.MaxFPS` | int | 继承 `MP.MaxFPS` | 协议 2 使用的预设。 |
| `MP.MinFPS` | int | `0` | 帧率下限。`0` 表示无下限。 |
| `MP.Protocol0.MinFPS` | int | 继承 `MP.MinFPS` | 协议 0 使用的下限。 |
| `MP.Protocol2.MinFPS` | int | 继承 `MP.MinFPS` | 协议 2 使用的下限。 |
| `MP.AdaptiveFPS` | bool | `yes` | 尊重引擎的滞后补偿。 |
| `MP.SpeedTableMode` | bool | `no` | 用速度表完全替换 `MP.MaxFPS` 与 `MP.MinFPS`。 |
| `MP.SpeedTable0` | int | `60` | 最快档位的目标帧率。 |
| `MP.SpeedTable1` | int | `45` | 档位 1 的目标帧率。 |
| `MP.SpeedTable2` | int | `30` | 档位 2 的目标帧率。 |
| `MP.SpeedTable3` | int | `20` | 档位 3 的目标帧率。 |
| `MP.SpeedTable4` | int | `15` | 档位 4 的目标帧率。 |
| `MP.SpeedTable5` | int | `12` | 档位 5 的目标帧率。 |
| `MP.SpeedTable6` | int | `10` | 最慢档位的目标帧率。 |

### `MP.MaxFPS` 预设

| 值 | 行为 |
|---|---|
| `-2` | 原生 60 FPS，不加节奏覆盖。默认值，也是最稳妥的选项。 |
| `-1` | 不设上限，循环可超过 60 FPS。 |
| `0` | 与 `-1` 相同。 |
| `N > 0` | 目标 `N` FPS，`N` 可以低于或高于 60。 |

预设在对局开始时应用，并在每一帧重新强制执行。

### 子键继承

`MP.Protocol0.*` 与 `MP.Protocol2.*` 在自身键缺失时继承对应的 `MP.*` 值。
继承在解析阶段完成，因此缺失的子键与基础键完全等价。

### `MP.MinFPS`

设置引擎减速不可越过的下限。速度滑杆与滞后补偿可以降低帧率，
但不会低于该值。`0` 表示不设下限。

### `MP.AdaptiveFPS`

- `yes`（默认）：速度滑杆按比例缩放 `MP.MaxFPS` 目标，滞后补偿可以进一步减速，
  让落后玩家追上。这是联机推荐设置。
- `no`：节奏尽可能稳定。滑杆仍会缩放目标，但忽略滞后补偿。
  仅建议在测试、录制或直播时使用。

### `MP.SpeedTableMode`

启用后，`MP.SpeedTable0` 到 `MP.SpeedTable6` 完全替换 `MP.MaxFPS` 与 `MP.MinFPS`：

- 档位 `0` 为最快，档位 `6` 为最慢。
- 每项可为 `-1` 或 `0`（该档不设上限），或 `1` 到 `1000`（显式帧率）。
- 超出范围的值会回退到该档的引擎原生值，并在日志中记录警告。
- 渲染上限跟随 `MP.SpeedTable0`。
- 此模式下 `MP.MaxFPS` 与 `MP.MinFPS` 被完全忽略，包括 `-2` 关闭开关。
  要恢复预设行为，请关闭该模式。

默认表 `60, 45, 30, 20, 15, 12, 10` 与引擎原生速度一致，
因此仅开启该模式不会产生变化。

## 示例

### 标准协议 2，不修改节奏

```ini
[Settings]
Protocol=2
FrameSendRate=4
MaxAhead=-1
MP.MaxFPS=-2
MP.MinFPS=0
MP.AdaptiveFPS=yes
```

### 协议 0，延迟上限 6

```ini
[Settings]
Protocol=0
PreCalcMaxAhead=4
MaxLatencyLevel=6
MP.Protocol0.MaxFPS=-2
```

### 解除多人帧率上限

```ini
[Settings]
MP.MaxFPS=-1
MP.AdaptiveFPS=yes
```

### 显式速度表

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
