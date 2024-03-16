#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CState final : public CBase
{
private:
	CState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CState(const CState& rhs);
	virtual ~CState() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() = 0;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) = 0;
	// Root Motion 제어
	//virtual void OnStateMove(_float fTimeDelta) = 0;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() = 0;

	// 타겟 매칭
	// 캐릭터의 손이나 발이 특정 지점에 특정 순간에 닿는 방식으로 움직임
	// https://docs.unity3d.com/kr/560/Manual/TargetMatching.html

public:
	static CState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END