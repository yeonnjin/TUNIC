#include "stdafx.h"

#include "Player.h"
#include "Monster_Bat.h"
#include "Bat_State_Attack.h"

CBat_State_Attack::CBat_State_Attack(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBat_State_Attack::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_ATTACK);
}

void CBat_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_Bat::ANIM_ATTACK))
    {
        m_pMonster->Change_State(CMonster_Bat::STATE_IDLE);
    }
}

void CBat_State_Attack::OnStateExit()
{
}

CBat_State_Attack* CBat_State_Attack::Create(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    CBat_State_Attack* pInstance = new CBat_State_Attack(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBat_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBat_State_Attack::Free()
{
    __super::Free();
}