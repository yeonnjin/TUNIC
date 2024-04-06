#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Move final : public CState
{
private:
	CPlayer_State_Move(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Move() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	class CPlayer*	m_pPlayer = { nullptr };
	CPlayer::DIR	m_eCurDir = { CPlayer::DIR_FRONT };
	CPlayer::DIR	m_ePreDir = { CPlayer::DIR_FRONT };

	_bool			m_isFirst = { true };
	_bool			m_isTurn = { false };
	_int			m_iAngle = { 0 };

	_vector			m_vLook = {};
	_float3			m_vLerpLook = {};

public:
	static CPlayer_State_Move* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END

