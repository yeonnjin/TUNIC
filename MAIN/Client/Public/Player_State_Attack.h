#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Attack abstract : public CState
{
protected:
	CPlayer_State_Attack();
	virtual ~CPlayer_State_Attack() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() = 0;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) = 0;
	// 상태 변경 시 호출
	virtual void OnStateExit() = 0;

protected:
	class CPlayer*			m_pPlayer = { nullptr };
	class CPlayer_Weapon*	m_pWeapon = { nullptr };
	_uint					m_iCombo = { 0 };
	_uint					m_iMaxCombo = { 0 };
	_float					m_fComboTime = { 0.f };
	_uint					m_iKey = { 0 };

public:
	virtual void Free() override;
};

END
