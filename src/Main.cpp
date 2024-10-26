#include <iostream>
#include <vector>
#include <unordered_map>
#include <Windows.h>

HHOOK KeyboardHook;

typedef std::vector<DWORD> KEY_CONTAINER;

KEY_CONTAINER KeyCache; // for current pressing
std::unordered_map<KEY_CONTAINER, KEY_CONTAINER> Binds; // saved binds?


void GetBindsFromConfigFile() {};
bool CreateConfigFile()
{
	return 0;
};



void EditKeysPressed(DWORD Key, bool Inserting)
{
	// TODO: Log in KeyCache
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		// key down
		bool KeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
		KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
		DWORD KeyCode = pKeyBoard->scanCode;

		char KeyName[256];
		// bitshift to represent the key in hex
		GetKeyNameTextA((KeyCode << 16), KeyName, sizeof(KeyName));
		EditKeysPressed(KeyCode, KeyDown);
	};
	
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
};

const LPCWSTR CIM_CLASS = L"CIM";

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

	// TODO: 
	// 1.) create a button that adds, listens to bind, awaits user confirmation, then adds to list
	// 2.) a list with all the buttons corresponding to specific keybinds
	HWND Window = CreateWindowEx(0, CIM_CLASS, L"Custom Input Manager", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);
	
	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
};
