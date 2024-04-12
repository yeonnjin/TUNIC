#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Idle.h"

CCowBot_State_Idle::CCowBot_State_Idle(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Idle::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_IDLE);

    m_fRandomTime = rand() % 3 + 1;
}

void CCowBot_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    if (m_fAccChangeTime > m_fChangeTime)
    {
        _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

        if (FIND_DISTANCE > fDistance)
        {
            m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
            m_fIdleTime = 0.f;
        }
        // 일정 시간이 지나면 기본 상태를 멈추고 이동 상태로 들어감
        else
        {
            m_fIdleTime += fTimeDelta;
            if (m_fIdleTime >= m_fRandomTime)
            {
                m_pMonster->Change_State(CMonster_CowBot::STATE_WALK);
                m_fIdleTime = 0.f;
            }
        }
    }   
}

void CCowBot_State_Idle::OnStateExit()
{
    m_fIdleTime = 0.f;
    m_fAccChangeTime = 0.f;
}

CCowBot_State_Idle* CCowBot_State_Idle::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Idle* pInstance = new CCowBot_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Idle::Free()
{
    __super::Free();
}
