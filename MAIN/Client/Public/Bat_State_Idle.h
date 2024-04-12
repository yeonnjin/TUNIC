#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CBat_State_Idle final : public CState
{
private:
	CBat_State_Idle(class CMonster_Bat* pMonster, class CPlayer* pPlayer);
	virtual ~CBat_State_Idle() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool					m_isLook = { true };

	_float					m_fAccIdleTime = { 0.f };
	_float					m_fIdleTime = { 2.f };

	_float					m_fAccLookTime = { 0.f };
	_float					m_fLookTime = { 1.f };

	_float					m_fAttackDistance = { 5.f };

	_vector					m_vTargetDir = {};
	_vector					m_vEnterPosition = {};

private:
	class CTransform*		m_pMonsterTransform = { nullptr };
	class CTransform*		m_pPlayerTransform = { nullptr };

	class CMonster_Bat*		m_pMonster = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	class CEasing*			m_pEasing = { nullptr };

public:
	static CBat_State_Idle* Create(class CMonster_Bat* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END

