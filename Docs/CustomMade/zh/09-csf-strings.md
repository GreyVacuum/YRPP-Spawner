# CSF 字符串表

本文介绍尤里的复仇使用的 `CSF` 字符串表，说明 Spawner 如何从字符串表读取文本，
以及可以用哪些标签定制 Spawner 的界面文字。面向需要修改显示文本、又不想重新编译
DLL 的模组作者和客户端打包者。

## 什么是 CSF

`CSF` 是 **Command & Conquer String File**（命令与征服字符串文件）的缩写，
是游戏启动时加载的二进制字符串表。`TXT_OK`、`GUI_AIHard` 这类标签在表中只存一份，
所有对话框、提示、EVA 语音文本和界面元素都从同一张表查找文字。

Spawner 不自带字符串表。它的 hook 运行时，引擎的 `StringTable` 已经加载完毕，
Spawner 只调用这张现成的表：

- `StringTable::LoadString(label)`：查找标签；标签不存在时返回 `L"MISSING:<label>"`。
- `StringTable::TryFetchString(label, fallback)`：查找标签；结果以 `MISSING:` 开头
  或为空时返回调用方给出的回退值。Spawner 使用这一形式，保证标签缺失时界面不崩。

这两个函数声明在 `YRpp/StringTable.h` 中。

## 文件与加载顺序

| 游戏版本 | 引擎加载的文件 |
|---|---|
| 尤里的复仇 | `ra2md.csf` |
| 红色警戒 2（经典模式） | `ra2.csf` |

引擎通过自己的文件系统解析该文件。游戏目录中的散装文件优先于 MIX 压缩包中的同名
文件，因此实际做法有两种：

1. 把编辑好的 `ra2md.csf` 放在 `gamemd.exe` 同级目录（启动器发行包通常通过自己的
   MIX 或散装文件完成这一步）。
2. 重新打包存放原版文件的 MIX 压缩包。

字符串表在启动时只读取一次，改动后必须重启游戏才生效。加载代码位于
`0x7346A0`（`LoadFile`）和 `0x734990`（`ReadFile`），已加载的文件名保存在
`StringTable::FileName`。

## 二进制格式

下列布局与 `YRpp/StringTable.h` 中的结构定义一致。

### 文件头（24 字节）

| 偏移 | 类型 | 字段 | 说明 |
|---|---|---|---|
| `0x00` | `uint32` | 签名 | 必须为 `0x43534620`（`" FSC"`，即 `CSF` 加一个空格）。 |
| `0x04` | `int` | 版本 | 尤里的复仇使用 `3`。 |
| `0x08` | `int` | 标签数量 | 后续标签记录的数量。 |
| `0x0C` | `int` | 数值数量 | 后续数值记录的数量。 |
| `0x10` | `int` | 未使用 | 保持 `0`。 |
| `0x14` | `int` | 语言 | `0` 美式英语，`1` 英式英语，`2` 德语，`3` 法语，`4` 西班牙语，`5` 意大利语，`6` 日语，`7` Jabberwockie，`8` 韩语，`9` 中文。 |

版本字段小于 `2` 时，引擎会强制把语言当作美式英语。

### 标签记录（签名 + 40 字节）

| 偏移 | 类型 | 字段 | 说明 |
|---|---|---|---|
| `0x00` | `uint32` | 签名 | `0x4C424C20`（`" LBL"`）。 |
| `0x04` | `char[32]` | 名称 | 标签文本，最多 31 个字符。 |
| `0x24` | `int` | 数值数量 | 该标签拥有的数值条数。 |
| `0x28` | `int` | 首个数值索引 | 在全局数值数组中的起始下标。 |

### 数值记录

| 偏移 | 类型 | 字段 | 说明 |
|---|---|---|---|
| `0x00` | `uint32` | 签名 | `0x53545220`（`" RTS"`）。`0x53545257`（`"WRTS"`）表示该值还带有附加字符串。 |
| `0x04` | `uint32` | 字符数量 | 存储的 UTF-16 码元数量，不含结尾空字符。 |
| `0x08` | `wchar_t[]` | 文本 | UTF-16LE 字符，不存储结尾空字符。 |

第一条数值记录紧跟在最后一条标签记录之后：
`0x18 + 标签数量 * 0x2C`。附加字符串（`WRTS`）排在正文之后，只有少数引擎标签
使用；本文列出的 Spawner 标签都是普通 `RTS` 数值。

## 实际需要注意的限制

- **标签长度**：31 个字符。超长名称会被截断或直接拒绝。
- **数值长度**：引擎把读到的值复制进内存中的 `CSFString`，其缓冲区为
  `wchar_t Text[102]`，因此文本应控制在 100 字符以内。过长的说明应拆分或精简。
