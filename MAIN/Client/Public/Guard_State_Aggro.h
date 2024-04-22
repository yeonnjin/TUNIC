#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CGuard_State_Aggro final : public CState
{
private:
	CGuard_State_Aggro(class CMonster_Guard* pMonster, class CPlayer* pPlayer);
	virtual ~CGuard_State_Aggro() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool					m_isLook = { true };

	_float					m_fAccLookTime = { 0.f };
	_float					m_fLookTime = { 1.f };

	_float					m_fAccChangeTime = { 0.f };
	_float					m_fChangeTime = { 0.4f };

	_float					m_fIdleDistance = { 20.f };
	_float					m_fAttackDistance = { 7.f };

	_vector					m_vTargetDir = {};
	_vector					m_vEnterPosition = {};

private:
	class CTransform*		m_pMonsterTransform = { nullptr };
	class CTransform*		m_pPlayerTransform = { nullptr };

	class CMonster_Guard*	m_pMonster = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	class CEasing*			m_pEasing = { nullptr };

public:
	static CGuard_State_Aggro* Create(class CMonster_Guard* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END