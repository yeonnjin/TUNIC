#pragma once

#include "Sprite.h"

BEGIN(Client)

class CSprite_Sweat final : public CSprite
{
private:
	CSprite_Sweat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSprite_Sweat() = default;

public:
	void	Set_Using(_bool isUsing);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isUsing = { false };

	_uint			m_iCurIndex = { 0 };
	_uint			m_iMaxIndex = { 7 };

	_float			m_fSize = { 128.f };

	_float			m_fAccTickTime = { 0.f };
	_float			m_fTickTime = { 0.05f };

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

public:
	static CSprite_Sweat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END