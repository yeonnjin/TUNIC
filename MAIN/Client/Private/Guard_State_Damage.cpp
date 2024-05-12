#include "stdafx.h"

#include "Player.h"
#include "Monster_Guard.h"
#include "Guard_State_Damage.h"

CGuard_State_Damage::CGuard_State_Damage(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CGuard_State_Damage::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Guard::ANIM_HURT);
    m_pGameInstance->Play_Once(TEXT("MONSTER_Guard_Hurt.wav"), CSound_Manager::MONSTER4);
}

void CGuard_State_Damage::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    if (m_fAccChangeTime > m_fChangeTime)
    {
        if (true == m_pMonster->Get_isFinished(CMonster_Guard::ANIM_HURT))
        {
            m_pMonster->Change_State(CMonster_Guard::STATE_AGGRO);
            m_fAccChangeTime = 0.f;
        }
    }
}

void CGuard_State_Damage::OnStateExit()
{
    m_fAccChangeTime = 0.f;
}

CGuard_State_Damage* CGuard_State_Damage::Create(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    CGuard_State_Damage* pInstance = new CGuard_State_Damage(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_State_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_State_Damage::Free()
{
    __super::Free();
}
