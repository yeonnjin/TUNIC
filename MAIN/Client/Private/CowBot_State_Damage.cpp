#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Damage.h"

CCowBot_State_Damage::CCowBot_State_Damage(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Damage::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_HIT);
}

void CCowBot_State_Damage::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    if (m_fAccChangeTime > m_fChangeTime)
    {
        if (true == m_pMonster->Get_isFinished(CMonster_CowBot::ANIM_HIT))
        {
            m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
        }
    }
}

void CCowBot_State_Damage::OnStateExit()
{
    m_fAccChangeTime = 0.f;
}

CCowBot_State_Damage* CCowBot_State_Damage::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Damage* pInstance = new CCowBot_State_Damage(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Damage::Free()
{
    __super::Free();
}
