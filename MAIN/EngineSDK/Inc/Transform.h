#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct Transform_Desc
	{
		_float	fSpeedPerSec = { 0.f };
		_float	fRotationPerSec = { 0.f };
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	/*void Set_State(STATE eState, _fvector vState)
	{
		_matrix	WorldMatrix;
		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		WorldMatrix.r[eState] = vState;
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}*/

	// Set
	void Set_State(STATE eState, _fvector vState)
	{
		_float4 vTemp;
		XMStoreFloat4(&vTemp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTemp, sizeof(_float4));
	}

	void Set_State(STATE eState, const _float4& vState)
	{
		memcpy(&m_WorldMatrix.m[eState], &vState, sizeof(_float4));
	}

	void Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ);

	void Set_WorldMatrix(_float4x4 matrix)
	{
		memcpy(&m_WorldMatrix, &matrix, sizeof(_float4x4));
	}

	void Set_Dead(_bool isDead) { m_isDead = isDead; }

	// Get
	_vector Get_State_Vector(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float4 Get_State_Float4(STATE eState) {
		_float4 vTemp;
		XMStoreFloat4(&vTemp, XMLoadFloat4x4(&m_WorldMatrix).r[eState]);
		return vTemp;
	}

	_float3 Get_Scaled() const {
		_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		return _float3(	XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
						XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
						XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	const _float4x4* Get_WorldFloat4x4_Ptr() const {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4_Inverse() const {
		_float4x4 WorldMatrixInverse;
		XMStoreFloat4x4(&WorldMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return WorldMatrixInverse;
	}

	_matrix Get_WorldMatrix_Inverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_bool Get_Dead() { return m_isDead; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta);	
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Look(_float fTimeDelta, _fvector vLook);
	void Look_At(_fvector vAt);
	void Look_At_For_LandOject(_fvector vAt, _bool isReverse = false);
	void Look_At_Dir(_fvector vDir);
	_bool Move_To_Target(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance = 0.f);
	void Turn(_fvector vAxis, _float fTimeDelta);
	_bool Turn_Look(_Out_ _bool* isFirst, _Out_ _float3* vLerpLook, _fvector vTargetLook, _float fTimeDelta);
	_bool Turn_Angle(_fvector vAxis, _float fAngle, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);

	void Go_Front(_float fTimeDelta);
	void Go_FL(_float fTimeDelta);
	void Go_L(_float fTimeDelta);
	void Go_BL(_float fTimeDelta);
	void Go_Back(_float fTimeDelta);
	void Go_BR(_float fTimeDelta);
	void Go_R(_float fTimeDelta);
	void Go_FR(_float fTimeDelta);

private:
	_float4x4		m_WorldMatrix;
	_float			m_fSpeedPerSec = { 0.f };
	_float			m_fRotationPerSec = { 0.f };

	_bool			m_isDead = { false };

public:
	static	CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END