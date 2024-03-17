#include "stdafx.h"

#include "Player.h"
#include "Player_State_Dodge.h"

CPlayer_State_Dodge::CPlayer_State_Dodge(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Dodge::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE);
}

void CPlayer_State_Dodge::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_DODGE))
    {
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Dodge::OnStateExit()
{
}

CPlayer_State_Dodge* CPlayer_State_Dodge::Create(CPlayer* pPlayer)
{
    CPlayer_State_Dodge* pInstance = new CPlayer_State_Dodge(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Dodge"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Dodge::Free()
{
    __super::Free();
}