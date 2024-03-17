#include "stdafx.h"

#include "Player.h"
#include "Player_State_Idle.h"

CPlayer_State_Idle::CPlayer_State_Idle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Idle::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);
}

void CPlayer_State_Idle::OnStateUpdate(_float fTimeDelta)
{
}

void CPlayer_State_Idle::OnStateExit()
{
}

CPlayer_State_Idle* CPlayer_State_Idle::Create(CPlayer* pPlayer)
{
    CPlayer_State_Idle* pInstance = new CPlayer_State_Idle(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Idle::Free()
{
    __super::Free();
}
