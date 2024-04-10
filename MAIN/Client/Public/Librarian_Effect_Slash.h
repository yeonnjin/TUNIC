#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CLibrarian_Effect_Slash final : public CEffect
{
public:
	typedef struct Librarian_Effect_Slash_Desc : public CEffect::EFFECT_DESC
	{
		_vector vStartPosition;
		_vector vLookDir;
		_bool	isVertical;
	}LIBRARIAN_EFFECT_SLASH_DESC;

private:
	CLibrarian_Effect_Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLibrarian_Effect_Slash(const CLibrarian_Effect_Slash& rhs);
	virtual ~CLibrarian_Effect_Slash() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isVertical = { false };

	_float			m_fAccLiveTime = { 0.f };
	_float			m_fLiveTime = { 1.f };

private:
	CModel*			m_pModelCom = { nullptr };
	CModel*			m_pModelCom_Horizon = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLibrarian_Effect_Slash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CEffect을(를) 통해 상속됨
	//void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END