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

#include "Spawner.h"
#include "NetHack.h"

#include <HouseClass.h>
#include <SessionClass.h>
#include <BeaconManagerClass.h>
#include <Utilities/Debug.h>
#include <Utilities/Macro.h>
#include <Unsorted.h>
#include <CCINIClass.h>
#include <StringTable.h>
#include <Surface.h>

DEFINE_HOOK(0x6BD7C5, WinMain_SpawnerInit, 0x6)
{
	if (Spawner::Enabled)
	{
		Spawner::Init();


		Patch::Apply_CALL(0x48CDD3, Spawner::StartGame); // Main_Game
		Patch::Apply_CALL(0x48CFAA, Spawner::StartGame); // Main_Game

		{ // HousesStuff
			Patch::Apply_CALL(0x68745E, Spawner::AssignHouses); // Read_Scenario_INI
			Patch::Apply_CALL(0x68ACFF, Spawner::AssignHouses); // ScenarioClass::Read_INI

			Patch::Apply_LJMP(0x5D74A0, 0x5D7570);   // MPGameModeClass_AllyTeams
			Patch::Apply_LJMP(0x501721, 0x501736);   // HouseClass_ComputerParanoid
			//Patch::Apply_LJMP(0x686A9E, 0x686AC6); // ReadScenario_InitSomeThings - Moved to a hook to allow conditional toggling of Special house's alliances.
		}

		{ // NetHack
			Patch::Apply_CALL(0x7B3D75, NetHack::SendTo);   // UDPInterfaceClass::Message_Handler
			Patch::Apply_CALL(0x7B3EEC, NetHack::RecvFrom); // UDPInterfaceClass::Message_Handler
		}

		{ // Skip Intro, EA_WWLOGO and LoadScreen
			Patch::Apply_LJMP(0x52CB50, 0x52CB6E); // InitIntro_Skip
			Patch::Apply_LJMP(0x52C5E0, 0x52C5F8); // InitGame_SkipLogoAndLoadScreen
		}

		{ // Cooperative
			Patch::Apply_LJMP(0x553321, 0x5533C5); // LoadProgressMgr_Draw_CooperativeDescription
			Patch::Apply_LJMP(0x55D0DF, 0x55D0E8); // AuxLoop_Cooperative_EndgameCrashFix
		}

		// Set ConnTimeout
		Patch::Apply_TYPED<int>(0x6843C7, { Spawner::GetConfig()->ConnTimeout }); // Scenario_Load_Wait

		// Show GameMode in DiplomacyDialog in Skirmish
		Patch::Apply_LJMP(0x658117, 0x658126); // RadarClass_DiplomacyDialog

		// Leaves bottom bar closed for losing players during last game frames
		Patch::Apply_LJMP(0x6D1639, 0x6D1640); // TabClass_6D1610

		// Skip load *.PKT, *.YRO and *.YRM map files
		Patch::Apply_LJMP(0x699AD9, 0x69A1B2); // SessionClass::Read_Scenario_Descriptions
	}

	return 0;
}

// Display UIGameMode if is set
// Otherwise use mode name from MPModesMD.ini
DEFINE_HOOK(0x65812E, RadarClass__DiplomacyDialog_UIGameMode, 0x6)
{
	enum { Show = 0x65813E, DontShow = 0x65814D };

	if (Spawner::Enabled && Spawner::GetConfig()->UIGameMode[0])
	{
		R->EBX(R->EAX());
		R->EAX(Spawner::GetConfig()->UIGameMode);
		return Show;
	}

	if (!SessionClass::Instance.MPGameMode)
		return DontShow;

	return 0;
}

// Clear UIGameMode on game load
DEFINE_HOOK(0x689669, ScenarioClass_Load_Suffix, 0x6)
{
	if (Spawner::Enabled)
		Spawner::GetConfig()->UIGameMode[0] = 0;

	return 0;
}

#pragma region MPlayerDefeated
namespace MPlayerDefeated
{
	HouseClass* pThis = nullptr;
}

DEFINE_HOOK(0x4FC0B6, HouseClass__MPlayerDefeated_SaveArgument, 0x5)
{
	MPlayerDefeated::pThis = (Spawner::Enabled && !SessionClass::IsCampaign())
		? R->ECX<HouseClass*>()
		: nullptr;

	return 0;
}

