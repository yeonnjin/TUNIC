#include "stdafx.h"

#include "Player.h"
#include "Player_State_Attack_Stick.h"

CPlayer_State_Attack_Stick::CPlayer_State_Attack_Stick(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Attack_Stick::OnStateEnter()
{
	m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK1);
	//m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Stick"), true);
}

void CPlayer_State_Attack_Stick::OnStateUpdate(_float fTimeDelta)
{
    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 했을 때 : 다음 콤보
    if (0 == m_iCombo && false == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_STICK1) && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
    {
        ++m_iCombo;
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK2);
    }

    // 첫 번째 콤보가 끝나기 전까지 추가 공격을 못했을 때 : 상태 종료
    if (0 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_STICK1))
    {
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }

    // 두 번째 콤보가 끝나면 상태 종료
    if (1 == m_iCombo && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SWING_STICK2))
    {
        m_iCombo = 0;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Attack_Stick::OnStateExit()
{
    //m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Stick"), false);
}

CPlayer_State_Attack_Stick* CPlayer_State_Attack_Stick::Create(CPlayer* pPlayer)
{
    CPlayer_State_Attack_Stick* pInstance = new CPlayer_State_Attack_Stick(pPlayer);

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
