#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CLibrarian_State_Pattern_Lightning_Warp final : public CState
{
private:
	CLibrarian_State_Pattern_Lightning_Warp(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Pattern_Lightning_Warp() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_uint					m_iPattern = { 0 };

	// 2. 낙하 지점에 번개
	_vector					m_vDropPosition = {};
	_float					m_fAccLightningTime = { 0.f };
	_float					m_fLightningTime = { 3.f };

	// 3. 꽂은 채로 일정 시간 반복
	_float					m_fAccLoopTime = { 0.f };
	_float					m_fLoopTime = { 3.f };

private:
	class CTransform* m_pMonsterTransform = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

private:
	_vector					Get_Drop_Position();

public:
	static CLibrarian_State_Pattern_Lightning_Warp* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END