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


// MACRO
#define		IF_PLAYER_ISMOVE											\
			if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS) ||	\
			m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS) ||		\
			m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) ||		\
			m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))			

#define		CAM_DISTANCE _vector{ 0.f, 12.8, -13.f };
