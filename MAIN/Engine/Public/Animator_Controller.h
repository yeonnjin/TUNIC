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
public:
	enum STATE { STATE_PREV, STATE_CUR, STATE_END };

private:
	CAnimator_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//CAnimator_Controller(const CAnimator_Controller& rhs);
	virtual ~CAnimator_Controller() = default;

public:
	// Animation (블렌딩 속도, 프레임 속도 등등 제어)
	HRESULT					Play_Animation(_float fTimeDelta);
	HRESULT					Set_Animation_Index(_uint iAnimIndex);
	HRESULT					Set_Animation_Transform(CTransform* pObjectTransform);

	// 공통 적용
	HRESULT					Blending_Animation(_uint iNextAnimIndex, _float fTimeDelta);	// 애니메이션 마다 지정X, 상태(State) 바뀔 때, 루프 돌 때 마다 자동 블렌딩
	HRESULT					Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop);
	void					Set_Animation_isRoot(_uint iAnimIndex, _bool isRoot);
	void					Set_Blend_Time(_uint iAnimIndex, _float fBlendTime);
	void					Set_Frame_Tick(_uint iAnimIndex, _uint iStartFrame, _uint iEndFrame, _float fTickWeight);

	_uint					Get_Current_Frame(_uint iAnimIndex);

	// Test
	_bool					isFinished();
	_bool					isFinished(_uint iAnimIndex);

public:				
	HRESULT					Set_Avatar(CAvatar* pAvatar);

public:
	HRESULT					Initialize_Prototype();
	HRESULT					Initialize(void* pArg);

private:
	CAvatar*				m_pAvatar = { nullptr };

private:
	_uint					m_iNumAnimations = { 0 };
	vector<CAnimation*>*	m_pAnimations = { nullptr };
	_uint					m_iNumBones = { 0 };
	vector<CBone*>*			m_pBones = { nullptr };
	

private:
	
	_uint					m_iCurrentAnimIndex = { 0 };

	// Blending
	_bool					m_isBlending = { false };
	_uint					m_iPrevAnimIndex = {};
	vector<CHANNELSTATE>	m_ChannelStates[STATE_END];
	vector<_uint>			m_ChannelBoneIndex;

	// Root
	_float3					m_vRootDistance[STATE_END] = {};
	CTransform*				m_pTargetTransform = { nullptr };

	// Test
	_bool					m_isFinished = { false };
	_bool					m_isChanged = { false };
	_bool					m_isNeedRoot = { false };

	_bool					m_isSetInfo = { false };
	_float					m_fTime = { 0.f };

private:
	// 블렌딩
	HRESULT					Set_Blending_Info();
	HRESULT					Update_Blending(_float fTimeDelta);
	void					Update_RootMotion(_float fTimeDelta);

public:
	static CAnimator_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAnimator_Controller* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END