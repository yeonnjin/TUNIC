#pragma once

#include "Base.h"

/*
애니메이션 상태의 상호 작용을 제어하는 그래프. 각 상태는 애니메이션 블렌드 트리나 단일 애니메이션 클립을 참조
*/
BEGIN(Engine)

class CState_Machine final : public CBase
{
private:
	CState_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CState_Machine(const CState_Machine& rhs);
	virtual ~CState_Machine() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() = 0;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) = 0;
	// Root Motion 제어
	virtual void OnStateMove(_float fTimeDelta) = 0;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() = 0;

	// 타겟 매칭
	// 캐릭터의 손이나 발이 특정 지점에 특정 순간에 닿는 방식으로 움직임
	// https://docs.unity3d.com/kr/560/Manual/TargetMatching.html

public:
	static CState_Machine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END