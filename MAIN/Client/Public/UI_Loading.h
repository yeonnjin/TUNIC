#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Loading final : public CUI
{
private:
	CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Loading() = default;

public:
	void				Set_Using(_bool isUsing) { m_isUsing = isUsing; }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool				m_isUsing = { true };

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

