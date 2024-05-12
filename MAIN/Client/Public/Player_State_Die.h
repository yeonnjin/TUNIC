#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Die final : public CState
{
private:
	CPlayer_State_Die(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Die() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool	m_isAnimFinish = { false };

	_uint	m_eLevel = {};

	_vector m_vRespawnPosition = {};

private:
	class CPlayer* m_pPlayer = { nullptr };

private:
	void	Set_RespawnPosition();

public:
	static CPlayer_State_Die* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END



