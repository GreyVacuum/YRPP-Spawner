# CnCNet Spawner 说明文档

本目录介绍 CnCNet Spawner：它是一个与 Syringe 兼容的尤里的复仇扩展 DLL，
用于为启动器驱动的多人游戏准备对局、网络参数和界面。

文档按"配置来源"拆分。几乎所有设置只属于两个文件之一，因此可以只读你需要的那一篇。

## 配置来源

| 来源 | 文件 | 读取方 | 作用范围 |
|---|---|---|---|
| 游戏选项 | `RA2MD.INI` | 游戏本体，Spawner 在启动时读取 | 聊天、嘲讽音效、视频、窗口、调试、统计 |
| 对局选项 | `SPAWN.INI` | 仅 Spawner | 游戏模式、场景、玩家、房屋、网络、扩展功能 |

两个文件都是普通 INI 文件，只在开局前读取一次。未出现的键保持表中给出的默认值。

## 文档索引

| 文档 | 内容 |
|---|---|
| [01-command-line-arguments.md](01-command-line-arguments.md) | Spawner 支持的命令行参数 |
| [02-ra2md-ini.md](02-ra2md-ini.md) | Spawner 读取的全部 `RA2MD.INI` 键 |
| [03-spawn-ini-game-and-scenario.md](03-spawn-ini-game-and-scenario.md) | `SPAWN.INI` 游戏模式、存档与场景键 |
| [04-spawn-ini-players-and-houses.md](04-spawn-ini-players-and-houses.md) | `SPAWN.INI` 玩家段、房屋段、结盟与 AI 槽位 |
| [05-spawn-ini-network.md](05-spawn-ini-network.md) | `SPAWN.INI` 协议、延迟、隧道与帧率控制 |
| [06-spawn-ini-extended.md](06-spawn-ini-extended.md) | `SPAWN.INI` 扩展开关：RA2 模式、聊天、观察者、统计 |
| [07-spawner-systems.md](07-spawner-systems.md) | 存档、统计、随机地图、自定义 MIX、界面系统 |
| [08-fixed-bugs.md](08-fixed-bugs.md) | Spawner 修复的崩溃、不同步与输入问题 |
| [09-csf-strings.md](09-csf-strings.md) | CSF 字符串表格式与 Spawner 读取的标签 |

## 文档约定

- **默认值**：键不存在或为空时使用的值。
- **类型**：解析器期望的 INI 类型：
  - `bool` 接受 `yes`/`no`、`true`/`false`、`1`/`0`。
  - `int` 为十进制或带 `0x` 前缀的数字。
  - `float` 为小数。
  - `string` 为原始文本，名称与界面字符串按 UTF-8 读取。
- 游戏 INI 解析器不区分键名大小写，但本文统一使用源码中的标准写法。
- 除特别说明外，同一个段内的键顺序不影响结果。

## Spawner 的启动流程

1. 游戏读取 `RA2MD.INI`，Spawner 应用静态选项（CPU 亲和性、窗口模式、聊天掩码、`ddraw.dll` 设置）。
2. 创建对局时，Spawner 读取 `SPAWN.INI`。
3. 配置好的玩家、房屋、场景与网络模式被写入游戏对局。
4. 场景开始。Spawner 继续运行自动存档、统计、观察者和界面相关 hook，直到游戏退出。

## 应该修改哪个文件

- 先判断选项必须在开局前存在（路径、玩家槽位、协议），还是属于客户端或玩家偏好（聊天、窗口、视频、调试日志）。
- 对局内容写入 `SPAWN.INI`，个人与机器偏好写入 `RA2MD.INI`。
- 启动器通常会生成这两个文件。只有在直接启动游戏，或制作固定发行包时才需要手动编辑。

## 版本区别

Spawner 有三种构建：

- **CnCNet YR / Hardened**：必须带 `-SPAWN`，否则游戏弹出提示并退出。
- **开发版（DevBuild）**：`-SPAWN` 可选。不带该参数时 Spawner 保持禁用，保留原版游戏行为。

详见 [01-command-line-arguments.md](01-command-line-arguments.md)。

## 版本说明

本文描述本仓库的源码。如果使用其他分支或旧版本构建，请先对照
`src/Main.Config.cpp` 与 `src/Spawner/Spawner.Config.cpp` 中的键名，再依赖某个设置。
