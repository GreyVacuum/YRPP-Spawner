# Spawner 运行系统与界面

本篇介绍不由单个 INI 键控制的运行系统：存档、统计、自定义 MIX、观察者、
视频模式和界面修复。

## 存档

Spawner 将所有存档操作重定向到 `SavedGameDir`：

- 保存、加载、删除、列出存档都使用该目录。
- 支持嵌套路径，例如 `Saved Games\Yuri's Revenge`。
- 写出存档前会自动创建目录。
- `SavedGameDir` 为空时保留原版行为。

存档文件旁还会写入附加元数据：

- 记录自制任务编号，便于第三方战役过滤。
- `CustomMissionID` 为 `0` 时存档不进入自制任务过滤范围。

### 自动存档

当 `AutoSaveCount > 0` 且 `AutoSaveInterval > 0` 时，
Spawner 每隔 `AutoSaveInterval` 帧写出一个轮换存档：

- 文件名为 `AUTOSAVE1.SAV` 到 `AUTOSAVEn.SAV`。
- 下一个编号由 `NextAutoSaveNumber` 控制，达到上限后回到 0。
- 新对局开始时编号与帧计时器会重置。
- 只有对局真正开始后才会自动存档。

`ShowMPSaveGameButton` 为 `yes` 时，多人暂停菜单显示保存按钮。

## 统计

统计只写多人对局，战役中不会写出。

### `stats.dmp`

发送统计包时写出，包含原始数据包与附加字段：

- `SCEN`：`UIMapName`。
- `HASH`：`MapHash`。
- `MYID`、`NKEY`、`SKEY`：玩家身份字段。
- `ALY*`、`BSP*`：每个玩家的结盟与起始位置。

### `debug\Statistic.log`

准备计分界面时写出的可读摘要：

```
Name: Winner
 Lost = N
 Kills = N
 Built = N
 Score = N
...
Game loop finished. Average FPS = N
```

行为说明：

- 人类房屋使用显示名，AI 房屋统一写作 "Computer"。
- 被动房屋与观察者房屋被跳过，与游戏内计分界面一致。
- 分数来自填充计分界面的同一数组。
- 仅在 `GenerateStatistics=yes` 时写出。

两个文件都由 `WriteStatistics` 控制，可在 `SPAWN.INI` 或 `RA2MD.INI` 中启用。

## 自定义 MIX

CnCNet YR 构建在启动时挂载两个额外 MIX 存档：

- `cncnet.mix` 始终加载。
- `ra2mode.mix` 在 RA2 模式启用时加载。

游戏退出时会释放这两个存档。

## 随机地图

场景被标记为随机地图时，Spawner 会：

- 从场景文件读取 `[Basic] RandomMap` 标记。
- 应用配置的种子，使所有玩家生成同一张地图。
- 生成后修正起始点与单位摆放。

这样可以让随机地图在多人客户端之间保持同步。

## 观察者支持

多人对局支持观察者：

- `IsSpectator=yes` 的玩家成为观察者房屋。
- 观察者的起始点会从场景房屋列表中移除。
- `Observer.ShowAIOnSidebar=yes` 时侧栏可以列出 AI 房屋。
- `DefeatedBecomesObserver=yes` 时失败玩家可转为观察者。
- 遭遇战观察者可以改变游戏速度；多人观察者在没有存活人类玩家后可以改变速度。

## 视频与窗口模式

Spawner 启用时：

- 选项界面解锁高分辨率模式。
- 跳过分辨率检查，所有支持的 16 位模式都被接受。
- 根据所选分辨率重新计算界面布局，包括地图预览位置。
- `Video.Windowed=yes` 以窗口运行，`NoWindowFrame=yes` 移除边框。
- 加载期间不绘制菜单背景。
- Windows blit 替换为 ddraw blit，后者线程安全。

## 界面修复

### 暂停菜单与对话框

- 启用时多人暂停菜单增加保存按钮。
- Esc 可以像返回键一样关闭多个对话框。

### 雷达

- 在雷达上拖动可以移动战术摄像机。
- 雷达视图矩形被限制在雷达图像范围内，小地图不会画出越界的视图框。

### 加载界面

- `ShowLoadingProgress=yes` 时每个玩家名后显示百分比。
- 加载界面不再为未捕获鼠标时无效的光标操作等待，
  每次场景加载大约节省两秒。
- RA2 模式下窗口标题会改为 "Red Alert 2"。

### 计分界面

- `SkipScoreScreen=yes` 时立即关闭计分界面。
- Spawner 启用时跳过按钮动画。

## 输入修复

- 空格键不再关闭"等待玩家"窗口。
- Tab 键打开战役简报，输入锁定时 Tab 仍然有效。
- 游戏输入锁定时忽略 Esc 抬起事件，但影片仍可跳过。
- 连续按类型选择键不再掉帧。
- 可以关闭边缘滚屏。

## 游戏速度

`DisableGameSpeed` 可关闭游戏内速度滑杆，观察者按上文例外处理。
`[Settings] GameSpeed` 定义开局速度档位。

## No-CD 与防拷贝

- `-CD`、没有检测到光驱、或相关配置会启用 No-CD 模式。
- 防拷贝检查被中和，游戏无需原版启动器即可运行。
- 跳过 CD-ROM 文件路径，并跳过次级 MIX 检查。

## 网络接口

Spawner 使用 `GetAdaptersAddresses` 枚举本机 IPv4 接口，替代旧的
主机名查询方式。多网卡机器也能得到可靠的地址与广播地址列表。

## 其他

- `SkipCreateAppMutex=yes` 允许开启多个游戏实例。
- `QuickExit=yes` 跳过退出等待。
- `MPDebug=yes` 绘制多人调试统计。
- `AllowTaunts=no` 同时静音房屋喝彩音。
- 敞篷运输载具的乘客开火时，运输载具不会解除隐形。
- Blowfish 替代实现移除了原实现的导出限制。
- CnCNet 构建会安装应用图标与 `cncnet.fnt` 字体。
