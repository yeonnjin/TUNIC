#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CLibrarian_State_Melee final : public CState
{
private:
	CLibrarian_State_Melee(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Melee() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_uint							m_iPattern = { 0 };
	CMonster_Librarian::ANIMATION	m_eAnimation[2];

	_vector							m_vOriginPosition = {};
	_vector							m_vRetreatPosition = {};
	_vector							m_vDashPosition = {};

private:
	class CTransform*				m_pMonsterTransform = { nullptr };
	class CTransform*				m_pPlayerTransform = { nullptr };

	class CMonster_Librarian*		m_pMonster = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };

	class CEasing*					m_pEasing = { nullptr };

private:
	void	Play_Thrust();

public:
	static CLibrarian_State_Melee* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END