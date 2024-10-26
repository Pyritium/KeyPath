#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <Windows.h>

HHOOK KeyboardHook;
HWND TEXT;
HWND LIST_BOX;
HWND WINDOW;
HWND SUB_WINDOW;

const LPCWSTR CIM_CLASS = L"CIM";
constexpr size_t MAX_CONTAINER_SIZE = 256;
constexpr size_t MAX_KEYS = 5;

// For the current keybind being written out
typedef std::array<DWORD, MAX_KEYS> KEY_CONTAINER;
// Pertains to the keys recorded
typedef std::vector<DWORD> RECORDED_INPUT;

RECORDED_INPUT Input;
KEY_CONTAINER KeyCache; // for current pressing

std::map<KEY_CONTAINER, KEY_CONTAINER> Binds; // saved binds, acts as a cache so we can add to it and then write to save file after closing

void PrintVectorElements()
{
	for (const DWORD key : KeyCache) {
		std::cout << key << ",";
	};
	std::cout << std::endl;
}

void GetBindsFromConfigFile() {};
bool CreateConfigFile()
{
	return 0;
};

// For KeyCache, to represent what the *keybind is*
void EditKeysPressed(DWORD Key, bool Inserting)
{
	auto it = std::find(KeyCache.begin(), KeyCache.end(), Key);
	bool Found = it != KeyCache.end();
	bool CanInsert = KeyCache.size() < MAX_CONTAINER_SIZE && Input.size() < MAX_KEYS && (!Found);
	if (Inserting && CanInsert)
	{
		Input.push_back(Key);
	}
	else if (!Inserting)
	{
		// TODO
	}
};


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

		// Debug Tools
		if (KeyDown) PrintVectorElements();
	};
	
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
};

LRESULT CALLBACK SubWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
	{

		return 0;
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

		SUB_WINDOW = CreateWindowEx(0, wc.lpszClassName, L"New Input", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);

		ShowWindow(SUB_WINDOW, SW_SHOW);
		UpdateWindow(SUB_WINDOW);

		SetForegroundWindow(SUB_WINDOW);
		EnableWindow(WINDOW, FALSE);
	}
	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
	{
		// Global
		TEXT = CreateWindowEx(0,L"STATIC",L"Use the 'NEW' button to create a new keybind. 'DELETE' will remove them!",WS_VISIBLE | WS_CHILD,0, 10, 300, 300,hwnd,NULL,NULL,NULL);
		LIST_BOX = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_SORT, 10, 100, 300, 150, hwnd, (HMENU)3, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		// Local
		HWND NewButton = CreateWindowEx(0,L"BUTTON",L"NEW",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,10, 50,100, 30,hwnd,(HMENU)1,(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
		HWND DeleteButton = CreateWindowEx(0,L"BUTTON",L"DELETE",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,150, 50,100, 30,hwnd,(HMENU)2,(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
		

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

	// TODO: 
	// 1.) create a button that adds, listens to bind, awaits user confirmation, then adds to list
	// 2.) a list with all the buttons corresponding to specific keybinds
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
