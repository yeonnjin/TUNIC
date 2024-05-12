#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CObject_Gem final: public CInteractiveObject
{
public:
	typedef struct Object_Gem_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_vector				vPosition;
		_uint				iGemCount;
		_uint				iIndex;
		_uint				iNavigationIndex;
	}OBJECT_GEM_DESC;

private:
	CObject_Gem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Gem(const CObject_Gem& rhs);
	virtual ~CObject_Gem() = default;

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;
	virtual HRESULT			Render_LightDepth() override;

private:
	_bool					m_isLand = { false };
	_bool					m_isCollision = { false };

	_uint					m_iGemCount = {};
	_uint					m_iIndex = {};
	_uint					m_iNavigationIndex = {};

	_float					m_fAccPopTime = { 0.f };
	_float					m_fPopTime = { 5.f };

	_float					m_fOriginY = { 0.f };
	_vector					m_vOriginPosition = { 0.f };

	_vector					m_vMtrlDiffuse = { 1.f, 0.696f, 0.f, 1.f };
	_vector					m_vPrePosition = {};

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();
	virtual		void		Compute_ColliderMatrix();
	void					Compute_Dir();
	void					Move_Pop(_float fTimeDelta);
	void					Move_Collision(_float fTimeDelta);

public:
	static CObject_Gem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END