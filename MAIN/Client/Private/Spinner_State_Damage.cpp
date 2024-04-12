#include "stdafx.h"

#include "Monster_Spinner.h"
#include "Spinner_State_Damage.h"

CSpinner_State_Damage::CSpinner_State_Damage(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CSpinner_State_Damage::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_RECOIL);
}

void CSpinner_State_Damage::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    if (m_fAccChangeTime > m_fChangeTime)
    {
        if (true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_RECOIL))
        {
            m_pMonster->Change_State(CMonster_Spinner::STATE_IDLE);
        }
    }  
}

void CSpinner_State_Damage::OnStateExit()
{
    m_fAccChangeTime = 0.f;
}

CSpinner_State_Damage* CSpinner_State_Damage::Create(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    CSpinner_State_Damage* pInstance = new CSpinner_State_Damage(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CSpinner_State_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinner_State_Damage::Free()
{
    __super::Free();
}