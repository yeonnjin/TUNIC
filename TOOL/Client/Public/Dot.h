#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;

class CDot final : public CGameObject
{
public:
	typedef struct Dot_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		vPosition;
	}DOT_DESC;

private:
	CDot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDot(const CDot& rhs);
	virtual ~CDot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render() override;

private:
	CCollider*		m_pColliderCom = { nullptr };
	_float4x4		m_WorldMatrix;

private:
	virtual HRESULT	Add_Components();

public:
	static CDot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END