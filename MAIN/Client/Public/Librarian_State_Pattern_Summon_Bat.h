#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CLibrarian_State_Pattern_Summon_Bat final : public CState
{
private:
	CLibrarian_State_Pattern_Summon_Bat(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Pattern_Summon_Bat() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_vector		m_vPositions[4];
	_uint		m_vIndices[4];

	_uint	m_iPattern = { 0 };

	_float	m_fAccLoopTime = { 0.f };
	_float	m_fLoopTime = { 1.f };

private:
	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CLibrarian_State_Pattern_Summon_Bat* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END