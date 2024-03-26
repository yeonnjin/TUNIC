#pragma once

#include "Base.h"

/*
애니메이션 상태의 상호 작용을 제어하는 그래프. 각 상태는 애니메이션 블렌드 트리나 단일 애니메이션 클립을 참조
*/
BEGIN(Engine)

class CState;

class CState_Machine final : public CBase
{
private:
	CState_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CState_Machine(const CState_Machine& rhs);
	virtual ~CState_Machine() = default;

public:
	void							Add_State(_uint iState, class CState* pAddState);
	void							Update_State(_float fTimeDelta);
	void							Change_State(_uint iState);

public:
	HRESULT							Initialize_Prototype();
	HRESULT							Initialize();

private:
	CState*							Find_State(_uint iState);

private:
	CState*							pCurrent_State = { nullptr };
	unordered_map<_uint, CState*>	m_States;

public:
	static CState_Machine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CState_Machine* Clone();
	virtual void Free() override;
};

END