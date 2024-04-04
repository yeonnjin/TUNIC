#include "stdafx.h"

#include "Player.h"
#include "Player_State_LockOn.h"

CPlayer_State_LockOn::CPlayer_State_LockOn(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_LockOn::OnStateEnter()
{
    m_pGameInstance->Change_Camera(TEXT("Camera_LockOn"));
}

void CPlayer_State_LockOn::OnStateUpdate(_float fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, KEY_UP))
    {
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_LockOn::OnStateExit()
{
    
}

CPlayer_State_LockOn* CPlayer_State_LockOn::Create(CPlayer* pPlayer)
{
    CPlayer_State_LockOn* pInstance = new CPlayer_State_LockOn(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_LockOn"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_LockOn::Free()
{
    __super::Free();
}
