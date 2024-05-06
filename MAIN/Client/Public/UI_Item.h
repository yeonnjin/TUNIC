#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Item final : public CUI
{
public:
	typedef struct UIItem_Desc : public CUI::UI_DESC
	{
		_bool		isObtain = { false };
		_uint		iTextureIndex;
		_float2		vPosition;
		_float		fSize = { 64.f };
	}UIITEM_DESC;

private:
	CUI_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Item() = default;

public:
	void	Set_isUsing(_bool isUsing) { m_isUsing = isUsing; }
	void	Set_TextureIndex(_uint iTextureIndex);
	void	Set_Position(_float2 vPosition);
	void	Set_Count(_int iCount) { m_iCount += iCount; }

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
	_bool			m_isUsing = { true };
	_bool			m_isObtain = { false };

	_int			m_iCount = { 1 };

	_float2			m_vPosition = {};
	_float			m_fSize = { 64.f };

	_tchar			m_szFont[MAX_PATH] = TEXT("");

public:
	static CUI_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END