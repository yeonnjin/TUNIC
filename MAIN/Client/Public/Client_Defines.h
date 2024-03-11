#pragma once

#include "GameInstance.h"

namespace Client
{
	enum LEVEL {	LEVEL_STATIC = 0, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY = 3, LEVEL_END };

	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;