// Skip match-end logic if MPlayerDefeated called for observer
DEFINE_HOOK_AGAIN(0x4FC332, HouseClass__MPlayerDefeated_SkipObserver, 0x5)
DEFINE_HOOK(0x4FC262, HouseClass__MPlayerDefeated_SkipObserver, 0x6)
{
	enum { ProcEpilogue = 0x4FC6BC };

	if (!MPlayerDefeated::pThis)
		return 0;

	return MPlayerDefeated::pThis->IsInitiallyObserver()
		? ProcEpilogue
		: 0;
}

DEFINE_HOOK(0x4FC551, HouseClass__MPlayerDefeated_NoEnemies, 0x5)
{
	enum { ProcEpilogue = 0x4FC6BC };

	if (!MPlayerDefeated::pThis)
		return 0;

	for (const auto pHouse : HouseClass::Array)
	{
		if (pHouse->Defeated || pHouse == MPlayerDefeated::pThis || pHouse->Type->MultiplayPassive)
			continue;

		if ((pHouse->IsHumanPlayer || Spawner::GetConfig()->ContinueWithoutHumans) && pHouse->IsMutualAlly(MPlayerDefeated::pThis))
		{
			Debug::Log("MPlayer_Defeated() - Defeated player has a living ally");
			if (Spawner::GetConfig()->DefeatedBecomesObserver)
				MPlayerDefeated::pThis->MakeObserver();

			return ProcEpilogue;
		}
	}

	return 0;
}

DEFINE_HOOK(0x4FC57C, HouseClass__MPlayerDefeated_CheckAliveAndHumans, 0x7)
{
	enum { ProcEpilogue = 0x4FC6BC, FinishMatch = 0x4FC591 };

	if (!MPlayerDefeated::pThis)
		return 0;

	GET_STACK(int, numHumans, STACK_OFFSET(0xC0, -0xA8));
	GET_STACK(int, numAlive, STACK_OFFSET(0xC0, -0xAC));

	bool continueWithoutHumans = Spawner::GetConfig()->ContinueWithoutHumans
		|| MPlayerDefeated::pThis->IsInitiallyObserver();

	if (!continueWithoutHumans && !MPlayerDefeated::pThis->IsHumanPlayer)
	{
		bool isHasAliveHumanPlayers = false;
		for (const auto pHouse : HouseClass::Array)
		{
			if (pHouse->IsHumanPlayer && !pHouse->Defeated)
			{
				isHasAliveHumanPlayers = true;
				break;
			}
		}

		if (!isHasAliveHumanPlayers)
			continueWithoutHumans = true;
	}

	if (numAlive > 1 && (numHumans != 0 || continueWithoutHumans))
	{
		if (Spawner::GetConfig()->DefeatedBecomesObserver)
			MPlayerDefeated::pThis->MakeObserver();

		return ProcEpilogue;
	}

	return FinishMatch;
}

#pragma endregion MPlayerDefeated

#pragma region Save&Load

DEFINE_HOOK_AGAIN(0x624271, SomeFunc_InterceptMainLoop, 0x5);
DEFINE_HOOK_AGAIN(0x623D72, SomeFunc_InterceptMainLoop, 0x5);
DEFINE_HOOK_AGAIN(0x62314E, SomeFunc_InterceptMainLoop, 0x5);
DEFINE_HOOK_AGAIN(0x60D407, SomeFunc_InterceptMainLoop, 0x5);
DEFINE_HOOK_AGAIN(0x608206, SomeFunc_InterceptMainLoop, 0x5);
DEFINE_HOOK(0x48CE8A, SomeFunc_InterceptMainLoop, 0x5)
{
	/**
	 *  Main loop.
	 */
	Game::MainLoop();

	/**
	 *  After loop.
	 */
	Spawner::After_Main_Loop();
	return R->Origin() + 0x5;
}

DEFINE_HOOK(0x52DAEF, Game_Start_ResetGlobal, 0x5)
{
	Spawner::DoSave = false;
	Spawner::NextAutoSaveFrame = -1;
	Spawner::NextAutoSaveNumber = 0;
	return 0;
}

