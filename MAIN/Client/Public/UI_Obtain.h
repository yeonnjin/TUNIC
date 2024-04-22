#pragma once

#include "UI.h"
#include "Item.h"

BEGIN(Client)

class CUI_Obtain final : public CUI
{
private:
	CUI_Obtain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Obtain() = default;

public:
	_bool	Get_Using() { return m_isUsing; }
	void	Set_Using(_bool isUsing, CItem::ITEM eItem = CItem::ITEM_END);

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
	_bool			m_isUsing = { false };

	CItem::ITEM		m_eSelectItem = { CItem::ITEM_END };

	_float			m_fAccShowTime = { 0.f };
	_float			m_fShowTime = { 2.f };

private:
	class CUI_Item* m_pUIItem = { nullptr };

public:
	static CUI_Obtain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END