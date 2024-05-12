#include "stdafx.h"

#include "Player.h"
#include "Monster_Guard.h"

#include "Guard_Weapon_Spear.h"
#include "Guard_Weapon_Shield.h"

#include "Guard_State_Block.h"

CGuard_State_Block::CGuard_State_Block(CMonster_Guard* pMonster, CPlayer* pPlayer, CPartObject* pWeapon_Spear, CPartObject* pWeapon_Shield)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_pWeapon_Spear = dynamic_cast<CGuard_Weapon_Spear*>(pWeapon_Spear);
    m_pWeapon_Shield = dynamic_cast<CGuard_Weapon_Shield*>(pWeapon_Shield);
}

void CGuard_State_Block::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Guard::ANIM_BLOCK);
    m_pWeapon_Spear->Set_isAttackFrame(false);
}

void CGuard_State_Block::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    m_pWeapon_Spear->Set_isAttackFrame(false);

    if (m_fAccChangeTime > m_fChangeTime)
    {
        if (true == m_pMonster->Get_isFinished(CMonster_Guard::ANIM_BLOCK))
        {
            m_pMonster->Change_State(CMonster_Guard::STATE_AGGRO);
            m_fAccChangeTime = 0.f;
        }

        // 공격 가능 프레임 : 37 ~ 47
        _uint iFrame = m_pMonster->Get_Current_Frame(CMonster_Guard::ANIM_BLOCK);
        if(37 == iFrame)
            m_pGameInstance->Play_Once(TEXT("MONSTER_Guard_Attack.wav"), CSound_Manager::MONSTER4);

        if (37 <= iFrame && 47 >= iFrame)
            m_pWeapon_Spear->Set_isAttackFrame(true);
        
    }
}

void CGuard_State_Block::OnStateExit()
{
    m_fAccChangeTime = 0.f;

    m_pWeapon_Spear->Set_isAttackFrame(false);
}

CGuard_State_Block* CGuard_State_Block::Create(CMonster_Guard* pMonster, CPlayer* pPlayer, CPartObject* pWeapon_Spear, CPartObject* pWeapon_Shield)
{
    CGuard_State_Block* pInstance = new CGuard_State_Block(pMonster, pPlayer, pWeapon_Spear, pWeapon_Shield);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_State_Block"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_State_Block::Free()
{
    __super::Free();
}
