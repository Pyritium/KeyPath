#pragma once
#include <vector>
#include <string>
#include <array>
#include <map>
#include <Windows.h>

enum DataType {
	TYPE_RECORDED_INPUT,
	TYPE_KEY_CONTAINER,
	TYPE_NULL
};

enum KeyType {
	KEY_DOWN,
	KEY_UP
};

struct Key {
	KeyType State;
	DWORD KeyCode;
};

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
typedef std::vector<DWORD> RECORDED_INPUT;