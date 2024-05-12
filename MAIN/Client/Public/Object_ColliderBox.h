#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CObject_ColliderBox final : public CInteractiveObject
{
public:
	typedef struct ColliderBox_Desc : public CInteractiveObject::GAMEOBJECT_DESC
	{
		_vector		vPosition;
		_float3		vSize = { 1.f, 1.f, 1.f };
		_bool		isRotation = { false };
		_float		fAngle = { 45.f };
		_bool		isUseCollider = { false };
	}COLLIDERBOX_DESC;

private:
	CObject_ColliderBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_ColliderBox(const CObject_ColliderBox& rhs);
	virtual ~CObject_ColliderBox() = default;

public:

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;

private:
	_bool		m_isUseCollider = { false };

	_float3		m_vSize = { 1.f, 1.f, 1.f };

private:
	CCollider*	m_pRigidColliderCom = { nullptr };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();

public:
	static CObject_ColliderBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END