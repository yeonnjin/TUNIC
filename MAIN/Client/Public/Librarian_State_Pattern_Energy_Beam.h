#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CLibrarian_State_Pattern_Energy_Beam final : public CState
{
private:
	CLibrarian_State_Pattern_Energy_Beam(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Pattern_Energy_Beam() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_uint	m_iPattern = { 0 };

	_float	m_fAccLoopTime = { 0.f };
	_float	m_fLoopTime = { 2.f };

private:
	class CLibrarian_Effect_Beam* m_pBeam = { nullptr };
	class CTransform* m_pMonsterTransform = { nullptr };
	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CLibrarian_State_Pattern_Energy_Beam* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END