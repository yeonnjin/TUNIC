#include "stdafx.h"

#include "Player.h"
#include "Monster_Bat.h"
#include "Bat_State_Idle.h"

CBat_State_Idle::CBat_State_Idle(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
	m_pMonster = pMonster;
	m_pPlayer = pPlayer;
}

void CBat_State_Idle::OnStateEnter()
{
	m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_IDLE);
}

void CBat_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;

    if (2.f < m_fIdleTime)
    {
        _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Look_At_For_LandOject(vPlayerPosition, true);
        dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Go_Backward(fTimeDelta);

        if (2 > XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0])
        {
            m_pMonster->Change_State(CMonster_Bat::STATE_ATTACK);
            m_fIdleTime = 0.f;
        }
    }  
}

void CBat_State_Idle::OnStateExit()
{
    m_fIdleTime = 0.f;
}

CBat_State_Idle* CBat_State_Idle::Create(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    CBat_State_Idle* pInstance = new CBat_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBat_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBat_State_Idle::Free()
{
    __super::Free();
}