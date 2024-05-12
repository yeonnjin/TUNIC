#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CPlayer_State_Top final : public CState
{
private:
	CPlayer_State_Top(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Top() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool			m_isMove = { false };
	_bool			m_isPreMove = { false };

	_bool			m_isIdle = { false };
	_bool			m_isSuccess = { false };
	_bool			m_isFly = { false };
	_bool			m_isFadeOut = { false };

	_float			m_fAccEnterTime = { 0.f };
	_float			m_fEnterTime = { 2.f };

	_float			m_fAccFlyTime = { 0.f };
	_float			m_fFlyTime = { 1.f };

	_float			m_fAccChangeTime = { 0.f };
	_float			m_fChangeTime = { 3.f };

	_vector			m_vOriginPosition = {};
	_vector			m_vFlyPosition = {};
	_vector			m_vEndPosition = {};

	vector<_vector> m_vAxis;

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CUI_Arrow* m_pUIArrow = { nullptr };
	class CEasing* m_pEasing = { nullptr };

private:
	void	Player_Move(_float fTimeDelta);
	void	Input_Arrow();
	void	Success(_float fTimeDelta);
	void	Set_Axis();

public:
	static CPlayer_State_Top* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END