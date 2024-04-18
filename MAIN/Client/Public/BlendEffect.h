#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Client)

class CBlendEffect final : public CBlendObject
{
private:
	CBlendEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendEffect(const CBlendEffect& rhs);
	virtual ~CBlendEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float				m_fFrame = { 0.f };

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBlendEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END