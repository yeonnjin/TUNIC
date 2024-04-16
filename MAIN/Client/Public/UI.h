#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	typedef struct UI_Desc
	{
		_bool	isRender = { true };
		_uint	iBindTextureIndex;
		_uint	iBindTransformIndex;
	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT	Set_UIInfo() = 0;

protected:
	//_uint			m_iNumUI = { 0 };
	_uint			m_iBindTextureIndex = { 0 };
	_uint			m_iBindTransformIndex = { 0 };

	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

protected:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

	vector<CTransform*> m_pUITransformComs;
	vector<UI_DESC>		m_UIDescs;
	
protected:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END