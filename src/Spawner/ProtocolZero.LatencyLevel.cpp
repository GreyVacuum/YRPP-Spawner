/**
*  yrpp-spawner
*
*  Copyright(C) 2023-present CnCNet
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

#include "ProtocolZero.h"
#include "ProtocolZero.LatencyLevel.h"

#include <HouseClass.h>
#include <MessageListClass.h>
#include <StringTable.h>
#include <Utilities/Debug.h>
#include <Unsorted.h>

LatencyLevelEnum LatencyLevel::CurentLatencyLevel = LatencyLevelEnum::LATENCY_LEVEL_INITIAL;
unsigned char LatencyLevel::NewFrameSendRate = 3;

void LatencyLevel::Apply(LatencyLevelEnum newLatencyLevel)
{
	if (newLatencyLevel > LatencyLevelEnum::LATENCY_LEVEL_MAX)
		newLatencyLevel = LatencyLevelEnum::LATENCY_LEVEL_MAX;

	auto maxLatencyLevel = static_cast<LatencyLevelEnum>(ProtocolZero::MaxLatencyLevel);
	if (newLatencyLevel > maxLatencyLevel)
		newLatencyLevel = maxLatencyLevel;

	if (newLatencyLevel <= CurentLatencyLevel)
		return;

	Debug::Log("Player %ls, Loss mode (%d, %d) Frame = %d\n"
		, HouseClass::CurrentPlayer->UIName
		, newLatencyLevel
		, CurentLatencyLevel
		, (int)Unsorted::CurrentFrame
	);

	CurentLatencyLevel = newLatencyLevel;
	NewFrameSendRate = static_cast<unsigned char>(newLatencyLevel);

	// Native behaviour, matches yr-patches protocol_zero_c.c LatencyMode_Apply:
	// the protocol-0 latency adaptation re-pins the network frame rate to 60.
	// Do NOT route this through the spawner MaxFPS presets -- RequestedFPS /
	// PreCalcFrameRate are netcode- and speed-control-owned.
	Game::Network::PreCalcFrameRate = 60;

	Game::Network::PreCalcMaxAhead = GetMaxAhead(newLatencyLevel);

	MessageListClass::Instance.PrintMessage(GetLatencyMessage(newLatencyLevel), (int)(RulesClass::Instance->MessageDelay * 900), ColorScheme::White, true);
}

int LatencyLevel::GetMaxAhead(LatencyLevelEnum latencyLevel)
{
	static const int maxAhead[] =
	{
		/* 0 */ 1

		/* 1 */ ,4
		/* 2 */ ,6
		/* 3 */ ,12
		/* 4 */ ,16
		/* 5 */ ,20
		/* 6 */ ,24
		/* 7 */ ,28
		/* 8 */ ,32
		/* 9 */ ,36
	};

	return maxAhead[(int)latencyLevel];
}

const wchar_t* LatencyLevel::GetLatencyMessage(LatencyLevelEnum latencyLevel)
{
	// Each latency-mode notification is looked up from the CSF string table so it
	// can be localized. The hard-coded English strings below are only fallbacks used
	// when the corresponding TXT_CNCNET_LATENCY_* label is absent from the CSF.
	switch (latencyLevel)
	{
		case LatencyLevelEnum::LATENCY_LEVEL_INITIAL: return StringTable::TryFetchString("TXT_CNCNET_LATENCY_0", L"CnCNet: Latency mode set to: 0 - Initial");
		case LatencyLevelEnum::LATENCY_LEVEL_1:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_1", L"CnCNet: Latency mode set to: 1 - Best");
		case LatencyLevelEnum::LATENCY_LEVEL_2:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_2", L"CnCNet: Latency mode set to: 2 - Super");
		case LatencyLevelEnum::LATENCY_LEVEL_3:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_3", L"CnCNet: Latency mode set to: 3 - Excellent");
		case LatencyLevelEnum::LATENCY_LEVEL_4:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_4", L"CnCNet: Latency mode set to: 4 - Very Good");
		case LatencyLevelEnum::LATENCY_LEVEL_5:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_5", L"CnCNet: Latency mode set to: 5 - Good");
		case LatencyLevelEnum::LATENCY_LEVEL_6:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_6", L"CnCNet: Latency mode set to: 6 - Good");
		case LatencyLevelEnum::LATENCY_LEVEL_7:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_7", L"CnCNet: Latency mode set to: 7 - Default");
		case LatencyLevelEnum::LATENCY_LEVEL_8:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_8", L"CnCNet: Latency mode set to: 8 - Default");
		case LatencyLevelEnum::LATENCY_LEVEL_9:       return StringTable::TryFetchString("TXT_CNCNET_LATENCY_9", L"CnCNet: Latency mode set to: 9 - Default");
		default:                                      return L"";
	}
}

LatencyLevelEnum LatencyLevel::FromResponseTime(unsigned char rspTime)
{
	for (auto i = LatencyLevelEnum::LATENCY_LEVEL_1; i < LatencyLevelEnum::LATENCY_LEVEL_MAX; i = static_cast<LatencyLevelEnum>(1 + static_cast<char>(i)))
	{
		if (rspTime <= GetMaxAhead(i))
			return static_cast<LatencyLevelEnum>(i);
	}

	return LatencyLevelEnum::LATENCY_LEVEL_MAX;
}
