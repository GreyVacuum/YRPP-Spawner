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
#include <Main.h>

class CCINIClass;

class SpawnerConfig
{

	// Used to create NodeNameType
	// The order of entries may differ from HouseConfig
	struct PlayerConfig
	{
		bool IsHuman;
		wchar_t Name[20];
		int Color;
		int Country;
		int Difficulty;
		bool IsObserver;
		char Ip[0x20];
		int Port;

		PlayerConfig()
			: IsHuman { false }
			, Name { L"" }
			, Color { -1 }
			, Country { -1 }
			, Difficulty { -1 }
			, IsObserver { false }
			, Ip { "0.0.0.0" }
			, Port { -1 }
		{ }

		void LoadFromINIFile(CCINIClass* pINI, int index);
	};

	// Used to augment the generated HouseClass
	// The order of entries may differ from PlayerConfig
	struct HouseConfig
	{
		bool IsObserver;
		int SpawnLocations;
		double CreditsFactor;
		int HandicapDifficulty;
		int Alliances[8];

		HouseConfig()
			: IsObserver { false }
			, SpawnLocations { -2 }
			, CreditsFactor { 1.0 }
			, HandicapDifficulty { -1 }
			, Alliances { -1, -1, -1, -1, -1, -1, -1, -1 }
		{ }

		void LoadFromINIFile(CCINIClass* pINI, int index);
	};

public:
	// Game Mode Options
	int  MPModeIndex;
	bool Bases;
	int  Credits;
	bool BridgeDestroy;
	bool Crates;
	bool ShortGame;
	bool SuperWeapons;
	bool BuildOffAlly;
	int  GameSpeed;
	bool MultiEngineer;
	int  UnitCount;
	int  AIPlayers;
	int  AIDifficulty;
	bool AlliesAllowed;
	bool HarvesterTruce;
	bool FogOfWar;
	bool MCVRedeploy;
	wchar_t UIGameMode[60];
	bool SpecialHouseIsAlly;

	// SaveGame Options
	bool LoadSaveGame;
	char SavedGameDir[MAX_PATH]; // Nested paths are also supported, e.g. "Saved Games\\Yuri's Revenge"
	char SaveGameName[60];
	int CustomMissionID;
	int AutoSaveCount;
	int AutoSaveInterval;
	int NextAutoSaveNumber;

	// Scenario Options
	int  Seed;
	int  TechLevel;
	bool IsCampaign;
	int  Tournament;
	DWORD WOLGameID;
	char ScenarioName[260];
	char MapHash[0xff];
	wchar_t UIMapName[45];
	bool ReadMissionSection;

	// Network Options
	int Protocol;
	int FrameSendRate;
	int ReconnectTimeout;
	int ConnTimeout;
	int MaxAhead;
	int PreCalcMaxAhead;
	byte MaxLatencyLevel;
	bool ForceMultiplayer;

	// ===== Frame-rate control system (spawner [Settings], multiplayer only;
	// single-player always keeps its native pacing) =====
	//
	//   MP.MaxFPS            : -2 = native 60 (default, zero intervention)
	//                          -1 = no ceiling (free-run, >60)
	//                           N = target N FPS (N may be below OR above 60)
	//   MP.Protocol0.MaxFPS / MP.Protocol2.MaxFPS
	//                        : per-protocol override, inheriting MP.MaxFPS
	//                          when absent (collapsed at parse time)
	//   MP.MinFPS            : floor (0 = off). Engine-driven slowdowns (speed
	//                          slider, adaptive lag compensation) never push
	//                          the FPS below this value.
	//   MP.Protocol0.MinFPS / MP.Protocol2.MinFPS
	//                        : per-protocol floor override, inheriting
	//                          MP.MinFPS when absent (collapsed at parse time).
	//   MP.AdaptiveFPS       : yes (default) = the engine's own pacing
	//                          decisions are respected: the speed slider scales
	//                          the MaxFPS target, and adaptive lag compensation
	//                          may slow it further (lets lagging players catch
	//                          up -- the safe choice for multiplayer).
	//                          no = rock-stable pacing: the slider still scales
	//                          the target, but lag compensation is IGNORED (the
	//                          engine cannot self-slow to catch up -- for
	//                          testing / streaming, riskier online).
	//
	// Implementation note (IDA-verified): none of the netcode-owned globals
	// (RequestedFPS 0xA8B558 / PreCalcFrameRate 0xA8B570) are ever written --
	// pinning them desyncs multiplayer and welds the speed control. The lever
	// is the in-game frame-wait budget at 0x887330; the speed slider reaches
	// the engine as a synced GameSpeed event that writes RequestedFPS.
	int MP_MaxFPS;
	int MP_Protocol0MaxFPS;
	int MP_Protocol2MaxFPS;
	int MP_MinFPS;
	int MP_Protocol0MinFPS;
	int MP_Protocol2MinFPS;
	//   MP.SpeedTableMode   : No (default) = MP.MaxFPS / MP.MinFPS drive the
	//                          pacing. Yes = the MP.SpeedTableN keys below take
	//                          over COMPLETELY -- MP.MaxFPS and MP.MinFPS are
	//                          ignored (including MaxFPS' -2 kill switch; turn
	//                          the mode off instead).
	//   MP.SpeedTableN      : N = 0..6, one explicit FPS target per engine
	//                          slider slot, 0 = fastest ... 6 = slowest
	//                          (native slots: 60/45/30/20/15/12/10 FPS -- the
	//                          engine's GameSpeed index direction, IDA-verified:
	//                          GameSpeed = 6 - slider position).
	//                          Value = >60 (unlock), <60 (cap), -1/0 (uncapped
	//                          for that slot). Default = the engine's own value,
	//                          so enabling the mode alone changes nothing.
	//                          MP.AdaptiveFPS still decides whether the engine's
	//                          adaptive lag compensation is respected, and the
	//                          renderer cap follows MP.SpeedTable0.
	bool MP_AdaptiveFPS;
	bool MP_SpeedTableMode;
	int  MP_SpeedTable[7];

