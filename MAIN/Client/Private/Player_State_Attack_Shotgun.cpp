#include "stdafx.h"

#include "Player.h"
#include "Player_State_Attack_Shotgun.h"

CPlayer_State_Attack_Shotgun::CPlayer_State_Attack_Shotgun(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Attack_Shotgun::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SHOTGUN);
	//m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Shotgun"), true);
}

void CPlayer_State_Attack_Shotgun::OnStateUpdate(_float fTimeDelta)
{
	if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_SHOTGUN))
	{
		m_pPlayer->Change_State(CPlayer::STATE_IDLE);
	}
}

void CPlayer_State_Attack_Shotgun::OnStateExit()
{
	//m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon_Shotgun"), false);
}

CPlayer_State_Attack_Shotgun* CPlayer_State_Attack_Shotgun::Create(CPlayer* pPlayer)
{
	CPlayer_State_Attack_Shotgun* pInstance = new CPlayer_State_Attack_Shotgun(pPlayer);

	if (nullptr == pInstance)
	{
		MSG_BOX(TEXT("Failed To Create : CPlayer_State_Attack_Shotgun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_State_Attack_Shotgun::Free()
{
	__super::Free();

}

