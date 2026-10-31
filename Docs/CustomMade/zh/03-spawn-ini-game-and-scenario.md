# SPAWN.INI：游戏模式与场景

`SPAWN.INI` 在创建对局时由 Spawner 读取。本篇介绍 `[Settings]` 中决定
"怎么打"以及"打哪张图"的键。

玩家槽位与房屋细节单独成篇：
[04-spawn-ini-players-and-houses.md](04-spawn-ini-players-and-houses.md)。

## 游戏模式键

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `GameMode` | int | `1` | 多人游戏模式列表的索引。列表来自 `MPModesMD.ini`（RA2 模式为 `MPModes.ini`）。所选模式决定目标、队伍规则与可用 AI 槽位。 |
| `Bases` | bool | `yes` | 开局给予建造厂与 MCV。 |
| `Credits` | int | `10000` | 初始资金。 |
| `BridgeDestroy` | bool | `yes` | 允许桥梁被摧毁。 |
| `Crates` | bool | `no` | 启用木箱。 |
| `ShortGame` | bool | `no` | 玩家失去基地时结束对局。 |
| `Superweapons` | bool | `yes` | 启用超级武器。注意键名拼写为 `Superweapons`。 |
| `BuildOffAlly` | bool | `no` | 允许在盟友建筑上建造。 |
| `GameSpeed` | int | `0` | 引擎速度索引。`0` 为最快原始档，`6` 为最慢。 |
| `MultiEngineer` | bool | `no` | 允许多个工程师占领同一建筑。为 `no` 时 Spawner 将 `EngineerCaptureLevel` 设为 1.0。 |
| `UnitCount` | int | `0` | 单位数量选项，含义由所选游戏模式定义。 |
| `AIPlayers` | int | `0` | 模式应填充的 AI 对手数量。 |
| `AIDifficulty` | int | `1` | 生成的 AI 房屋默认难度。 |
| `AlliesAllowed` | bool | `no` | 允许玩家在局内更改结盟。 |
| `HarvesterTruce` | bool | `no` | 采矿车之间互不攻击。 |
| `FogOfWar` | bool | `no` | 启用战争迷雾。 |
| `MCVRedeploy` | bool | `yes` | 允许 MCV 重新部署为建造厂。 |
| `SpecialHouseIsAlly` | bool | `yes` | 将特殊（中立）房屋视为与所有玩家结盟。 |
| `UIGameMode` | string (UTF-8) | 空 | 外交界面显示的模式名，替代 INI 中的模式名。 |
| `CooperativeDescription` | bool | `no` | 用合作模式说明替换游戏模式说明。 |
| `CooperativeAIAutoAlly` | bool | `no` | 合作模式下自动让所有 AI 房屋结盟。 |
| `CooperativePlayerAutoAlly` | bool | `no` | 合作模式下自动让所有人类玩家结盟。 |

`GameSpeed` 是引擎索引，不是帧率。帧率覆盖使用 `MP.*` 键，
见 [05-spawn-ini-network.md](05-spawn-ini-network.md)。

## 存档键

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `LoadSaveGame` | bool | `no` | 加载 `SaveGameName`，而不是开始新场景。 |
| `SavedGameDir` | string | `Saved Games` | 存档目录。允许嵌套路径，例如 `Saved Games\Yuri's Revenge`。 |
| `SaveGameName` | string | 空 | 要加载的存档文件名。 |
| `CustomMissionID` | int | `0` | 加载自制战役任务时使用的任务编号。 |
| `AutoSaveCount` | int | `5` | 轮换存档 `AUTOSAVE*.SAV` 的数量。`0` 表示关闭自动存档。 |
| `AutoSaveInterval` | int | `7200` | 两次自动存档之间的帧数。默认节奏下约两分钟。`0` 表示关闭自动存档。 |
| `NextAutoSaveNumber` | int | `0` | 下一个自动存档的槽位编号，达到 `AutoSaveCount` 后回到 0。 |
| `ShowMPSaveGameButton` | bool | `yes` | 在多人暂停菜单显示保存按钮。 |

只有同时满足 `AutoSaveCount > 0`、`AutoSaveInterval > 0` 且对局已开始时，
自动存档才会生效。文件名从 `AUTOSAVE1.SAV` 到 `AUTOSAVEn.SAV`，
其中 `n` 等于 `AutoSaveCount`。

## 场景键

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `Seed` | int | `0` | 随机种子。`0` 表示由游戏自行选择。 |
| `TechLevel` | int | `10` | 场景科技等级。 |
| `IsSinglePlayer` | bool | `no` | 标记为战役或单人任务。内部字段名为 `IsCampaign`。 |
| `Tournament` | int | `0` | 锦标赛模式索引。 |
| `GameID` | int | `0xDEADBEEF` | 用于 WOL 与统计的对局标识。 |
| `Scenario` | string | `spawnmap.ini` | 要加载的场景文件。支持两种前缀，见下文。 |
| `MapHash` | string | 空 | 统计中报告的地图哈希。 |
| `UIMapName` | string (UTF-8) | 空 | 界面显示的地图名，也是统计中的 `SCEN` 字段。 |
| `ReadMissionSection` | bool | `no` | 从 `SPAWN.INI` 读取 `[Mission]` 段，而不是场景文件。 |
| `ReadCoopCampSection` | bool | `no` | 从 `SPAWN.INI` 读取合作战役段，而不是场景文件。 |

### `Scenario` 前缀

`Scenario` 支持两种特殊前缀，在使用文件名之前被剥离：

- `RA2-><名称>`：剥离前缀并启用 RA2 模式。
- `PlayMovies-><名称>`：剥离前缀，并在场景开始前播放列出的影片。
  多个影片名用解析器的标准分隔符分隔。

```
Scenario=RA2->spawnmap.ini
Scenario=PlayMovies->intro.vqa,logo.vqa
```

启用 `WriteStatistics` 或 `GenerateStatistics` 时，建议同时填写 `MapHash`
与 `UIMapName`，统计包会报告这两个值。

## 示例

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

UIGameMode=标准模式
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
