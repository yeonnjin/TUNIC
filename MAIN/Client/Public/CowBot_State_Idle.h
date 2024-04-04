#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CCowBot_State_Idle final : public CState
{
private:
	CCowBot_State_Idle(class CMonster_CowBot* pMonster, class CPlayer* pPlayer);
	virtual ~CCowBot_State_Idle() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_float					m_fIdleTime = { 0.f };
	_float					m_fRandomTime = { 0.f };

private:
	class CMonster_CowBot* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CCowBot_State_Idle* Create(class CMonster_CowBot* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END