#pragma once

#include "Player_State_Attack.h"

BEGIN(Client)

class CPlayer_State_Attack_Sword final : public CPlayer_State_Attack
{
private:
	CPlayer_State_Attack_Sword(class CPlayer* pPlayer, class CPlayer_Weapon* pWeapon);
	virtual ~CPlayer_State_Attack_Sword() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool	m_isNextCombo = { false };
	_uint	m_iNumKeyInput = { 0 };

public:
	static CPlayer_State_Attack_Sword* Create(class CPlayer* pPlayer, class CPlayer_Weapon* pWeapon);
	virtual void Free() override;
};

END