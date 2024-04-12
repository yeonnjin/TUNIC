#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CSpinner_State_Attack final : public CState
{
private:
	CSpinner_State_Attack(class CMonster_Spinner* pMonster, class CPlayer* pPlayer);
	virtual ~CSpinner_State_Attack() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool					m_isAttack = { false };

	_vector					m_vPrePosition = {};

	_float					m_fAccSpinTime = { 0.f };
	_float					m_fSpinTime = { 0.4f };

	_float					m_fAccChangeTime = { 0.f };
	_float					m_fChangeTime = { 0.4f };

private:
	class CMonster_Spinner* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CSpinner_State_Attack* Create(class CMonster_Spinner* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END
