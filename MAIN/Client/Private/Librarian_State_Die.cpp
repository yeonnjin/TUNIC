#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Die.h"

CLibrarian_State_Die::CLibrarian_State_Die(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CLibrarian_State_Die::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_DIE);
}

void CLibrarian_State_Die::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_DIE))
    {
        m_pMonster->Set_Dead(true);
    }
}

void CLibrarian_State_Die::OnStateExit()
{
}

CLibrarian_State_Die* CLibrarian_State_Die::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Die* pInstance = new CLibrarian_State_Die(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Die::Free()
{
    __super::Free();
}
