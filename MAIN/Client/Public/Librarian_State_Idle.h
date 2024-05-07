#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CLibrarian_State_Idle final : public CState
{
private:
	CLibrarian_State_Idle(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Idle() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool		m_isTargetPosition = { true };
	_bool		m_isClockwise = { true };
	_bool		m_isUp = { false };
	_bool		m_isRetreat = { false };
	_bool		m_isLook = { true };

	_vector		m_vOriginPosition = {};
	_vector		m_vTargetPosition = {};
	_vector		m_vEnterPosition = {};
	_vector		m_vTargetDir = {};

	_float		m_fRadian = { 0.f };
	_float		m_fHeight = { 1.f };
	_float		m_fTargetHeight = { 0.f };

	_float		m_fAccTargetTime = { 0.f };
	_float		m_fTargetTime = { 3.f };

	_float		m_fAccHeightTime = { 0.f };
	_float		m_fHeightTime = { 3.f };

	_float		m_fAccIdleTime = { 0.f };
	_float		m_fIdleTime = { 5.f };

	_float		m_fAccLookTime = { 0.f };
	_float		m_fLookTime = { 3.f };

private:
	class CTransform* m_pMonsterTransform = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	class CEasing* m_pEasing = { nullptr };

private:
	void	Compute_TargetPosition();
	void	Compute_Height(_float fTimeDelta);

public:
	static CLibrarian_State_Idle* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END