#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CBlob_State_Jump final : public CState
{
private:
	CBlob_State_Jump(class CMonster_Blob* pMonster, class CPlayer* pPlayer);
	virtual ~CBlob_State_Jump() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_float m_fAccJumpTime = { 0.f };
	_float m_fJumpTime = { 0.4f };

	_float m_fIdleDistance = { 5.f };
	_float m_fAttackDistance = { 2.f };

private:
	class CMonster_Blob* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CBlob_State_Jump* Create(class CMonster_Blob* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END