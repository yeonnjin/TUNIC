#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Walk.h"

CCowBot_State_Walk::CCowBot_State_Walk(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Walk::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_WALK);

    _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    m_vTargetPosition = _vector{    vMonsterPosition.m128_f32[0] + (_float)(rand() % 20 + 1) - (_float)(rand() % 20 + 1), 
                                    vMonsterPosition.m128_f32[1], 
                                    vMonsterPosition.m128_f32[2] + (_float)(rand() % 20 + 1) - (_float)(rand() % 20 + 1), 1.f};
}

void CCowBot_State_Walk::OnStateUpdate(_float fTimeDelta)
{
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));

    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    if (FIND_DISTANCE > XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0])
    {
        m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
    }
    else
    {
       pMonsterTransform->Look_At_For_LandOject(m_vTargetPosition, true);

        if (4 > XMVector3Length(m_vTargetPosition - vMonsterPosition).m128_f32[0])
            m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
    }   
}

void CCowBot_State_Walk::OnStateExit()
{
    m_fRandomTime = 0.f;
}

CCowBot_State_Walk* CCowBot_State_Walk::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Walk* pInstance = new CCowBot_State_Walk(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Walk::Free()
{
    __super::Free();
}
