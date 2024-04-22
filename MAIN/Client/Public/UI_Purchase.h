#pragma once

#include "UI.h"
#include "Item.h"

BEGIN(Client)

class CUI_Purchase final : public CUI
{
private:
	CUI_Purchase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Purchase() = default;

public:
	void	Set_Using(_bool isUsing, CItem::ITEM eItem = CItem::ITEM_END) {
		m_isUsing = isUsing; 
		m_eSelectItem = eItem;
	}

public:
	_bool			Get_isOK()  { return m_isOK; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isUsing = { false };
	_bool			m_isOK = { true };

	CItem::ITEM		m_eSelectItem = { CItem::ITEM_END };

	_float2			m_vSelectPosition[2];

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

	void			Set_RenderOff();
	void			Set_SelectRenderOn();

public:
	static CUI_Purchase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