DEFINE_HOOK(0x686B20, INIClass_ReadScenario_AutoSave, 0x6)
{
	/**
	 *  Schedule the next autosave.
	 */
	Spawner::NextAutoSaveFrame = Unsorted::CurrentFrame;
	Spawner::NextAutoSaveFrame += Spawner::GetConfig()->AutoSaveInterval;
	return 0;
}

// Do not change the address without adjusting Phobos handling
// and reading the comments in Spawner::After_Main_Loop
DEFINE_HOOK(0x4C7A14, EventClass_RespondToEvent_SaveGame, 0x5)
{
	Spawner::RespondToSaveGame();
	return 0x4C7B42;
}

// for some reason beacons are only inited on scenario init, which doesn't happen on load
DEFINE_HOOK(0x67E6DA, LoadGame_AfterInit, 0x6)
{
	BeaconManagerClass::Instance.LoadArt();
	return 0;
}

#pragma endregion

DEFINE_HOOK(0x686A9E, ReadScenario_InitSomeThings_SpecialHouseIsAlly, 0x6)
{
	if (Spawner::GetConfig()->SpecialHouseIsAlly)
		return 0;

	return 0x686AC6;
}

DEFINE_HOOK(0x686D46, ReadScenarioINI_MissionININame, 0x5)
{
	LEA_STACK(CCFileClass*, pFile, STACK_OFFSET(0x174, -0xF0));

	if (Spawner::GetConfig()->ReadMissionSection)
	{
		pFile->SetFileName("SPAWN.INI");
		return 0x686D57;
	}

	return 0;
}

DEFINE_HOOK(0x65F57F, BriefingDialog_MissionININame, 0x6)
{
	LEA_STACK(CCFileClass*, pFile, STACK_OFFSET(0x1D4, -0x16C));

	if (Spawner::GetConfig()->ReadMissionSection)
	{
		pFile->SetFileName("SPAWN.INI");
		return 0x65F58F;
	}

	return 0;
}

// ============================================================================
// Loading Progress Percentage Display
// ============================================================================
// Two hooks work together:
// 1. Hook at 0x643C2F in sub_643AE0: re-implements the loop logic after
//    sub_643720 returns (the 6 overwritten bytes: movsx/inc/cmp).
// 2. Hook at 0x643AD1 in sub_643720: after sub_643670 draws the player name,
//    renders the loading percentage "XX%" right after the name text.
//    We have the exact text position (v35, v36) on the stack at this point.
// ============================================================================

// Hook 1: Loop logic in sub_643AE0 (no drawing here, just loop control)
DEFINE_HOOK(0x643C2F, ProgressScreen_Draw_LoadingPercentage_Loop, 0x6)
{
	DWORD pThis = R->ESI();
	int playerIndex = R->EDI();

	// Re-execute overwritten loop logic:
	// movsx eax, byte ptr [esi+61h]  -> totalPlayers = [esi+0x61]
	// inc edi                         -> newPlayerIndex = playerIndex + 1
	// cmp edi, eax; jl ...           -> if (newPlayerIndex < totalPlayers) continue
	int totalPlayers = (signed char)(*(byte*)(pThis + 0x61));
	int newPlayerIndex = playerIndex + 1;
	R->EDI(newPlayerIndex);

	if (newPlayerIndex < totalPlayers)
		return 0x643C0B; // Jump to loop start
	else
		return 0x643C38; // Jump to after loop
}

