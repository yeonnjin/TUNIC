#include "stdafx.h"

#include "Player.h"
#include "Player_State_Defense.h"

CPlayer_State_Defense::CPlayer_State_Defense(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Defense::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SHIELD);   
}

void CPlayer_State_Defense::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_UP))
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);

    // 들고 있을 때 피격 시 -> ANIM_PARRY
}

void CPlayer_State_Defense::OnStateExit()
{
}

CPlayer_State_Defense* CPlayer_State_Defense::Create(CPlayer* pPlayer)
{
    CPlayer_State_Defense* pInstance = new CPlayer_State_Defense(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Defense"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Defense::Free()
{
    __super::Free();
}

