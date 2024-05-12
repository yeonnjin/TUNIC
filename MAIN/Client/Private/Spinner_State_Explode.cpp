#include "stdafx.h"

#include "Monster_Spinner.h"
#include "Spinner_State_Explode.h"

CSpinner_State_Explode::CSpinner_State_Explode(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CSpinner_State_Explode::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_EXPLODE);
    m_pGameInstance->Play_Once(TEXT("MONSTER_Spinner_Death.wav"), CSound_Manager::MONSTER2);
}

void CSpinner_State_Explode::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_EXPLODE))
    {
        m_pMonster->Set_Dead(true);
        m_pMonster->Clone_Gem();
    }
}

void CSpinner_State_Explode::OnStateExit()
{
}

CSpinner_State_Explode* CSpinner_State_Explode::Create(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    CSpinner_State_Explode* pInstance = new CSpinner_State_Explode(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CSpinner_State_Explode"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinner_State_Explode::Free()
{
    __super::Free();
}