# 命令行参数

Spawner 在游戏窗口创建之前很早就解析命令行，参数名不区分大小写。

## 支持的参数

| 参数 | 类型 | 作用 |
|---|---|---|
| `-SPAWN` | 开关 | 标记本进程为 Spawner 对局。 |
| `-CD` | 开关 | 强制启用 No-CD 模式。 |
| `-DumpTypes` | 开关 | 将已加载的类型列表写入 `DumpRulesTypes.ini`。 |
| `-RA2ModeSaveID=<数字>` | 值 | 定义会激活 RA2 模式的存档版本号。 |

## `-SPAWN`

`-SPAWN` 是总开关。

- 在 **CnCNet YR** 与 **Hardened** 构建中必须提供。缺少该参数时，游戏会弹出提示框，
  并以 `ERROR_BAD_ARGUMENTS` 退出。
- 在 **开发版** 中该参数可选。不带它时 `Spawner::Enabled` 为 false，
  Spawner 以直通模式运行，保留原版单人游戏行为。

启动器会自动添加 `-SPAWN`。从快捷方式手动测试游戏时需要自己添加。

```
GameMD.exe -SPAWN
```

## `-CD`

即使检测到光驱，也强制启用 No-CD 模式。没有检测到光驱时，No-CD 模式也会自动启用。

变化如下：

- 不再从 CD-ROM 光驱读取文件。
- 跳过次级 MIX 文件检查。

```
GameMD.exe -SPAWN -CD
```

## `-DumpTypes`

在游戏目录写出 `DumpRulesTypes.ini`。文件按 1 开始编号，列出游戏已加载的全部类型，
包含以下段：

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

每行格式为 `编号=ID`，当 ID 与显示名不同时追加 `;显示名`。
该功能适合校验修改后的 rules 文件，也适合构建 AI 或国家索引列表。

也可以在 `RA2MD.INI` 中设置 `[Debug] DumpTypes=yes`，无需命令行参数。

## `-RA2ModeSaveID=<数字>`

定义一个存档格式版本号。当加载的存档版本等于该值时，Spawner 应用 RA2 模式兼容补丁。

数字支持十进制或带 `0x` 前缀的输入。

```
GameMD.exe -SPAWN -RA2ModeSaveID=0x1234
```

以下任一条件也会激活 RA2 模式：

- `SPAWN.INI` 中 `[Settings] Ra2Mode=yes`。
- `[Settings] Scenario` 以 `RA2->` 前缀开头。

完整触发条件见 [06-spawn-ini-extended.md](06-spawn-ini-extended.md)。

## 组合使用

各参数互相独立，顺序任意：

```
GameMD.exe -SPAWN -CD -DumpTypes -RA2ModeSaveID=1234
```

Spawner 会忽略无法识别的参数，它们留给游戏本体或其他 Syringe 插件处理。