- **数量字段**：文件头中的标签数量和数值数量必须与实际记录数一致。数量错误会导致
  整张表无法读取或被截断。
- **编码**：整个文件是二进制 UTF-16LE。把数值存成 ANSI 文本会产生乱码。

## Spawner 会读取的标签

以下标签全部可选。缺失时 Spawner 使用表中的回退值，因此未改动的原版
`ra2md.csf` 也能正常工作。

| 标签 | 缺失时的回退值 | 读取位置 | 相关设置 |
|---|---|---|---|
| `TXT_CNCNET_LATENCY_0` 到 `TXT_CNCNET_LATENCY_9` | `CnCNet: Latency mode set to: N - ...` | 协议 0 的延迟提示 | `[Settings] Protocol`、`LatencyLevel` |
| `TXT_QuickMatch_PlayerNames` | `Player` | 快速对战的玩家名替换 | `QuickMatch_ProgressScreen`、`QuickMatch_Diplomacy`、`QuickMatch_InGameDialog`、`QuickMatch_WaitForPlayers` |
| `TXT_LoadProgressFormat` | `%s %3d` | 加载界面百分比 | `ShowLoadingProgress` |
| `TXT_AUTOSAVE_SUFFIX` | `Autosave (slot %d)` | 单人自动存档描述 | `AutoSaveCount`、`AutoSaveInterval` |
| `GUI_AIHard`、`GUI_AINormal`、`GUI_AIEasy` | 引擎内置字符串 | 按难度命名的 AI 名称 | `AINamesByDifficulty` |

Spawner 自己的对话框和消息还会读取这些引擎标签：`TXT_OK`、
`TXT_UNABLE_READ_SCENARIO`、`TXT_ERROR_LOADING_GAME`、`TXT_GAME_WAS_SAVED`、
`TXT_ERROR_SAVING_GAME`、`TXT_SAVING_GAME` 和 `TXT_MULTIPLAYER_GAME`。
只有当你希望整个游戏统一更换措辞时才修改它们，否则仅影响某处界面。

## 格式化标签的占位符规则

Spawner 先把标签值交给 CRT 格式化，再由游戏绘制结果。百分号会被两套系统解释：

- **CRT 格式化**识别 `%s`、`%d`、`%3d` 和 `%%`。
- **文字渲染器**把单独的 `%` 当作控制字符并吞掉。

`TXT_LoadProgressFormat` 传入两个参数：

1. 玩家名（`%s`）；
2. 完成百分比（`%d`，或用 `%3d` 在三个字符宽度内右对齐）。

要显示字面百分号，标签里必须写成 `%%`，或者由代码追加。Spawner 在格式化后自行追加
`%%`，所以默认格式 `%s %3d` 显示为 `Name  42%`。

`TXT_AUTOSAVE_SUFFIX` 用 `%d` 接收存档槽编号，默认值已经包含该占位符。

## 编辑流程

1. 取出原版 `ra2md.csf`（来自游戏目录，或存放它的 MIX 压缩包）。
2. 用支持 CSF 的编辑器打开，例如 XCC Utilities 或同类的 CSF/MIX 工具。用普通文本
   编辑器会破坏二进制结构。
3. 修改已有数值，或按上面的表格追加新标签。
4. 保持版本字段为 `3`，语言字段不变。
5. 保存后放到引擎会在原版文件之前读取的位置，然后重启游戏。
6. 验证：打开用到该文本的对话框，或触发对应的 Spawner 功能。

建议在现有文件上修改，而不是替换成一个只有几条记录的精简文件。地图、Ares、Phobos
和客户端都依赖同一张表里的许多其他标签，精简文件会让它们出错。

## 故障排查

| 现象 | 原因 |
|---|---|
| 界面显示 `MISSING:<标签>` | 标签不存在，或拼写不一致。 |
| 游戏文本正常，Spawner 文本缺失 | 标签加到了错误的文件（`ra2.csf` 而不是 `ra2md.csf`），或加到了优先级更低的 MIX。 |
| 百分号消失 | 使用了单独的 `%`，应写成 `%%`。 |
| 文本乱码或第一个字符后被截断 | 数值被存成 ANSI 而不是 UTF-16LE，或字符数量与实际文本不一致。 |
| 改动不生效 | 没有重启游戏，或编辑后的文件被 MIX 压缩包中的副本覆盖。 |
| 整张表加载失败 | 文件头数量与实际标签、数值记录数不一致。 |
| 开启 `AINamesByDifficulty` 后仍显示旧名称 | `GUI_AIHard`、`GUI_AINormal` 或 `GUI_AIEasy` 未替换，或该开关未开启。 |

注意：字符串表属于本地数据。不同玩家使用不同的 CSF 文件不会导致不同步，只是各自
看到的文字不同。
