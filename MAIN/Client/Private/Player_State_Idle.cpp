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
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    if (CPlayer::LOCK_ON_FIND == eLockOn)
    {
        if (CPlayer::STATE_DODGE != m_pPlayer->Get_State())
        {
            _vector vTargetPosition = m_pPlayer->Get_LockOn_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Look_At_For_LandOject(vTargetPosition, true);
        }
    }
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
