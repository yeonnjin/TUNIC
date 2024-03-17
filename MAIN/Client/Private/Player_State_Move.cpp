#include "stdafx.h"

#include "Player.h"
#include "Player_State_Move.h"

CPlayer_State_Move::CPlayer_State_Move(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Move::OnStateEnter()
{
    // TODO:연달아서 누르면 블렌딩 안되는 듯

    m_eDir = m_pPlayer->Get_Dir();
    switch (m_eDir)
    {
    case CPlayer::DIR_FORWARD:
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
        break;
    case CPlayer::DIR_BACKWARD:
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_BACKWARD);
        break;
    case CPlayer::DIR_LEFT:
        //m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_LEFT);
        break;
    case CPlayer::DIR_RIGHT:
        //m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_RIGHT);
        break;
    case CPlayer::DIR_END:
        break;
    default:
        break;
    }
}

void CPlayer_State_Move::OnStateUpdate(_float fTimeDelta)
{
    switch (m_eDir)
    {
    case CPlayer::DIR_FORWARD:
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Straight(fTimeDelta);
        break;
    case CPlayer::DIR_BACKWARD:
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Backward(fTimeDelta);
        break;
    case CPlayer::DIR_LEFT:
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Left(fTimeDelta);
        //((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Backward(fTimeDelta);
        break;
    case CPlayer::DIR_RIGHT:
        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Right(fTimeDelta);
        //((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Backward(fTimeDelta);
        break;
    case CPlayer::DIR_END:
        break;
    default:
        break;
    }
}

void CPlayer_State_Move::OnStateExit()
{
}

CPlayer_State_Move* CPlayer_State_Move::Create(CPlayer* pPlayer)
{
    CPlayer_State_Move* pInstance = new CPlayer_State_Move(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Move"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Move::Free()
{
    __super::Free();
}