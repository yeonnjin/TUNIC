#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Idle.h"

CLibrarian_State_Idle::CLibrarian_State_Idle(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CLibrarian_State_Idle::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_IDLE);
}

void CLibrarian_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    /*if (true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_FLYING_IDLE))
    {

    }*/
}

void CLibrarian_State_Idle::OnStateExit()
{
}

CLibrarian_State_Idle* CLibrarian_State_Idle::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Idle* pInstance = new CLibrarian_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Idle::Free()
{
    __super::Free();
}
