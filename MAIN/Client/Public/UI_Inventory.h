#pragma once

#include "UI.h"
#include "Item.h"

BEGIN(Client)

class CUI_Inventory final : public CUI
{
private:
	CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Inventory() = default;

public:
	void	Set_Using(_bool isUsing) { m_isUsing = isUsing; }
	void	Set_Select(_uint iRow, _uint iColumn);

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
	_bool	m_isUsing = { false };

	_float	m_fBGSizeX = { 0.f };
	_float	m_fBGSizeY = { 0.f };

	_float2 m_vPositions[CItem::TYPE_END][4];

public:
	static CUI_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END