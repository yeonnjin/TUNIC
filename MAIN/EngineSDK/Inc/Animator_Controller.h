#pragma once

#include "Base.h"

/* 
컨트롤러는 내부에 사용하는 애니메이션 클립에 대한 레퍼런스를 가지고 있으며, 
	다양한 애니메이션 상태와 상태 간 전환을 소위 상태 머신(State Machine) 을 이용하여 관리
	애니메이션 클립이 애니메이터 컨트롤러 안에 배치
	하나의 애니메이터 컨트롤러는 Animator 컴포넌트가 있는 다수의 모델에 의해 참조될 수 있습니다.
*/

BEGIN(Engine)

class CState;
class CState_Machine;
class CBone;
class CAvatar;
class CAnimation;
class CTransform;

class CAnimator_Controller final : public CBase
{
private:
	CAnimator_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//CAnimator_Controller(const CAnimator_Controller& rhs);
	virtual ~CAnimator_Controller() = default;

public:
	// State_Machine
	void					Add_State(_uint iState, CState* pAddState);
	void					Update_State(_float fTimeDelta);
	void					Change_State(_uint iState);

	// Animation (블렌딩 속도, 프레임 속도 등등 제어)
	HRESULT					Play_Animation(_float fTimeDelta);
	HRESULT					Set_Animation_Index(_uint iAnimIndex);
	HRESULT					Set_Animation_Transform(CTransform* pObjectTransform);

	// 공통 적용
	HRESULT					Set_Animation_isBlending(_uint iAnimIndex, _bool isBlend);	// 애니메이션 마다 지정X, 상태(State) 바뀔 때, 루프 돌 때 마다 자동 블렌딩
	HRESULT					Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop);

	// 블렌딩 - 시간 스케일(Time Scale)
public:				
	HRESULT					Set_Avatar(CAvatar* pAvatar);

public:
	HRESULT					Initialize_Prototype();
	HRESULT					Initialize(void* pArg);

private:
	CState_Machine*			m_pState_Machine = { nullptr };
	CAvatar*				m_pAvatar = { nullptr };

private:
	vector<CAnimation*>*	m_pAnimations = { nullptr };
	vector<CBone*>*			m_pBones = { nullptr };
	CTransform*				m_pTargetTransform = { nullptr };

private:
	_uint					m_iNumAnimations = { 0 };
	_uint					m_iCurrentAnimIndex = { 0 };

public:
	static CAnimator_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAnimator_Controller* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END