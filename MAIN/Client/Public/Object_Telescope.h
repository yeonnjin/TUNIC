#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CObject_Telescope final : public CInteractiveObject
{
private:
	CObject_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Telescope(const CObject_Telescope& rhs);
	virtual ~CObject_Telescope() = default;

public:

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;
	virtual HRESULT			Render_LightDepth() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CCollider*				m_pRigidColliderCom = { nullptr };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();
	virtual		void		Compute_ColliderMatrix();

public:
	static CObject_Telescope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;
};

END