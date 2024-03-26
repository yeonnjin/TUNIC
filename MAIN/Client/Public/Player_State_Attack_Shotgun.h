#pragma once

#include "Player_State_Attack.h"

BEGIN(Client)

class CPlayer_State_Attack_Shotgun final : public CPlayer_State_Attack
{
private:
	CPlayer_State_Attack_Shotgun(class CPlayer* pPlayer, class CPlayer_Weapon* pWeapon);
	virtual ~CPlayer_State_Attack_Shotgun() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

public:
	static CPlayer_State_Attack_Shotgun* Create(class CPlayer* pPlayer, class CPlayer_Weapon* pWeapon);
	virtual void Free() override;
};

END