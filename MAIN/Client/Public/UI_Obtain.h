#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Obtain final : public CUI
{
private:
	CUI_Obtain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Obtain() = default;

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


public:
	static CUI_Obtain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END