#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CEasing;
class CTransform;
END

BEGIN(Client)

class CLibrarian_State_Pattern_Lunge_Swipe final : public CState
{
private:
	CLibrarian_State_Pattern_Lunge_Swipe(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Pattern_Lunge_Swipe() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool	m_isSlash = { false };

	_float	m_fOriginRotation = {};
	_float	m_fTargetAngle = { 360.f };

	_vector	m_vOriginPosition = {};
	_vector m_vRetreatPosition = {};
	_vector m_vDashPosition = {};

private:
	class CTransform* m_pMonsterTransform = { nullptr };
	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	class CEasing* m_pEasing = { nullptr };

public:
	static CLibrarian_State_Pattern_Lunge_Swipe* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END