#pragma once

#include "Client_Defines.h"
#include "State.h"

// 우->좌 / 좌->우
BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCowBot_State_Attack final :  public CState
{
private:
	CCowBot_State_Attack(class CMonster_CowBot* pMonster, class CPlayer* pPlayer, class CCowBot_Weapon* pWeapon);
	virtual ~CCowBot_State_Attack() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool					m_isCombo = { false };

	_float					m_fAccChangeTime = { 0.f };
	_float					m_fChangeTime = { 0.4f };

private:
	class CTransform*		m_pMonsterTransform = { nullptr };
	class CTransform*		m_pPlayerTransform = { nullptr };

	class CMonster_CowBot*	m_pMonster = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	class CCowBot_Weapon*	m_pWeapon = { nullptr };

public:
	static CCowBot_State_Attack* Create(class CMonster_CowBot* pMonster, class CPlayer* pPlayer, class CCowBot_Weapon* pWeapon);
	virtual void Free() override;
};

END