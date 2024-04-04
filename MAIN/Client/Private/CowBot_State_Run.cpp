#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Run.h"

CCowBot_State_Run::CCowBot_State_Run(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Run::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_RUN);
}

void CCowBot_State_Run::OnStateUpdate(_float fTimeDelta)
{
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));

    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    pMonsterTransform->Look_At_For_LandOject(vPlayerPosition, true);
    pMonsterTransform->Go_Backward(fTimeDelta);

    _vector vDistance = vPlayerPosition - vMonsterPosition;

    if (4 > XMVector3Length(vDistance).m128_f32[0])
        m_pMonster->Change_State(CMonster_CowBot::STATE_ATTACK);

    else if (15 < XMVector3Length(vDistance).m128_f32[0])
    {
        m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
    }
}

void CCowBot_State_Run::OnStateExit()
{
}

CCowBot_State_Run* CCowBot_State_Run::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Run* pInstance = new CCowBot_State_Run(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Run::Free()
{
    __super::Free();
}

