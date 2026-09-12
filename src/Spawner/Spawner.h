/**
*  yrpp-spawner
*
*  Copyright(C) 2022-present CnCNet
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "Spawner.Config.h"
#include <Ext/Event/Body.h>
#include <memory>

class Spawner
{
public:
	static bool Enabled;
	static bool Active;
	static bool DoSave;
	static int NextAutoSaveFrame;
	static int NextAutoSaveNumber;

private:
	static std::unique_ptr<SpawnerConfig> Config;

public:
	static SpawnerConfig* GetConfig()
	{
		return Config.get();
	}

	// Preset meaning "override nothing" (returned outside multiplayer, or when
	// no key is configured). Distinct from every valid preset (-2/-1/0/N>0).
	static constexpr int MaxFPS_NoOverride = -3;

	// Resolves the effective frame-rate preset for the current session:
	// MP.Protocol0.MaxFPS or MP.Protocol2.MaxFPS (inheriting MP.MaxFPS when
	// absent) for network sessions, MaxFPS_NoOverride for everything else
	// (single-player is left completely alone).
	static int GetEffectiveMaxFPS();

	// Resolves the effective FPS floor for the current session (used by the
	// MP.MinFPS clamp): MP.Protocol0.MinFPS or MP.Protocol2.MinFPS (inheriting
	// MP.MinFPS when absent) for network sessions, MaxFPS_NoOverride otherwise.
	// 0 = no floor.
	static int GetEffectiveMinFPS();

	// Applies a MaxFPS preset WITHOUT touching the netcode-owned engine globals
	// (RequestedFPS 0xA8B558 / PreCalcFrameRate 0xA8B570 -- pinning them caused
	// multiplayer out-of-sync and a welded in-game speed control). Levers used:
	//  1. the in-game frame-wait budget global at 0x887330 (consumed by the
	//     per-frame waiter sub_55E160; derived by the main loop from
	//     RequestedFPS): the speed slider reaches the engine as a synced
	//     GameSpeed event whose payload is written into RequestedFPS
	//     (sub_4C6CB0 @0x4C807D), and the budget is 1000/RequestedFPS.
	//     -1/0 -> free-run at the engine's own max speed, yielding whenever
	//     the engine wants to run slower (slider below 60 / lag compensation);
	//     N>0 -> PROPORTIONAL target: budget = 60000/(N * RequestedFPS), i.e.
	//     the target scales with the engine speed ratio (MaxFPS * req/60), so
	//     the 7-speed slider sweeps the range between N and MinFPS;
	//     -2 -> no pacing override and no renderer write at all (native);
	//     MP.AdaptiveFPS=no -> same proportional target, but adaptive lag
	//     compensation is deliberately IGNORED (rock-stable pacing);
	//     MP.SpeedTableMode=yes -> the MP.SpeedTable0..6 targets (0 = fastest
	//     ... 6 = slowest, defaults = native 60/45/30/20/15/12/10) replace
	//     MP.MaxFPS AND MP.MinFPS entirely; the renderer cap follows
	//     MP.SpeedTable0;
	//  2. the cnc-ddraw renderer present cap ("TargetFPS", CnCNet build only).
	// MaxFPS_NoOverride = no-op. Intentionally does NOT log (per-frame hook).
	static void ApplyMaxFPS(int maxFPS);

	static void Init();
	static bool StartGame();
	static void AssignHouses();
	static void After_Main_Loop();
	static void RespondToSaveGame();

private:
	static bool StartScenario(const char* scenarioName);
	static bool LoadSavedGame(const char* scenarioName);

	static void InitNetwork();
	static bool Reconcile_Players();
	static void LoadSidesStuff();
};
