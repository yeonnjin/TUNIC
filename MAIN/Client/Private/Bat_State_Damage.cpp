#include "stdafx.h"

#include "Player.h"
#include "Monster_Bat.h"
#include "Bat_State_Damage.h"

CBat_State_Damage::CBat_State_Damage(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBat_State_Damage::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_FLINCH);
}

void CBat_State_Damage::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_Bat::ANIM_FLINCH))
    {
        _int iHP = m_pMonster->Get_HP();
        if (0 >= iHP)
            m_pMonster->Set_Dead(true);
        else
            m_pMonster->Change_State(CMonster_Bat::STATE_IDLE);
    }
}

void CBat_State_Damage::OnStateExit()
{
}

CBat_State_Damage* CBat_State_Damage::Create(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    CBat_State_Damage* pInstance = new CBat_State_Damage(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBat_State_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBat_State_Damage::Free()
{
    __super::Free();
}