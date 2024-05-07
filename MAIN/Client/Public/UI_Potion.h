#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Potion final : public CUI
{
private:
	CUI_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Potion() = default;

public:
	void				Use_Potion();
	void				Add_Potion();
	_bool				Get_isMax() { return (m_iMaxPotion == m_iNumPotion) ? true : false; }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_uint				m_iMaxPotion = { 2 };
	_uint				m_iNumPotion = { 2 };

	_float				m_fPlusHP = { 2.f };
	_float				m_fSize = { 50.f };

	_float2				m_vPositions[2];

private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources();
	virtual HRESULT		Set_UIInfo();

public:
	static CUI_Potion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

