#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Idle final : public CState
{
private:
	CPlayer_State_Idle(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Idle() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	class CPlayer*			m_pPlayer = { nullptr };

public:
	static CPlayer_State_Idle* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END