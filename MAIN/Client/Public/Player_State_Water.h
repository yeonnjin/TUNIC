#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Water final : public CState
{
private:
	CPlayer_State_Water(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Water() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_float	m_fEnterX = { 0.f };
	_float	m_fEnterY = { 0.f };

	_float	m_fMidX = { 1.172f };
	_float	m_fMinX = { -0.004f };
	_float	m_fMaxX = { 2.5f };

	_float	m_fMaxDepth = { -0.7f };

private:
	CPlayer::DIR	m_eCurDir = { CPlayer::DIR_FRONT };
	CPlayer::DIR	m_ePreDir = { CPlayer::DIR_FRONT };

	_bool			m_isFirst = { true };
	_bool			m_isTurn = { false };
	_int			m_iAngle = { 0 };

	_vector			m_vLook = {};
	_float3			m_vLerpLook = {};

private:
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CPlayer_State_Water* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END

