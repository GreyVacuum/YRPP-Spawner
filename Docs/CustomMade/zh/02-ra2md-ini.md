# RA2MD.INI 参考

`RA2MD.INI` 是游戏本体的设置文件。Spawner 从中读取三个段，并在启动时应用，
时机早于 `SPAWN.INI` 的读取。

下表所有键都是可选的。键不存在时使用表中的默认值。

## `[Options]`

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `AllowChat` | bool | `yes` | 为 `no` 时清空所有玩家的聊天掩码，关闭整局聊天。 |
| `AllowTaunts` | bool | `yes` | 写入 `SessionClass::LANTaunts`。为 `no` 时除了嘲讽音效，房屋喝彩音也会静音。 |
| `DDrawHandlesClose` | bool | `no` | Spawner 会把取反后的值写入 `ddraw.dll` 包装层的 `GameHandlesClose` 导出。默认 `no` 时由包装层处理窗口关闭，`yes` 时由游戏处理。 |
| `DisableEdgeScrolling` | bool | `no` | 关闭鼠标贴边滚屏。 |
| `QuickExit` | bool | `no` | 跳过正常的退出等待。适合专用服务器或自动化环境。 |
| `SingleProcAffinity` | bool | `yes` | 将进程绑定到第一个 CPU 核心，旧引擎更稳定，因此这是安全默认值。 |
| `SkipScoreScreen` | bool | `no` | 对局结束后跳过多人计分界面。 |
| `SpeedControl` | bool | `no` | 启用引擎自带的游戏内速度控制全局变量。 |
| `MPDEBUG` | bool | `no` | `[Debug] MPDebug` 的旧别名。`[Debug]` 中的键优先。 |

## `[Video]`

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `DDrawTargetFPS` | int | `-1` | 传给 `ddraw.dll` 包装层的目标帧率。`-1` 表示不修改。 |
| `NoWindowFrame` | bool | `no` | 窗口模式下移除窗口边框。仅在 `Video.Windowed=yes` 时生效。 |
| `Video.Windowed` | bool | `no` | 以窗口模式运行，而不是全屏。 |

`NoWindowFrame` 通过修改主窗口样式常量实现，未启用窗口模式时不起作用。

## `[Debug]`

| 键 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `DumpTypes` | bool | `no` | 写出 `DumpRulesTypes.ini`，等同于 `-DumpTypes` 参数。 |
| `ForceMultiplayer` | bool | `no` | 即使以单人方式启动，也强制使用多人对局行为。 |
| `MPDebug` | bool | `no` | 启用多人调试与同步统计。 |
| `MPDebug.Show` | bool | `yes` | 绘制屏幕上的多人调试信息。仅在 `MPDebug=yes` 时使用。 |
| `SkipCreateAppMutex` | bool | `no` | 跳过单实例互斥检查，允许开启多个游戏实例。 |
| `WriteStatistics` | bool | `no` | 启用统计收集。`SPAWN.INI` 中的同名键也能启用。 |

## 优先级与交互

- `WriteStatistics` 在 `RA2MD.INI` 与 `SPAWN.INI` 之间取"或"关系，任一文件启用即可。
- `SPAWN.INI` 的 `SkipScoreScreen` 默认继承 `RA2MD.INI [Options] SkipScoreScreen`，
  因此全局设置可以被对局配置沿用。
- `[Options] MPDEBUG` 仅为兼容用途，建议使用 `[Debug] MPDebug`。
- 命令行 `-CD` 与 `-DumpTypes` 分别等价于 `NoCD` 与 `DumpTypes`。
  命令行在 INI 应用之前解析，因此命令行优先。

## 示例

```ini
[Options]
AllowChat=yes
AllowTaunts=no
DDrawHandlesClose=yes
DisableEdgeScrolling=yes
QuickExit=no
SingleProcAffinity=yes
SkipScoreScreen=no
SpeedControl=yes

[Video]
DDrawTargetFPS=60
NoWindowFrame=yes
Video.Windowed=yes

[Debug]
DumpTypes=no
ForceMultiplayer=no
MPDebug=no
MPDebug.Show=yes
SkipCreateAppMutex=no
WriteStatistics=no
```
