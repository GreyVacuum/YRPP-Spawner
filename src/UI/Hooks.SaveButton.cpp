#include <Spawner/Spawner.h>
#include <Utilities/Macro.h>
#include <Unsorted.h>
#include <windows.h>

enum GameOptionsControlID
{
	SaveGameButton = 1311,
	GameControlsButton = 1313,
	AbortMissionButton = 1314
};

// Dialog 3002 template geometry, in dialog units.
enum GameOptionsDialogLayout
{
	MultiplayerGameOptionsDialog = 3002,
	DialogWidth = 533,
	DialogHeight = 369,
	ButtonLeft = 425,
	ButtonWidth = 108,
	ButtonHeight = 23,
	// The save button takes AbortMission's slot and moves it to the next one, which places the
	// save button between GameControls and AbortMission like the single-player dialog does.
	SaveGameTop = 149,
	AbortMissionTop = 176
};

// Create the button before StandardWndProc registers and formats the controls.
DEFINE_HOOK(0x4F11D6, MultiplayerGameOptionsDialog_AddSaveButton, 0x6)
{
	if (!Spawner::Enabled || !Spawner::GetConfig()->ShowMPSaveGameButton)
		return 0;

	GET(UINT, message, EBX);
	GET(HWND, hDialog, ESI);
	GET(const WORD*, initData, EBP);

	if (message != WM_INITDIALOG || !initData || *initData != MultiplayerGameOptionsDialog)
		return 0;

	const auto hExistingSave = Imports::GetDlgItem(hDialog, SaveGameButton);
	if (hExistingSave)
		return 0;

	const auto hAbort = Imports::GetDlgItem(hDialog, AbortMissionButton);
	const auto hControls = Imports::GetDlgItem(hDialog, GameControlsButton);

	// Convert the template geometry with the dialog's own client size, the same mapping Windows
	// used to lay out the template controls, so the result does not depend on the resolution,
	// the DPI or on where the game moves the other buttons.
	RECT clientRect;
	if (!Imports::GetClientRect(hDialog, &clientRect) || clientRect.right <= 0 || clientRect.bottom <= 0)
		return 0;

	const int left = ButtonLeft * clientRect.right / DialogWidth;
	const int width = ButtonWidth * clientRect.right / DialogWidth;
	const int height = ButtonHeight * clientRect.bottom / DialogHeight;
	const int saveTop = SaveGameTop * clientRect.bottom / DialogHeight;
	const int abortTop = AbortMissionTop * clientRect.bottom / DialogHeight;
	const auto hInstance = reinterpret_cast<HINSTANCE>(Imports::GetWindowLongA(hDialog, GWL_HINSTANCE));
	const auto hSave = Imports::CreateWindowExA(0, "BUTTON", "GUI:SaveGame",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		left, saveTop, width, height, hDialog,
		reinterpret_cast<HMENU>(SaveGameButton), hInstance, nullptr);

	if (!hSave)
		return 0;

	const auto font = Imports::SendMessageA(hAbort ? hAbort : hDialog, WM_GETFONT, 0, 0);
	Imports::SendMessageA(hSave, WM_SETFONT, font, FALSE);

	// Keep the save button between the game controls and AbortMission in the child window order.
	Imports::SetWindowPos(hSave, hControls ? hControls : hAbort, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	if (hAbort)
		Imports::SetWindowPos(hAbort, nullptr, left, abortTop, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

DEFINE_HOOK(0x609299, UI_IsStaticAndOrOwnerDraw_MultiplayerGameOptionsDialog, 0x5)
{
	if (!Spawner::Enabled)
		return 0;

	enum { RetTrue = 0x609693 };
	GET(int, dlgCtrlID, EAX);

	if (dlgCtrlID == SaveGameButton)
		return RetTrue;

	return 0;
}

// The description lookup of dialog 3002 has no entry for the save button, so the bottom text
// area stays empty. Reuse the description of the single-player save button for it.
DEFINE_HOOK(0x6050CD, MultiplayerGameOptionsDialog_SaveButtonDescription, 0x5)
{
	// Case 181, control 1311: pop edi, load "STT:IGSingleOptButtonSave", pop esi/ecx, return.
	enum { ReturnSinglePlayerSaveDescription = 0x605088 };

	if (!Spawner::Enabled)
		return 0;

	GET(int, dlgCtrlID, EAX);

	if (dlgCtrlID == SaveGameButton)
		return ReturnSinglePlayerSaveDescription;

	return 0;
}
