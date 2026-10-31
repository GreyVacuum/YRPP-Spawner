# SPAWN.INI：玩家与房屋

一局最多八个槽位，每个槽位由两个独立配置块描述：

- **玩家配置**：决定该槽位是否为人类玩家，以及名称、颜色、国家、观察者标记与网络地址。
- **房屋配置**：补充生成的 `HouseClass`：起始点、资金倍率、难度与结盟。

两个配置块使用不同的段名，所以一个槽位的信息分散在多个段中。

## 玩家段

| 段 | 槽位 |
|---|---|
| `[Settings]` | 玩家 1 |
| `[Other1]` | 玩家 2 |
| `[Other2]` | 玩家 3 |
| `[Other3]` | 玩家 4 |
| `[Other4]` | 玩家 5 |
| `[Other5]` | 玩家 6 |
| `[Other6]` | 玩家 7 |
| `[Other7]` | 玩家 8 |

`[Settings]` 同时也是主选项段，可以同时存放
[03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md) 中的模式键
和下面玩家 1 的键。

### 段存在即代表人类玩家

槽位所在段存在时，该槽位成为人类玩家：

- `[Settings]` 始终存在，因此玩家 1 始终按人类处理。
- `[OtherN]` 存在时创建对应槽位的人类玩家。
- `[OtherN]` 不存在时，该槽位由 `[HouseColors]`、`[HouseCountries]`、
  `[HouseHandicaps]` 中的默认值作为 AI 房屋填充。

玩家段存在时，其 `Difficulty` 会先被重置为 `-1`，再考虑默认难度键。

### 玩家键

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `Name` | string (UTF-8) | 空 | 显示名称，存放在 20 字符宽的缓冲区中。 |
| `Color` | int | `-1` | 房屋颜色索引。`-1` 表示使用槽位默认值。 |
| `Side` | int | `-1` | 国家索引。INI 键名是 `Side`，内部字段名是 `Country`。 |
| `IsSpectator` | bool | `no` | 以观察者身份开始。 |
| `Ip` | string | `0.0.0.0` | 玩家的网络地址。 |
| `Port` | int | `-1` | 玩家的网络端口。 |

## AI 槽位默认值

槽位没有玩家段时，由以下段提供 AI 房屋属性，槽位标签为 `Multi1` 到 `Multi8`。

| 段 | 键 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `[HouseColors]` | `MultiN` | int | `-1` | AI 房屋颜色索引。 |
| `[HouseCountries]` | `MultiN` | int | `-1` | AI 房屋国家索引。 |
| `[HouseHandicaps]` | `MultiN` | int | `-1` | AI 房屋难度。 |

示例：

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

## 房屋段

这些段对人类与 AI 槽位都有效，槽位标签同样是 `Multi1` 到 `Multi8`。

| 段 | 键 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `[IsSpectator]` | `MultiN` | bool | `no` | 将该房屋标记为观察者房屋。 |
| `[SpawnLocations]` | `MultiN` | int | `-2` | 强制起始点。`-2` 表示由游戏分配。 |
| `[CreditsFactor]` | `MultiN` | float | `1.0` | 初始资金倍率。 |
| `[HandicapDifficulty]` | `MultiN` | int | `-1` | 每个房屋的难度修正。`-1` 保持默认。 |

`CreditsFactor` 是浮点数，叠加在 `[Settings] Credits` 之上。例如
`CreditsFactor=1.5` 且 `Credits=10000`，该房屋开局拥有 15000 资金。

## 结盟

每个槽位有独立的结盟段 `[MultiN_Alliances]`，`N` 为 1 到 8：

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

| 键 | 类型 | 默认 |
|---|---|---|
| `HouseAllyOne` | int | `-1` |
| `HouseAllyTwo` | int | `-1` |
| `HouseAllyThree` | int | `-1` |
| `HouseAllyFour` | int | `-1` |
| `HouseAllyFive` | int | `-1` |
| `HouseAllySix` | int | `-1` |
| `HouseAllySeven` | int | `-1` |
| `HouseAllyEight` | int | `-1` |

`-1` 表示"不强制结盟"，正数为房屋索引。这些结盟在房屋创建后应用，
与游戏模式自身的队伍分配配合工作。

合作模式还有两个自动结盟开关：

- `CooperativeAIAutoAlly`：让所有 AI 房屋自动结盟。
- `CooperativePlayerAutoAlly`：让所有人类玩家自动结盟。

见 [03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md)。

## 完整示例

```ini
[Settings]
Name=玩家一
Color=0
Side=0
Ip=0.0.0.0
Port=-1

[Other1]
Name=玩家二
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

## 注意事项

- 内部槽位索引从 0 开始，`Multi1` 到 `Multi8` 对应内部索引 0 到 7。
- `Side` 的值是国家索引，不是显示名。可用 `-DumpTypes` 或
  `DumpRulesTypes.ini` 查看 `[Countries]` 的索引。
- 名称超过缓冲区长度时只保留前 19 个字符。
- 生成统计时会跳过观察者房屋。
