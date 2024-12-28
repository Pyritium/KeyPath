#pragma once
#include <vector>
#include <string>
#include <array>
#include <map>
#include <Windows.h>

// Globals
#define TimeID 1;
const UINT TIMER_INTERVAL = 1000;
const UINT TIMER_GOAL = TIMER_INTERVAL*3;


enum DataType {
	TYPE_RECORDED_INPUT, // For key binding(s)?
	TYPE_KEY_CONTAINER, // For storing the actual key logs?
	TYPE_NULL
};

enum KeyType {
	KEY_DOWN,
	KEY_UP
};

struct KeyInput {
	KeyType State;
	DWORD Data;
	KeyInput(DWORD kc, WPARAM wp) : Data(kc)
	{
		bool KeyDown = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
		State = KeyDown ? KEY_DOWN : KEY_UP;
	};
};


UINT_PTR TimerID = 1;
int ElapsedSeconds;


HHOOK KeyboardHook;
HWND TEXT;
HWND LIST_BOX;
HWND WINDOW;
HWND SUB_WINDOW;
HWND RECORD_WINDOW;


const LPCWSTR CIM_CLASS = L"CIM";
constexpr size_t MAX_CONTAINER_SIZE = 256;
constexpr size_t MAX_KEYS = 5;

// For the current keybind being written out
typedef std::array<DWORD, MAX_KEYS> KEY_CONTAINER;
// Pertains to the keys recorded
typedef std::vector<KeyInput> RECORDED_INPUT;