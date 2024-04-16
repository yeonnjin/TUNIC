#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CGuard_State_Block final : public CState
{
private:
	CGuard_State_Block(class CMonster_Guard* pMonster, class CPlayer* pPlayer, class CPartObject* pWeapon_Spear, class CPartObject* pWeapon_Shield);
	virtual ~CGuard_State_Block() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_float						m_fAccChangeTime = { 0.f };
	_float						m_fChangeTime = { 0.4f };

private:
	class CMonster_Guard*		m_pMonster = { nullptr };
	class CPlayer*				m_pPlayer = { nullptr };

	class CGuard_Weapon_Spear*	m_pWeapon_Spear = { nullptr };
	class CGuard_Weapon_Shield* m_pWeapon_Shield = { nullptr };

public:
	static CGuard_State_Block* Create(class CMonster_Guard* pMonster, class CPlayer* pPlayer, class CPartObject* pWeapon_Spear, class CPartObject* pWeapon_Shield);
	virtual void Free() override;
};

END

