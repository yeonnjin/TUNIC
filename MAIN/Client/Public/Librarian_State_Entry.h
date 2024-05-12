#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CEasing;
class CTransform;
END

BEGIN(Client)
class CLibrarian_State_Entry final : public CState
{
private:
	CLibrarian_State_Entry(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual ~CLibrarian_State_Entry() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool	m_isTrigger = { false };
	_bool	m_isCheck = { false };
	_bool	m_isActive = { false };

	_float	m_fOriginRotation = {};
	_float	m_fTargetAngle = { 270.f };

private:
	class CTransform* m_pMonsterTransform = { nullptr };
	class CMonster_Librarian* m_pMonster = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	class CEasing* m_pEasing = { nullptr };

public:
	static CLibrarian_State_Entry* Create(class CMonster_Librarian* pMonster, class CPlayer* pPlayer);
	virtual void Free() override;
};

END