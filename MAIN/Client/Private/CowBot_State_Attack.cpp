#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_Weapon.h"
#include "CowBot_State_Attack.h"

CCowBot_State_Attack::CCowBot_State_Attack(CMonster_CowBot* pMonster, CPlayer* pPlayer, CCowBot_Weapon* pWeapon)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
    m_pWeapon = pWeapon;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
}

void CCowBot_State_Attack::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_ATTACK);
    m_pWeapon->Set_isAttackFrame(false);
}

void CCowBot_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    m_pWeapon->Set_isAttackFrame(false);

    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

    m_pMonsterTransform->Look_At_For_LandOject(vPlayerPosition, true);

    // TODO: 코드 최적화하기
    if (m_fAccChangeTime > m_fChangeTime)
    {
        if (false == m_isCombo && 42 == m_pMonster->Get_Current_Frame(CMonster_CowBot::ANIM_ATTACK))
        {
            if (FIND_DISTANCE < fDistance)
                m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
            else if (4 < fDistance)
                m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
            else
            {
                m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_BACKSWIPE);
                m_isCombo = true;
            }
        }

        // 첫 번째 콤보 공격 가능 프레임 : 34 ~ 40
        if (false == m_isCombo)
        {
            _uint iFrame = m_pPlayer->Get_Current_Frame(CMonster_CowBot::ANIM_ATTACK);
            if (34 <= iFrame && 40 >= iFrame)
                m_pWeapon->Set_isAttackFrame(true);
        }

        if (true == m_isCombo && true == m_pMonster->Get_isFinished(CMonster_CowBot::ANIM_BACKSWIPE))
        {
            m_isCombo = false;

            if (FIND_DISTANCE < fDistance)
                m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
            else if (4 < fDistance)
                m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
            else
                m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_ATTACK);
        }

        // 두 번째 콤보 공격 가능 프레임 : 20 ~ 26
        if (true == m_isCombo)
        {
            _uint iFrame = m_pPlayer->Get_Current_Frame(CMonster_CowBot::ANIM_BACKSWIPE);
            if (20 <= iFrame && 26 >= iFrame)
                m_pWeapon->Set_isAttackFrame(true);
        }
    }   
}

void CCowBot_State_Attack::OnStateExit()
{
    m_isCombo = false;

    m_fAccChangeTime = 0.f;
}

CCowBot_State_Attack* CCowBot_State_Attack::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer, CCowBot_Weapon* pWeapon)
{
    CCowBot_State_Attack* pInstance = new CCowBot_State_Attack(pMonster, pPlayer, pWeapon);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Attack::Free()
{
    __super::Free();
}
