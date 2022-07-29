#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using Key = int;

// Is ctrl or alt down?
struct ModKeyState {
	bool	ctrl{ false };
	bool	alt{ false };
};