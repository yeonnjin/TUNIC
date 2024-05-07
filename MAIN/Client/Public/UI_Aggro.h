#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Aggro final : public CUI
{
public:
	typedef struct UI_Aggro_Desc : public CUI::UI_DESC
	{
		CTransform*		pTargetTransform;
		_float			fHeight = { 4.f };
	}UI_AGGRO_DESC;

private:
	CUI_Aggro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Aggro() = default;

public:
	void	Set_Using(_bool isUsing);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool				m_isUsing = { false };

	_float				m_fSize = { 40.f };
	_float				m_fHeight = { 2.f };

private:
	CTransform*			m_pTargetTransform = { nullptr };

private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources();
	virtual HRESULT		Set_UIInfo();

public:
	static CUI_Aggro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

