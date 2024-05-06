#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Font final : public CUI
{
private:
	CUI_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Font() = default;

public:
	typedef struct UI_Font_Desc : public CUI::UI_DESC
	{
		_float2		vPosition;
		_uint*		pCount;
	}UI_FONT_DESC;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT	Add_Components() { return S_OK; }
	virtual HRESULT Bind_ShaderResources() { return S_OK; }
	virtual HRESULT	Set_UIInfo() { return S_OK; }

private:
	_bool			m_isUsing = { true };
	_bool			m_isObtain = { false };

	_uint*			m_pCount = { nullptr };

	_float2			m_vPosition = {};

	_tchar			m_szFont[MAX_PATH] = TEXT("");

public:
	static CUI_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END