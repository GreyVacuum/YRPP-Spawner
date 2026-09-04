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

	// Resolves the effective frame-rate cap preset for the current session by
	// game mode: Multiplayer.Protocol0.MaxFPS or Multiplayer.Protocol2.MaxFPS
	// (inheriting Multiplayer.MaxFPS when absent) for network sessions, -1
	// (uncapped) for everything else. There is no skirmish key: the engine
	// frame pacer only throttles network sessions, so single-player is always
	// left at its native (uncapped) rate.
	static int GetEffectiveMaxFPS();

	// Applies a MaxFPS preset to the engine frame-rate globals and to the
	// cnc-ddraw.dll renderer present cap. Preset semantics: -1 (or 0) =
	// uncapped, -2 = 60, N>0 = cap N. Intentionally does NOT log, because the
	// per-frame hook at 0x55DDA0 calls it every frame.
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
