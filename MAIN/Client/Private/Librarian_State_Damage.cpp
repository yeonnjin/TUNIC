#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Damage.h"

CLibrarian_State_Damage::CLibrarian_State_Damage(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CLibrarian_State_Damage::OnStateEnter()
{
    // 패턴
    // 0. ANIM_FLINCH
    // 1. 애니메이션 재생 중 추가 데미지 맞으면 다시 처음부터 재생

    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLINCH);
}

void CLibrarian_State_Damage::OnStateUpdate(_float fTimeDelta)
{
    m_fAccDamageTime += fTimeDelta;

    // 공격 받았을 때, m_isDamage 가 1틱만 true 여야 함~.~
    if(false == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_FLINCH) && true == m_pMonster->Get_isDamage())
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLINCH);

    // 데미지 가능 시간 지나면 강제로 상태 종료
    if (true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_FLINCH) || (m_fAccDamageTime >= m_fDamageTime))
    {
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
        m_pMonster->Set_isDamage(false);
        m_pMonster->Set_Damage_AccCoolTime(0.f);
    }
}

void CLibrarian_State_Damage::OnStateExit()
{
    m_fAccDamageTime = 0.f;
}

CLibrarian_State_Damage* CLibrarian_State_Damage::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Damage* pInstance = new CLibrarian_State_Damage(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Damage::Free()
{
    __super::Free();
}
