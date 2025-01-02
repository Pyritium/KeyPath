#include <iostream>
#include "Enum.h"
#include "wchar.h"

RECORDED_INPUT Input;
KEY_CONTAINER KeyCache; // for current pressing

DataType Type = TYPE_NULL;

std::wstring RECORDING_STRING;


std::map<KEY_CONTAINER, KEY_CONTAINER> Binds; // saved binds, acts as a cache so we can add to it and then write to save file after closing

template <typename T>
std::wstring FormulateString(T data)
{
	std::wstring wstr;
	
	for (const KeyInput keydata : Input) {
		const DWORD key = keydata.Data;
		wstr += keydata.UnicodeChar;
	};

	return wstr;
}
void GetConfigDataFromFile() {};
bool CreateConfigFile()
{
	return 0;
};

void NewOption(wchar_t bind[], wchar_t recorded[]) {

};
void DeleteOption() {};

HWND TimerText;
void SetTimerLabel(int time)
{
	wchar_t Buffer[3];
	swprintf(Buffer, sizeof(Buffer) / sizeof(Buffer[0]), L"%d", time);
	SetWindowText(TimerText, Buffer);
}

void ResetTimer(HWND& hwnd) {
	ElapsedSeconds = 0;
	KillTimer(hwnd, TimerID);
}

bool IsSpecialKey(DWORD key) {
	return (key == VK_SHIFT || key == VK_CAPITAL || key == VK_LSHIFT || key == VK_RSHIFT ||
		key == VK_LCONTROL || key == VK_RCONTROL || key == VK_LMENU || key == VK_RMENU);
}

// For KeyCache, to represent what the *keybind is*
void EditKeysPressed(DWORD Key, WPARAM wParam, bool Inserting)
{
	auto it = std::find(KeyCache.begin(), KeyCache.end(), Key);
	bool Found = it != KeyCache.end();

	switch (Type)
	{
		case TYPE_KEY_CONTAINER:
		{

			break;
		}

		case TYPE_RECORDED_INPUT:
		{

			bool CanInsert = Input.size() < MAX_CONTAINER_SIZE && (!Found) && (!IsSpecialKey(Key));
			auto it = std::find_if(Input.begin(), Input.end(), [&](const auto& KeyCurrent) {
				return KeyCurrent.Data == Key;
			});
			bool found = (it != Input.end());

			if (Inserting && CanInsert && !found)
			{
				SetTimerLabel(TIMER_GOAL / 1000);
				ElapsedSeconds = 0;

				KeyInput NewKey(Key, wParam);
				Input.push_back(NewKey);
			}
			else if (!Inserting)
			{
				SetTimerLabel(TIMER_GOAL / 1000);
				ElapsedSeconds = 0;

				Input.erase(
					std::remove_if(Input.begin(), Input.end(), [&](const auto& KeyCurrent) {
						return KeyCurrent.Data == Key;
						}),
					Input.end()
				);
			}

			break;
		}
	}

	
	
};

HWND BoundToText;
HWND RecordToText;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		// key down
		bool KeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
		KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
		DWORD KeyCode = pKeyBoard->vkCode;

		//char KeyName[256];
		// bitshift to represent the key in hex
		//GetKeyNameTextA((KeyCode << 16), KeyName, sizeof(KeyName));

		EditKeysPressed(KeyCode, wParam, KeyDown);

		RECORDING_STRING = FormulateString(Input);
		SetWindowText(BoundToText, RECORDING_STRING.c_str());
	};
	
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
};

HWND RecordConfirmButton;
HWND SubConfirmButton;

void AdjustSubConfirmEnabled(HWND& hwnd)
{
	HWND bindToEdit = GetDlgItem(hwnd, 4);
	int bindLength = GetWindowTextLength(bindToEdit);

	HWND recordToEdit = GetDlgItem(hwnd, 5);
	int recordLength = GetWindowTextLength(recordToEdit);

	BOOL canConfirm = (bindLength > 0 && recordLength > 0);
	EnableWindow(SubConfirmButton, canConfirm);
}

LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		HWND TextBox = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | WS_BORDER, 10, 0, rect.right - 20, rect.bottom - 60, hwnd, (HMENU)1, NULL, NULL);
		RecordConfirmButton = CreateWindowEx(0, L"BUTTON", L"Confirm", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 100, 100, 100, 30, hwnd, (HMENU)2, NULL, NULL);
		EnableWindow(RecordConfirmButton, FALSE);
		return 0;
	}
	case WM_COMMAND: {
		int ID = LOWORD(wParam);

		HWND hEdit = GetDlgItem(hwnd, 1);
		int length = GetWindowTextLength(hEdit);

		switch (ID) {
		case 1: {
			BOOL over0Length = length > 0;
			EnableWindow(RecordConfirmButton, over0Length);
			break;
		}
		case 2: {
			wchar_t* buffer = new wchar_t[length + 1];
			GetWindowText(hEdit, buffer, length + 1);
			SetWindowText(RecordToText, buffer);
			DestroyWindow(hwnd);
			AdjustSubConfirmEnabled(SUB_WINDOW);

			delete[] buffer;
			break;
		}
		}
		return 0;
	}
	case WM_DESTROY: {
		RECORD_WINDOW = NULL;
		EnableWindow(SUB_WINDOW, TRUE);
		SetForegroundWindow(SUB_WINDOW);
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK SubWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HINSTANCE ptr = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	switch (msg) {
	case WM_CREATE:
	{
		
		HWND BindToButton = CreateWindowEx(0, L"BUTTON", L"Bind To:", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 50, 100, 30, hwnd, (HMENU)1, ptr, NULL);
		HWND RecordToButton = CreateWindowEx(0, L"BUTTON", L"Record To:", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 50, 100, 30, hwnd, (HMENU)2, ptr, NULL);
		SubConfirmButton = CreateWindowEx(0, L"BUTTON", L"Confirm", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 0, 100, 30, hwnd, (HMENU)3, ptr, NULL);

		// will enable when both boundto & recordto have insert(s)!
		EnableWindow(SubConfirmButton, FALSE);

		TimerText = CreateWindowEx(0, L"STATIC", L"0", WS_VISIBLE | WS_CHILD, 10, 10, 20, 20, hwnd, NULL, NULL, NULL);
		BoundToText = CreateWindowEx(0, L"STATIC", L"", WS_VISIBLE | WS_CHILD, 200, 100, 100, 100, hwnd, (HMENU)4, NULL, NULL);
		RecordToText = CreateWindowEx(0, L"STATIC", L"", WS_VISIBLE | WS_CHILD, 10, 100, 100, 100, hwnd, (HMENU)5, NULL, NULL);

		/*WNDCLASS pwc = {};
		pwc.lpfnWndProc = PopupProc;
		pwc.hInstance = g_hInstance;
		pwc.lpszClassName = L"PopupWndClass";*/

		return 0;
	}
	case WM_COMMAND:
	{
		int ID = LOWORD(wParam);
		AdjustSubConfirmEnabled(hwnd);
		switch (ID) {
		case 1: {
			ResetTimer(hwnd);
			SetTimer(hwnd, TimerID, TIMER_INTERVAL, NULL);
			Input.clear();
			Type = TYPE_RECORDED_INPUT;

			// Set the timer to the max timer goal for counting down, not accounting for the first second passing
			SetTimerLabel(TIMER_GOAL / 1000);

			break;
		}
		case 2: {
			WNDCLASS wc = {};
			wc.lpfnWndProc = PopupProc;
			wc.hInstance = ptr;
			wc.lpszClassName = L"POPUP_CLASS";
			RegisterClass(&wc);

			RECORD_WINDOW = CreateWindowEx(0, wc.lpszClassName, L"Record", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, SUB_WINDOW, NULL, NULL, NULL);

			ShowWindow(RECORD_WINDOW, SW_SHOW);
			UpdateWindow(RECORD_WINDOW);

			SetForegroundWindow(RECORD_WINDOW);
			EnableWindow(SUB_WINDOW, FALSE);

			break;
		}
		case 3:
			printf("Confirmed!\n");
			break;
		};
		break;
	}
	case WM_TIMER: {
		AdjustSubConfirmEnabled(hwnd);
		if (wParam == TimerID) {
			// Count down
			int GoalInSeconds = TIMER_GOAL / 1000;
			int SecondsLeft = GoalInSeconds - (ElapsedSeconds + 1);
			SetTimerLabel(SecondsLeft);

			ElapsedSeconds++;
			if (ElapsedSeconds >= GoalInSeconds) { // Trigger at 10 seconds
				ResetTimer(hwnd);
				Type = TYPE_NULL;
			}
		}
		break;
	}
	case WM_DESTROY:
		EnableWindow(WINDOW, TRUE);
		SetForegroundWindow(WINDOW);
		SUB_WINDOW = NULL;
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ActivateRecording(HWND hwnd) {
	if (!SUB_WINDOW)
	{
		WNDCLASS wc = {};
		wc.lpfnWndProc = SubWindowProc;
		wc.hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		wc.lpszClassName = L"SUBWINDOW_CLASS";
		RegisterClass(&wc);

		SUB_WINDOW = CreateWindowEx(0, wc.lpszClassName, L"New Input", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);

		ShowWindow(SUB_WINDOW, SW_SHOW);
		UpdateWindow(SUB_WINDOW);

		SetForegroundWindow(SUB_WINDOW);
		EnableWindow(WINDOW, FALSE);
	}
	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HINSTANCE ptr = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	switch (uMsg) {
	case WM_CREATE:
	{
		// Global
		TEXT = CreateWindowEx(0,L"STATIC",L"Use the 'NEW' button to create a new keybind. 'DELETE' will remove them!",WS_VISIBLE | WS_CHILD,0, 10, 300, 50,hwnd,NULL,NULL,NULL);
		LIST_BOX = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_SORT, 10, 100, 300, 150, hwnd, (HMENU)3, ptr, NULL);

		// Local
		HWND NewButton = CreateWindowEx(0,L"BUTTON",L"NEW",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,10, 50,100, 30,hwnd,(HMENU)1, ptr, NULL);
		HWND DeleteButton = CreateWindowEx(0, L"BUTTON", L"DELETE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 50, 100, 30, hwnd, (HMENU)2, ptr,NULL);
		

		return 0;
	}
	case WM_COMMAND: {
		int ID = LOWORD(wParam);
		switch (ID) {
		case 1:
			ActivateRecording(hwnd); // Call the button click handler
			return 0;
		case 2:
			// TODO: DELETE
			return 0;
		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	// Some debug tools to see processes
	AllocConsole();

	FILE* console;
	freopen_s(&console, "CONOUT$", "w", stdout);
	freopen_s(&console, "CONOUT$", "w", stderr);

	KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
	
	// Register WNDCLASS CIM
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CIM_CLASS;
	RegisterClass(&wc);

	WINDOW = CreateWindowEx(0, CIM_CLASS, L"Custom Input Manager", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);
	
	ShowWindow(WINDOW, SW_SHOW);
	UpdateWindow(WINDOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
};
