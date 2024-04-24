#include "stdafx.h"

#include "Player.h"
#include "Player_Weapon.h"
#include "Player_State_Attack_Stick.h"

CPlayer_State_Attack_Stick::CPlayer_State_Attack_Stick(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
	m_pPlayer = pPlayer;
    m_pWeapon = pWeapon;
}

void CPlayer_State_Attack_Stick::OnStateEnter()
{
	m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK1);
    m_pWeapon->Set_isAttackFrame(false);
    m_iKey = m_pWeapon->Get_Key();
}

void CPlayer_State_Attack_Stick::OnStateUpdate(_float fTimeDelta)
{
    m_fComboTime += fTimeDelta;
    m_pWeapon->Set_isAttackFrame(false);

    if (0.2f < m_fComboTime && true == m_pGameInstance->Get_DIKeyState(m_iKey, KEY_DOWN))
        ++m_iNumKeyInput;

    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 했을 때 : 다음 콤보
    if (0 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_STICK1) && 0 < m_iNumKeyInput)
    {
        ++m_iCombo;
        m_fComboTime = 0.f;
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK2);
    }

    // 첫 번째 콤보 공격 가능 프레임 : 28 ~ 36
    if (0 == m_iCombo)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(CPlayer::ANIM_SWING_STICK1);
        if (28 <= iFrame && 36 >= iFrame)
            m_pWeapon->Set_isAttackFrame(true);
    }

    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 못했을 때 : 상태 종료
    if (0 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_STICK1) && 1 > m_iNumKeyInput)
    {
        m_iCombo = 0;
        m_fComboTime = 0.f;

        IF_PLAYER_ISMOVE
            m_pPlayer->Change_State(CPlayer::STATE_MOVE);
        else
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }

    // 두 번째 콤보 공격 가능 프레임 : 20 ~ 33
    if (1 == m_iCombo)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(CPlayer::ANIM_SWING_STICK2);
        if (20 <= iFrame && 33 >= iFrame)
            m_pWeapon->Set_isAttackFrame(true);
    }

    // 두 번째 콤보가 끝나면 상태 종료
    if (1 == m_iCombo && 37 == m_pPlayer->Get_Current_Frame(CPlayer::ANIM_SWING_STICK2))
    {
        m_iCombo = 0;
        m_fComboTime = 0.f;
        m_iNumKeyInput = 0;

        m_pPlayer->Set_AnimationData_Initialize(CPlayer::ANIM_SWING_STICK2);

        IF_PLAYER_ISMOVE
            m_pPlayer->Change_State(CPlayer::STATE_MOVE);
        else
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Attack_Stick::OnStateExit()
{
    m_iCombo = 0;
    m_fComboTime = 0.f;
    m_iNumKeyInput = 0;

    m_pWeapon->Set_isAttackFrame(false);
}

CPlayer_State_Attack_Stick* CPlayer_State_Attack_Stick::Create(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
    CPlayer_State_Attack_Stick* pInstance = new CPlayer_State_Attack_Stick(pPlayer, pWeapon);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Attack_Stick"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Attack_Stick::Free()
{
    __super::Free();
}
