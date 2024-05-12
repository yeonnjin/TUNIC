#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CAnimator;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CInteractiveObject abstract: public CGameObject
{
public:
	enum INTERACTIVE { INTERACTIVE_CHEST, INTERACTIVE_ITEM, INTERACTIVE_TELESCOPE, INTERACTIVE_LADDER, INTERACTIVE_GEM, INTERACTIVE_COLLIDER, INTERACTIVE_TELEPORT, INTERACTIVE_TRIGGER_MAP, INTERACTIVE_TRIGGER_SCENE, INTERACTIVE_END };

protected:
	CInteractiveObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractiveObject(const CInteractiveObject& rhs);
	virtual ~CInteractiveObject() = default;

public:
	INTERACTIVE				Get_InteractiveType() { return m_eInteractiveType; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_LightDepth() override;

protected:
	INTERACTIVE				m_eInteractiveType = { INTERACTIVE_END };

	_matrix					m_ColliderMatrix = {};

protected:
	CAnimator*				m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	class CUI_Interactive*	m_pUIInteractive = { nullptr };

protected:
	virtual HRESULT	Add_Components();
	virtual HRESULT	Bind_ShaderResources();
	virtual void	Compute_ColliderMatrix();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END