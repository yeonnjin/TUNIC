#pragma once

#include "Base.h"

/* 
컨트롤러는 내부에 사용하는 애니메이션 클립에 대한 레퍼런스를 가지고 있으며, 
	다양한 애니메이션 상태와 상태 간 전환을 소위 상태 머신(State Machine) 을 이용하여 관리
	애니메이션 클립이 애니메이터 컨트롤러 안에 배치
	하나의 애니메이터 컨트롤러는 Animator 컴포넌트가 있는 다수의 모델에 의해 참조될 수 있습니다.
*/

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