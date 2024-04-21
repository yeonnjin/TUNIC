#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CMerchant_State_Idle final : public CState
{
private:
	CMerchant_State_Idle(class CNPC_Merchant* pNPC, class CPlayer* pPlayer);
	virtual ~CMerchant_State_Idle() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool		m_isLook = { true };
	_bool		m_isTargetPosition = { true };

	_vector		m_vEnterPosition = {};
	_vector		m_vTargetDir = {};
	_vector		m_vTargetPosition = {};

	_float		m_fAccLookTime = { 0.f };
	_float		m_fLookTime = { 5.f };

	_float		m_fAccTargetTime = { 0.f };
	_float		m_fTargetTime = { 4.f };

private:
	class CTransform* m_pNPCTransform = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

	class CNPC_Merchant* m_pNPC = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

	class CEasing* m_pEasing = { nullptr };

public:
	static CMerchant_State_Idle* Create(class CNPC_Merchant* pNPC, class CPlayer* pPlayer);
	virtual void Free() override;
};

END