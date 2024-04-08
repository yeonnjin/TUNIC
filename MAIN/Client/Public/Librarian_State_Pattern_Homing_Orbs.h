#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CLibrarian_State_Pattern_Homing_Orbs final : public CState
{
private:
	CLibrarian_State_Pattern_Homing_Orbs(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Pattern_Homing_Orbs() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_uint	m_iPattern = { 0 };
	_uint	m_iOrbPattern = { 0 };
	_uint	m_iActiveOrb = { 0 };

	_float	m_fAccLoopTime = { 0.f };
	_float	m_fLoopTime = { 1.5f };

	vector<class CLibrarian_Effect_Orb*>	m_Orbs;

private:
	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

private:
	void Set_Orbs_Line();
	void Set_Orbs_Hexagon();

public:
	static CLibrarian_State_Pattern_Homing_Orbs* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END