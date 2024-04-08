#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Librarian final : public CEffect
{
public:
	typedef struct Effect_Librarian_Desc : public CEffect::EFFECT_DESC
	{
		CBone* pSocketBone = { nullptr };
		wstring				strModelComTag;
	}EFFECT_LIBRARIAN_DESC;

private:
	CEffect_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Librarian(const CEffect_Librarian& rhs);
	virtual ~CEffect_Librarian() = default;

public:
	void			Set_isRender(_bool isRender) { m_isRender = isRender; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	wstring				m_strModelComTag = {};
	_bool				m_isRender = { false };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CBone* m_pSocketBone = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Librarian* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CEffect을(를) 통해 상속됨
	//void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END