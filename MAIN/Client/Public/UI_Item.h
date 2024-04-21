#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Item final : public CUI
{
public:
	typedef struct UIItem_Desc : public CUI::UI_DESC
	{
		_uint		iTextureIndex;
		_float2		vPosition;
	}UIITEM_DESC;

private:
	CUI_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

private:
	_float2		m_vPosition = {};
	_float		m_fSize = { 64.f };

public:
	static CUI_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END