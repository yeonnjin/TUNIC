#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_WandBeam final : public CEffect
{
public:
	typedef struct Effect_WandBeam_Desc : public CEffect::EFFECT_DESC
	{
		_vector vStartPosition;
		_vector vLookDir;
	}EFFECT_WANDBEAM_DESC;

private:
	CEffect_WandBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_WandBeam(const CEffect_WandBeam& rhs);
	virtual ~CEffect_WandBeam() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fAccLiveTime = { 0.f };
	_float			m_fLiveTime = { 2.f };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CEffect_WandBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CEffect을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END