#pragma once

#include "UI.h"

BEGIN(Client) 

class CUI_Stat final : public CUI
{
private:
	CUI_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Stat() = default;

public:
	void	Set_HP(_uint iHP) { m_iHP = iHP; }
	void	Set_SP(_uint iSP) { m_fSP = iSP; }
	void	Set_MP(_uint iMP) { m_fMP = iMP; }

	void	Set_Stat(_uint iHP, _float fSP, _float fMP) { m_iHP = iHP; m_fSP = fSP; m_fMP = fMP; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint	m_iHP = { 7 };
	_uint	m_iMaxHP = { 7 };
	_float	m_fHPSize = { 0 };

	_float	m_fSP = { 4.f };
	_float	m_fMaxSP = { 4.f };
	_float	m_fSPSize = { 0 };

	_float	m_fMP = { 4.f };
	_float	m_fMaxMP = { 4.f };
	_float	m_fMPSize = { 0 };

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

public:
	static CUI_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END