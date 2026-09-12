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
	// Multiplayer.Protocol0.MaxFPS or Multiplayer.Protocol2.MaxFPS (inheriting
	// Multiplayer.MaxFPS when absent) for network sessions, MaxFPS_NoOverride
	// for everything else (single-player is left completely alone).
	static int GetEffectiveMaxFPS();

	// Applies a MaxFPS preset WITHOUT touching the netcode-owned engine globals
	// (RequestedFPS 0xA8B558 / PreCalcFrameRate 0xA8B570 -- pinning them caused
	// multiplayer out-of-sync and a welded in-game speed control). Levers used:
	//  1. the in-game frame-wait budget global at 0x887330 (consumed by the
	//     per-frame waiter sub_55E160; derived by the main loop from
	//     RequestedFPS): -1/0 -> 0 (loop free-runs, uncapped), N>0 -> clamped
	//     to at least 1000/N so the speed slider can still slow below the cap,
	//     -2 -> left at the engine-derived value (native 60);
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
