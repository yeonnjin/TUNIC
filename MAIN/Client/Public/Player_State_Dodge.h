#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Dodge final : public CState
{
private:
	CPlayer_State_Dodge(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Dodge() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool	m_isEndDash = { false };

	_float	m_fSP = { 0.8f };
	_float	m_fDashSP = { 0.5f };

	_vector m_vPreRight = {};
	_vector m_vPreUp = {};
	_vector	m_vPreLook = {};

private:
	class CPlayer* m_pPlayer = { nullptr };
	CPlayer::DODGE	m_eDodge = { CPlayer::DODGE_END };
	CPlayer::ANIMATION m_eAnim = { CPlayer::ANIM_END };



public:
	static CPlayer_State_Dodge* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END