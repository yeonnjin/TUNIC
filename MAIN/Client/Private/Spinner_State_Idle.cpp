#include "stdafx.h"

#include "Player.h"
#include "Monster_Spinner.h"
#include "Spinner_State_Idle.h"

CSpinner_State_Idle::CSpinner_State_Idle(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CSpinner_State_Idle::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_IDLE);
    //m_pGameInstance->Play_Once(TEXT("MONSTER_Spinner_Idle.wav"), CSound_Manager::MONSTER2);
}

void CSpinner_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;

    if (1.5f < m_fIdleTime)
        m_pMonster->Change_State(CMonster_Spinner::STATE_ATTACK);
}

void CSpinner_State_Idle::OnStateExit()
{
    m_fIdleTime = 0.f;
}

CSpinner_State_Idle* CSpinner_State_Idle::Create(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    CSpinner_State_Idle* pInstance = new CSpinner_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CSpinner_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinner_State_Idle::Free()
{
    __super::Free();
}