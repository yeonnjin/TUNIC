#pragma once

#include "Sprite_Attack.h"

BEGIN(Client)

class CSprite_Attack_Stick final : public CSprite_Attack
{
private:
	CSprite_Attack_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSprite_Attack_Stick() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//_bool			m_isUsing = { false };

	//_uint			m_iCurIndex = { 0 };
	//_uint			m_iMaxIndex = { 4 };

	//_float			m_fSize = { 64.f };

	//_float			m_fAccTickTime = { 0.f };
	//_float			m_fTickTime = { 0.1f };

private:


private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

public:
	static CSprite_Attack_Stick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END