	// Tunnel Options
	int  TunnelId;
	char TunnelIp[0x20];
	int  TunnelPort;
	int  ListenPort;

	// Players Options
	PlayerConfig Players[8];

	// Houses Options
	HouseConfig Houses[8];

	// Extended Options
	bool Ra2Mode;
	bool DisableGameSpeed;
	bool QuickMatch;
	bool SkipScoreScreen;
	bool WriteStatistics;
	bool AINamesByDifficulty;
	bool ContinueWithoutHumans;
	bool DefeatedBecomesObserver;
	bool Observer_ShowAIOnSidebar;
#ifdef IS_CNCNET_YR_VER
	bool DisableChat;
#endif

	SpawnerConfig() // default values
		// Game Mode Options
		: MPModeIndex { 1 }
		, Bases { true }
		, Credits { 10000 }
		, BridgeDestroy { true }
		, Crates { false }
		, ShortGame { false }
		, SuperWeapons { true }
		, BuildOffAlly { false }
		, GameSpeed { 0 }
		, MultiEngineer { false }
		, UnitCount { 0 }
		, AIPlayers { 0 }
		, AIDifficulty { 1 }
		, AlliesAllowed { false }
		, HarvesterTruce { false }
		, FogOfWar { false }
		, MCVRedeploy { true }
		, UIGameMode { L"" }
		, SpecialHouseIsAlly { true }

		// SaveGame
		, LoadSaveGame { false }
		, SavedGameDir { "Saved Games" }
		, SaveGameName { "" }
		, CustomMissionID { 0 }
		, AutoSaveCount { 5 }
		, AutoSaveInterval { 7200 }
		, NextAutoSaveNumber { 0 }

		// Scenario Options
		, Seed { 0 }
		, TechLevel { 10 }
		, IsCampaign { false }
		, Tournament { 0 }
		, WOLGameID { 0xDEADBEEF }
		, ScenarioName { "spawnmap.ini" }
		, MapHash { "" }
		, UIMapName { L"" }
		, ReadMissionSection { false }

		// Network Options
		, Protocol { 2 }
		, FrameSendRate { 4 }
		, ReconnectTimeout { 2400 }
		, ConnTimeout { 3600 }
		, MaxAhead { -1 }
		, PreCalcMaxAhead { 0 }
		, MaxLatencyLevel { 0xFF }
		, ForceMultiplayer { false }

		, MP_MaxFPS { -2 } // 60 (native protocol 2 online behavior)
		, MP_Protocol0MaxFPS { -2 } // 60; overwritten to inherit MP.MaxFPS in LoadFromINIFile when key absent
		, MP_Protocol2MaxFPS { -2 }
		, MP_MinFPS { 0 } // 0 = no floor
		, MP_Protocol0MinFPS { 0 } // 0; overwritten to inherit MP.MinFPS in LoadFromINIFile when key absent
		, MP_Protocol2MinFPS { 0 }
		, MP_AdaptiveFPS { true }
		, MP_SpeedTableMode { false }
		// Engine-native per-slot FPS (slot 0 = fastest ... 6 = slowest).
		, MP_SpeedTable { 60, 45, 30, 20, 15, 12, 10 }

		// Tunnel Options
		, TunnelId { 0 }
		, TunnelIp { "0.0.0.0" }
		, TunnelPort { 0 }
		, ListenPort { 1234 }

		// Players Options
		, Players {
			PlayerConfig(),
			PlayerConfig(),
			PlayerConfig(),
			PlayerConfig(),

			PlayerConfig(),
			PlayerConfig(),
			PlayerConfig(),
			PlayerConfig()
		}

		// Houses Options
		, Houses {
			HouseConfig(),
			HouseConfig(),
			HouseConfig(),
			HouseConfig(),

			HouseConfig(),
			HouseConfig(),
			HouseConfig(),
			HouseConfig()
		}

		// Extended Options
		, Ra2Mode { false }
		, DisableGameSpeed { false }
		, QuickMatch { false }
		, SkipScoreScreen { Main::GetConfig()->SkipScoreScreen }
		, WriteStatistics { false }
		, AINamesByDifficulty { false }
		, ContinueWithoutHumans { false }
		, DefeatedBecomesObserver { false }
		, Observer_ShowAIOnSidebar { false }
#ifdef IS_CNCNET_YR_VER
		, DisableChat { false }
#endif
	{ }

	void LoadFromINIFile(CCINIClass* pINI);
};
