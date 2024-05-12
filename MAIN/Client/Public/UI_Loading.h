#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Loading final : public CUI
{
private:
	CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Loading() = default;

public:
	_bool				Get_isFinish() { return m_isFinish; }				
	_bool				Get_isFadeOut() { return m_isLoading; }

	void				Set_Using(_bool isUsing, _int iLeafIndex = -1);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool				m_isUsing = { true };
	_bool				m_isFadeOut = { false };
	_bool				m_isLoading = { false };
	_bool				m_isFinish = { false };

	_int				m_iLeafIndex = { -1 };

	_float				m_fAccShaderTimeDelta = { 0.f };
	_float				m_fShaderTimeDelta = { 1.f };

	_float				m_fAccLeafTime = { 0.f };
	_float				m_fLeafTime = { 0.1f };

private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources();
	virtual HRESULT		Set_UIInfo();

public:
	static CUI_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

