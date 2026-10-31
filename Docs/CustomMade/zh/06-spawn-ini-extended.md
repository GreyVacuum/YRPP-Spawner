# SPAWN.INI：扩展选项

以下 `[Settings]` 键用于开启不属于游戏模式、玩家或网络分组的可选行为。

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `Ra2Mode` | bool | `no` | 启用红警 2 兼容模式。 |
| `DisableGameSpeed` | bool | `no` | 移除游戏内选项中的速度控制。 |
| `QuickMatch` | bool | `no` | 启用快速匹配的玩家名隐藏功能。 |
| `QuickMatch.ProgressScreen` | bool | 继承 `QuickMatch` | 在加载进度界面隐藏玩家名。 |
| `QuickMatch.Diplomacy` | bool | 继承 `QuickMatch` | 在外交界面隐藏玩家名。 |
| `QuickMatch.InGameDialog` | bool | 继承 `QuickMatch` | 在游戏内玩家列表隐藏玩家名。 |
| `QuickMatch.WaitForPlayers` | bool | 继承 `QuickMatch` | 在等待玩家界面隐藏玩家名。 |
| `SkipScoreScreen` | bool | 继承 `RA2MD.INI` | 跳过多人计分界面。 |
| `WriteStatistics` | bool | `no` | 启用统计收集。 |
| `GenerateStatistics` | bool | `yes` | 写出 `debug\Statistic.log` 摘要。 |
| `AINamesByDifficulty` | bool | `no` | 按难度命名 AI 房屋，而不是统一叫 "Computer"。 |
| `ContinueWithoutHumans` | bool | `no` | 没有人类玩家时仍继续对局。 |
| `DefeatedBecomesObserver` | bool | `no` | 失败玩家转为观察者，而不是结束本局。 |
| `NoneEnemySwitchTeamCivilWarMode` | bool | `no` | 没有敌人时进入内战模式。 |
| `Observer.ShowAIOnSidebar` | bool | `no` | 观察者侧栏显示 AI 房屋。 |
| `ShowLoadingProgress` | bool | `no` | 在加载界面显示每个玩家的加载百分比。 |
| `DisableChat` | bool | `no` | 关闭全部游戏内聊天。 |

## RA2 模式

`Ra2Mode=yes` 应用红警 2 兼容补丁。以下任一条件也会激活：

- `LoadSaveGame=yes`，且存档版本等于 `-RA2ModeSaveID=` 传入的值。
- `IsSinglePlayer=yes`，且 `Scenario` 以 `RA2->` 开头。

补丁内容：

- 窗口标题改为 "Red Alert 2"。
- 允许用类型选择键选中建筑。
- 允许选中建筑内部或上方的单位。
- 允许警犬与防空步兵获得速度加成。
- 放宽"三个基础单位"的假设。
- 加载界面不显示小地图。
- 让心灵感应器可以探测伪装单位。
- 强制 `MaxControlNodes` 为 1。
- 界面使用经典边框样式。
- 跳过 `EXPANDMD*.MIX`、`CAMEOMD.MIX` 与 `MOVMD03.MIX`。
- 交换 `LOCALMD.MIX` 与 `LOADMD.MIX` 的优先级。
- 读取 RA2 文件名（`AUDIO.MIX`、`THEME.INI`、`MPModes.ini`、`MISSION.INI` 等），
  而不是 YR 版本。
- 允许盟军维修厂维修 RA2 单位与黑鹰。
- 让矿石精炼厂像 RA2 一样需要电力。
- 判断房屋是否被消灭时，同时检查盟军运输机内的存活步兵。

## `DisableGameSpeed`

为 `yes` 时，对所有玩家禁用游戏选项中的速度滑杆。
为 `no` 时，Spawner 按以下条件允许观察者与失败玩家控制速度：

- 遭遇战观察者始终可以改变速度。
- 多人对局中，没有存活人类玩家时，失败的人类玩家可以改变速度。

该键只影响界面，不改变 `[Settings] GameSpeed`。

## 快速匹配的玩家名隐藏

快速匹配在界面中隐藏玩家身份。四个子键分别控制一个界面，
自身键缺失时全部继承 `QuickMatch`。

名称被替换为 CSF 字符串 `TXT_QuickMatch_PlayerNames`，字符串缺失时默认为 "Player"。

## 统计

统计只用于多人对局，战役中不会收集。

- `WriteStatistics=yes` 在发送统计包时写出 `stats.dmp`，
  并附加字段 `SCEN`、`HASH`、`MYID`、`NKEY`、`SKEY`、`ALY*`、`BSP*`。
- `GenerateStatistics=yes` 在对局结束时写出可读的 `debug\Statistic.log` 摘要。
- `WriteStatistics` 也可以由 `RA2MD.INI [Debug] WriteStatistics` 开启，
  两个文件任一启用即可。
- 建议填写 `[Settings] UIMapName` 与 `MapHash`，
  让报告中的场景名与哈希有意义。

文件格式见 [07-spawner-systems.md](07-spawner-systems.md)。

## `AINamesByDifficulty`

为 `yes` 时，AI 房屋由内部的 "Computer" 改名为本地化难度名：

| 难度值 | 名称 |
|---|---|
| `0` | 困难 AI |
| `1` | 普通 AI |
| `2` | 简单 AI |

其他值保留原名。

## 失败处理

三个键用于改变玩家或 AI 被消灭后的行为。

### `ContinueWithoutHumans`

只要还有其他房屋存活，即使没有人类玩家也继续对局。
适合专用服务器，或观看纯 AI 对局。

### `DefeatedBecomesObserver`

失败房屋转为观察者，而不是从对局移除。玩家可以继续观战并使用观察者界面。

### `NoneEnemySwitchTeamCivilWarMode`

当原版判定"已无敌人"时，所有存活的非被动房屋互相敌对，对局进入内战。
该键要求至少两个存活房屋。

内战模式下，触发判定的房屋仍会受到 `DefeatedBecomesObserver` 影响。

## `Observer.ShowAIOnSidebar`

默认情况下，观察者侧栏只列出人类房屋。设为 `yes` 后，非被动 AI 房屋也会列出。
被动房屋与观察者房屋始终隐藏。

## `ShowLoadingProgress`

在加载界面每个玩家名后显示百分比。文本格式读取 CSF 标签
`TXT_LoadProgressFormat`，默认 `%s %3d`。Spawner 在格式化后追加百分号。

标签缺失时使用默认格式。

## `DisableChat`

关闭整局聊天：

- 发出的聊天不会发送，发送者最多每 90 帧看到一次
  "Chat is disabled. Message not sent."。
- 收到的消息会被抑制，提示音也跳过。
- 外交界面的聊天复选框强制关闭并设为只读。
- 每帧重置聊天掩码，防止从结盟菜单重新开启。

该键与 `RA2MD.INI [Options] AllowChat` 相互独立。后者只在开局时清空一次聊天掩码，
`DisableChat` 会在整局持续强制执行。

## 推荐基线

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
