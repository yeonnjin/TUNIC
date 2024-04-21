#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CMerchant_State_Hidden final : public CState
{
private:
	CMerchant_State_Hidden(class CNPC_Merchant* pNPC, class CPlayer* pPlayer);
	virtual ~CMerchant_State_Hidden() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	_bool m_isHidden = { true };

	_float m_fWakeUpDistance = { 7.f };

private:
	class CTransform* m_pNPCTransform = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

	class CNPC_Merchant* m_pNPC = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CMerchant_State_Hidden* Create(class CNPC_Merchant* pNPC, class CPlayer* pPlayer);
	virtual void Free() override;
};

END