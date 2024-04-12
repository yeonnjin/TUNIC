#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CCowBot_State_Walk final : public CState
{
private:
	CCowBot_State_Walk(class CMonster_CowBot* pMonster, class CPlayer* pPlayer);
	virtual ~CCowBot_State_Walk() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool					m_isLook = { true };

	_float					m_fRandomTime = { 0.f };

	_float					m_fAccLookTime = { 0.f };
	_float					m_fLookTime = { 1.f };

	_float					m_fAccChangeTime = { 0.f };
	_float					m_fChangeTime = { 0.4f };

	_vector					m_vTargetPosition = {};

	_vector					m_vTargetDir = {};
	_vector					m_vEnterPosition = {};

private:
	class CTransform*		m_pMonsterTransform = { nullptr };
	class CTransform*		m_pPlayerTransform = { nullptr };

	class CMonster_CowBot*	m_pMonster = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	class CEasing*			m_pEasing = { nullptr };

public:
	static CCowBot_State_Walk* Create(class CMonster_CowBot* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END