#include "stdafx.h"

#include "Player.h"
#include "Player_Weapon.h"
#include "Player_State_Attack_Sword.h"

CPlayer_State_Attack_Sword::CPlayer_State_Attack_Sword(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
    m_pPlayer = pPlayer;
    m_pWeapon = pWeapon;
}

void CPlayer_State_Attack_Sword::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_SWORD1);
    //m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Sword"), true);
}

void CPlayer_State_Attack_Sword::OnStateUpdate(_float fTimeDelta)
{
    m_fComboTime += fTimeDelta;

    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 했을 때 : 2번째 콤보
    if (0 == m_iCombo && 0.2f < m_fComboTime && false == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_SWORD1) && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
    {
        ++m_iCombo;
        m_fComboTime = 0.f;
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_SWORD2);
    }

    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 못했을 때 : 상태 종료w
    if (0 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_SWORD1))
    {
        m_iCombo = 0;
        m_fComboTime = 0.f;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }

    // 두 번째 콤보가 끝나기 전까지 추가 공격을 했을 때 : 3번째 콤보
    if (1 == m_iCombo && 0.2f < m_fComboTime && false == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_SWORD2) && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
    {
        ++m_iCombo;
        m_fComboTime = 0.f;
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_SWORD3);
    }

    // 두 번째 콤보가 끝나기 전까지 추가 공격을 못했을 때 : 상태 종료
    if (1 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_SWORD2))
    {
        m_iCombo = 0;
        m_fComboTime = 0.f;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }

    // 세 번째 콤보가 끝나면 상태 종료
    if (2 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_SWORD3))
    {
        m_iCombo = 0;
        m_fComboTime = 0.f;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }


    //m_pModelCom->Set_SlowMotion(ANIM_SWING_STICK1, 16, 26, 0.f);
    //m_pModelCom->Set_SlowMotion(ANIM_SWING_STICK1, 16, 26, 0.2f);
}

void CPlayer_State_Attack_Sword::OnStateExit()
{
    m_iCombo = 0;
    m_fComboTime = 0.f;
    //m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Sword"), false);
}

CPlayer_State_Attack_Sword* CPlayer_State_Attack_Sword::Create(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
    CPlayer_State_Attack_Sword* pInstance = new CPlayer_State_Attack_Sword(pPlayer, pWeapon);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Attack_Sword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Attack_Sword::Free()
{
    __super::Free();
}