// Hook 2: Draw percentage text after player name in sub_643720
// At 0x643AD1, sub_643670 has just drawn the player name at (v35, v36).
// The percentage is drawn right-aligned near the right edge of the progress
// bar area, so it's always visible regardless of name length.
//
// Stack layout at hook point (frame size: 0x5C):
//   ESP+0x18: v32  (var_44, progress bar width)
//   ESP+0x24: v35  (var_38, text X position)
//   ESP+0x28: v36  (var_34, text Y position)
//   ESP+0x68: arg_8 (a4 pointer, *a4 = left edge of area)
//   ESP+0x6C: arg_C (player index)
//
// Overwritten instructions (7 bytes):
//   0x643AD1: pop edi      (1)  - restore saved edi
//   0x643AD2: pop esi      (1)  - restore saved esi
//   0x643AD3: pop ebp      (1)  - restore saved ebp
//   0x643AD4: pop ebx      (1)  - restore saved ebx
//   0x643AD5: add esp, 4Ch (3)  - cleanup local variables
//   After: 0x643AD8: retn 14h
// ============================================================================
DEFINE_HOOK(0x643AD1, ProgressScreen_Draw_LoadingPercentage_Text, 0x7)
{
	// Render percentage if enabled
	if (Spawner::Enabled && Spawner::GetConfig()->ShowLoadingProgress)
	{
		// Read local variables from sub_643720's stack frame
		// v36 (textY) at ESP+0x28
		GET_STACK(int, textY, STACK_OFFSET(0x5C, 0x28 - 0x5C));
		// arg_8 (a4 pointer) at ESP+0x68, *a4 = left edge of area
		GET_STACK(int*, a4, STACK_OFFSET(0x5C, 0xC));
		// arg_C (playerIndex) at ESP+0x6C
		GET_STACK(int, playerIndex, STACK_OFFSET(0x5C, 0x10));

		// ESI = ProgressScreenClass* (this pointer saved in esi)
		DWORD pThis = R->ESI();

		// Get progress data: PlayerProgresses[playerIndex] / total
		double progress = *(double*)(pThis + 8 * playerIndex + 8);
		double total = *(double*)(pThis + 72);

		int percentage = 0;
		if (total > 0.0)
			percentage = static_cast<int>(progress / total * 100.0);
		if (percentage < 0) percentage = 0;
		if (percentage > 100) percentage = 100;

		// Get the wchar_t* player name from the stack (lParam at ESP+0x64)
		GET_STACK(wchar_t*, playerName, STACK_OFFSET(0x5C, 0x8));

		// Format "Name [XX" (name + number, no closing bracket).
		static wchar_t nameBuf[64];
		const wchar_t* format = StringTable::TryFetchString("TXT_LoadProgressFormat", L"%s [%d");
		swprintf_s(nameBuf, format, playerName ? playerName : L"?", percentage);

		// '%' is a special character to the text system (just like the %s / %d
		// format specifiers). A lone '%' gets swallowed when the string is
		// drawn, so we emit "%%" - the standard escape - which the renderer
		// collapses to a single literal '%'. Strip any trailing ']' the
		// CSF/format may have added, then close the bracket.
		size_t len = wcslen(nameBuf);
		if (len > 0 && nameBuf[len - 1] == L']')
			nameBuf[--len] = L'\0';
		wcscat_s(nameBuf, L"%%]");

		// Draw the combined text using Fancy_Text_Print_Wide (YRpp method)
		// at the same position where the original name would be drawn
		if (DSurface::Hidden != nullptr && a4 != nullptr)
		{
			// Use textX (v35 at ESP+0x24) where the name starts
			GET_STACK(int, textX, STACK_OFFSET(0x5C, 0x24 - 0x5C));
			Point2D location = { textX, textY };
			RectangleStruct bounds = { 0, 0, 800, 600 };
			Point2D tmp = { 0, 0 };
			Fancy_Text_Print_Wide(tmp, nameBuf, DSurface::Hidden, bounds, location, 0xFFFFFF, 0, TextPrintType::NoShadow);
		}
	}

	// Re-execute the overwritten cleanup instructions via R pointer
	// This avoids __asm stack corruption that conflicts with the compiler's frame pointer.
	// pop edi
	R->EDI(R->Stack<DWORD>(0));
	R->ESP(R->ESP() + 4);
	// pop esi
	R->ESI(R->Stack<DWORD>(0));
	R->ESP(R->ESP() + 4);
	// pop ebp
	R->EBP(R->Stack<DWORD>(0));
	R->ESP(R->ESP() + 4);
	// pop ebx
	R->EBX(R->Stack<DWORD>(0));
	R->ESP(R->ESP() + 4);
	// add esp, 4Ch
	R->ESP(R->ESP() + 0x4C);

	// Jump to retn 14h
	return 0x643AD8;
}
