#include "stdafx.h"

#include "Player.h"
#include "Player_State_Dodge.h"

CPlayer_State_Dodge::CPlayer_State_Dodge(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Dodge::OnStateEnter()
{
    m_eDodge = m_pPlayer->Get_Dodge();

    switch (m_eDodge)
    {
    case CPlayer::DODGE_ROLL:
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE);
        m_eAnim = CPlayer::ANIM_DODGE;
        break;
    case CPlayer::DODGE_FAST:
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE_GARBAGE);
        m_eAnim = CPlayer::ANIM_DODGE_GARBAGE;
        break;
    case CPlayer::DODGE_DASH:
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_HYPERDASH);
        m_eAnim = CPlayer::ANIM_HYPERDASH;
        break;
    case Client::CPlayer::DODGE_END:
        break;
    default:
        break;
    }
}

void CPlayer_State_Dodge::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pPlayer->Get_isFinished(m_eAnim))
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