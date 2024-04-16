#include "stdafx.h"

#include "Player.h"
#include "Player_State_Damage.h"

CPlayer_State_Damage::CPlayer_State_Damage(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Damage::OnStateEnter()
{
	m_pPlayer->Set_Blending(true, CPlayer::ANIM_STAGGER);

	/*m_eStatus = m_pPlayer->Get_Status();

	switch (m_eStatus)
	{
	case Client::CPlayer::STATUS_HURT:
		m_pPlayer->Set_Blending(true, CPlayer::ANIM_HURT);
		break;
	case Client::CPlayer::STATUS_STAGGER:
		m_pPlayer->Set_Blending(true, CPlayer::ANIM_STAGGER);
		break;
	case Client::CPlayer::STATUS_END:
		break;
	default:
		break;
	}*/
}

void CPlayer_State_Damage::OnStateUpdate(_float fTimeDelta)
{
	m_fAccChangeTime += fTimeDelta;

	if (m_fAccChangeTime > m_fChangeTime)
	{
		if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_STAGGER))
		{
			m_pPlayer->Change_State(CPlayer::STATE_IDLE);
		}
	}



	/*if (CPlayer::STATUS_HURT == m_eStatus && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_HURT))
	{
		m_pPlayer->Change_State(CPlayer::STATE_IDLE);
	}*/

	//if (CPlayer::STATUS_HURT == m_eStatus && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_HURT))
	//{
	//	m_eStatus = CPlayer::STATUS_STAGGER;
	//	m_pPlayer->Set_Blending(true, CPlayer::ANIM_STAGGER);
	//}
	//// TODO:애니메이션 끝나기 전에 전환하면 루트 안됨
	//else if (CPlayer::STATUS_STAGGER == m_eStatus && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_STAGGER))
	//{
	//	m_pPlayer->Set_Blending(true, CPlayer::ANIM_GETUP);
	//}
	//else if (CPlayer::STATUS_STAGGER == m_eStatus && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_GETUP))
	//{
	//	/*m_eStatus = CPlayer::STATUS_HURT;
	//	m_pPlayer->Set_Blending(true, CPlayer::ANIM_HURT);*/
	//	m_eStatus = CPlayer::STATUS_HURT;
	//	m_pPlayer->Change_State(CPlayer::STATE_IDLE);
	//}

	//if (m_pGameInstance->Get_DIKeyState(DIK_L, KEY_DOWN))
	//	m_pPlayer->Change_State(CPlayer::STATE_IDLE);
}

void CPlayer_State_Damage::OnStateExit()
{
	m_fAccChangeTime = 0.f;
}

CPlayer_State_Damage* CPlayer_State_Damage::Create(CPlayer* pPlayer)
{
	CPlayer_State_Damage* pInstance = new CPlayer_State_Damage(pPlayer);

	if (nullptr == pInstance)
	{
		MSG_BOX(TEXT("Failed To Create : CPlayer_State_Damage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_State_Damage::Free()
{
	__super::Free();
}
