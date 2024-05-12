#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Die.h"

CCowBot_State_Die::CCowBot_State_Die(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CCowBot_State_Die::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_DIE);
    m_pGameInstance->Play_Once(TEXT("MONSTER_CowBot_Death.wav"), CSound_Manager::MONSTER3);
}

void CCowBot_State_Die::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_CowBot::ANIM_DIE))
    {
        m_pMonster->Set_Dead(true);
        m_pMonster->Clone_Gem();
    }
}

void CCowBot_State_Die::OnStateExit()
{
}

CCowBot_State_Die* CCowBot_State_Die::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Die* pInstance = new CCowBot_State_Die(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Die::Free()
{
    __super::Free();
}
