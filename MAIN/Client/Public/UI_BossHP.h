#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossHP final : public CUI
{
private:
	CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_BossHP() = default;

public:
	void				Set_Using(_bool isUsing) { m_isUsing = isUsing; }
	void				Set_MaxHp(_float fMaxHP) { m_fHP = m_fRenderHP = m_fMaxHP = fMaxHP; }
	void				Set_HPMinus() { m_fHP -= 1.f; }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool				m_isUsing = { false };

	_float				m_fHP = { 7.f };
	_float				m_fRenderHP = { 7.f };
	_float				m_fMaxHP = { 7.f };
	_float				m_fHPSize = { 643.f };

	_float2				m_vPositions;


private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources();
	virtual HRESULT		Set_UIInfo();

public:
	static CUI_BossHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END