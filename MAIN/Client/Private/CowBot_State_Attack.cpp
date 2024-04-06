#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Attack.h"

CCowBot_State_Attack::CCowBot_State_Attack(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Attack::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_ATTACK);
}

void CCowBot_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));

    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _vector vDistance = vPlayerPosition - vMonsterPosition;

    pMonsterTransform->Look_At_For_LandOject(vPlayerPosition, true);

    // TODO: 코드 최적화하기
    if (false == m_isCombo && true == m_pMonster->Get_isFinished(CMonster_CowBot::ANIM_ATTACK))
    {    
        if(FIND_DISTANCE < XMVector3Length(vDistance).m128_f32[0])
            m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
        else if (4 < XMVector3Length(vDistance).m128_f32[0])
            m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
        else
        {
            m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_BACKSWIPE);
            m_isCombo = true;
        }
    }
     
    if (true == m_isCombo && true == m_pMonster->Get_isFinished(CMonster_CowBot::ANIM_BACKSWIPE))
    {
        m_isCombo = false;

        if (FIND_DISTANCE < XMVector3Length(vDistance).m128_f32[0])
            m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
        else if (4 < XMVector3Length(vDistance).m128_f32[0])
            m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
        else
            m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_ATTACK);
    }
}

void CCowBot_State_Attack::OnStateExit()
{
    m_isCombo = false;
}

CCowBot_State_Attack* CCowBot_State_Attack::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Attack* pInstance = new CCowBot_State_Attack(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Attack::Free()
{
    __super::Free();
}
