#include "stdafx.h"

#include "Player.h"
#include "Player_State_Open.h"

CPlayer_State_Open::CPlayer_State_Open(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Open::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_OPEN_CHEST);

    // ANIM_TAKE_SWORD
}

void CPlayer_State_Open::OnStateUpdate(_float fTimeDelta)
{
}

void CPlayer_State_Open::OnStateExit()
{
}

CPlayer_State_Open* CPlayer_State_Open::Create(CPlayer* pPlayer)
{
    CPlayer_State_Open* pInstance = new CPlayer_State_Open(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Open"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Open::Free()
{
    __super::Free();
}