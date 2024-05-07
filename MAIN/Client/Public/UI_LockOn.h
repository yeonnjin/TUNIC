#pragma once


#include "UI.h"

BEGIN(Client)

class CUI_LockOn final : public CUI
{
private:
	CUI_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_LockOn() = default;

public:
	void	Set_Using(_bool isUsing, CTransform* pTargetTransform = nullptr);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_bool				m_isUsing = { false };

	_float				m_fSize = { 50.f };
	_float				m_fHeight = { 2.f };

private:
	CTransform*			m_pTargetTransform = { nullptr };

private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources();
	virtual HRESULT		Set_UIInfo();

public:
	static CUI_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

