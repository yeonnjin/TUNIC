#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimator_Controller final : public CBase
{
private:
	CAnimator_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimator_Controller(const CAnimator_Controller& rhs);
	virtual ~CAnimator_Controller() = default;

public:
	void					Add_State_Machine(_uint iState, class CState_Machine* pStateMachine);
	class CState_Machine*	Find_State_Machine(_uint iState);
	// 블렌딩 - 시간 스케일(Time Scale)

public:
	static CAnimator_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END