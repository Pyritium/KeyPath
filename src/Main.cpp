#include <iostream>
#include "Enum.h"
#include "wchar.h"

RECORDED_INPUT Input;
KEY_CONTAINER KeyCache; // for current pressing

DataType Type = TYPE_NULL;



std::map<KEY_CONTAINER, KEY_CONTAINER> Binds; // saved binds, acts as a cache so we can add to it and then write to save file after closing

template <typename T>
std::wstring FormulateString(T data)
{
	std::wstring wstr;
	
	for (const DWORD key : Input) {
		wchar_t UnicodeChar = static_cast<wchar_t>(key);
		wstr += UnicodeChar;

		std::cout << UnicodeChar << '(' << key << ")," << std::endl;
	};

	return wstr;
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

	switch (Type)
	{
		case TYPE_KEY_CONTAINER:
		{

			break;
		}

		case TYPE_RECORDED_INPUT:
		{
			
			bool CanInsert = Input.size() < MAX_CONTAINER_SIZE && (!Found);
			if (Inserting && CanInsert)
			{
				Input.push_back(Key);
			}
			else if (!Inserting)
			{
				// TODO
			}

			break;
		}
	}

	
	
};


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		// key down
		bool KeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
		KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
		DWORD KeyCode = pKeyBoard->vkCode;

		//std::cout << KeyCode << std::endl;
		//wchar_t msg[32];
		//swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);

		std::cout << KeyCode << std::endl;

		char KeyName[256];
		// bitshift to represent the key in hex
		GetKeyNameTextA((KeyCode << 16), KeyName, sizeof(KeyName));
		EditKeysPressed(KeyCode, KeyDown);

	};
	
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
};

HWND BoundToText;
HWND RecordToText;

LRESULT CALLBACK SubWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	


	HINSTANCE ptr = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	switch (msg) {
	case WM_CREATE:
	{
		
		HWND BindToButton = CreateWindowEx(0, L"BUTTON", L"Bind To:", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 50, 100, 30, hwnd, (HMENU)1, ptr, NULL);
		HWND RecordToButton = CreateWindowEx(0, L"BUTTON", L"Record To:", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 50, 100, 30, hwnd, (HMENU)2, ptr, NULL);
		HWND ConfirmButton = CreateWindowEx(0, L"BUTTON", L"Confirm", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 0, 100, 30, hwnd, (HMENU)3, ptr, NULL);

		// will enable when both boundto & recordto have insert(s)!
		EnableWindow(ConfirmButton, FALSE);

		BoundToText = CreateWindowEx(0, L"STATIC", L"None!", WS_VISIBLE | WS_CHILD, 200, 100, 300, 300, hwnd, NULL, NULL, NULL);
		RecordToText = CreateWindowEx(0, L"STATIC", L"None!", WS_VISIBLE | WS_CHILD, 10, 100, 300, 300, hwnd, NULL, NULL, NULL);
		 

		return 0;
	}
	case WM_COMMAND:
	{
		int ID = LOWORD(wParam);
		std::wstring str = FormulateString(Input);
		switch (ID) {
		case 1:
			
			Type = TYPE_RECORDED_INPUT;
			
			SetWindowText(BoundToText, str.c_str());
			break;
		case 2:
			break;
		case 3:
			break;
		};
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

		SUB_WINDOW = CreateWindowEx(0, wc.lpszClassName, L"New Input", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);

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
		TEXT = CreateWindowEx(0,L"STATIC",L"Use the 'NEW' button to create a new keybind. 'DELETE' will remove them!",WS_VISIBLE | WS_CHILD,0, 10, 300, 300,hwnd,NULL,NULL,NULL);
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
