#include "stdafx.h"

#include "Player.h"
#include "Player_State_Attack.h"

CPlayer_State_Attack::CPlayer_State_Attack(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Attack::OnStateEnter()
{
    // TODO:무기 별 상태 처리 해줘야 함
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK1);
    //m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon"), true);
}

void CPlayer_State_Attack::OnStateUpdate(_float fTimeDelta)
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

void CPlayer_State_Attack::OnStateExit()
{
   // m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon"), false);
}

CPlayer_State_Attack* CPlayer_State_Attack::Create(CPlayer* pPlayer)
{
    CPlayer_State_Attack* pInstance = new CPlayer_State_Attack(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Attack::Free()
{
    __super::Free();
}