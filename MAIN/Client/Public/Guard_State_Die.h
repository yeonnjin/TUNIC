#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CGuard_State_Die final : public CState
{
private:
	CGuard_State_Die(class CMonster_Guard* pMonster, class CPlayer* pPlayer);
	virtual ~CGuard_State_Die() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	class CMonster_Guard*		m_pMonster = { nullptr };
	class CPlayer*				m_pPlayer = { nullptr };

public:
	static CGuard_State_Die* Create(class CMonster_Guard* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END

