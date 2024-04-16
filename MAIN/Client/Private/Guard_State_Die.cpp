#include "stdafx.h"

#include "Player.h"
#include "Monster_Guard.h"
#include "Guard_State_Die.h"

CGuard_State_Die::CGuard_State_Die(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
	m_pMonster = pMonster;
	m_pPlayer = pPlayer;
}

void CGuard_State_Die::OnStateEnter()
{
	m_pMonster->Set_Blending(true, CMonster_Guard::ANIM_DIE);
}

void CGuard_State_Die::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_Guard::ANIM_DIE))
    {
        m_pMonster->Set_Dead(true);
    }
}

void CGuard_State_Die::OnStateExit()
{
}

CGuard_State_Die* CGuard_State_Die::Create(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    CGuard_State_Die* pInstance = new CGuard_State_Die(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_State_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_State_Die::Free()
{
    __super::Free();
}
