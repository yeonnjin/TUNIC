#pragma once

#include "UI.h"
#include "Player.h"

BEGIN(Client)

class CUI_Slot final : public CUI
{
public:
	enum SLOT { SLOT_J, SLOT_K, SLOT_L, SLOT_END };

private:
	CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Slot() = default;

public:
	CPlayer::WEAPON		Get_Weapon(SLOT eSlot) { return m_eSlotWeapon[eSlot]; }
	void				Set_Slot(SLOT eSlot, CPlayer::WEAPON eWeapon);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	_float				m_fSize = { 40.f };

	_float2				m_vPositions[SLOT_END];

	CPlayer::WEAPON		m_eSlotWeapon[SLOT_END];

private:
	virtual HRESULT		Add_Components();
	virtual HRESULT		Bind_ShaderResources(); 
	virtual HRESULT		Set_UIInfo();

	_uint				Set_WeaponTextureIndex(CPlayer::WEAPON eWeapon);

public:
	static CUI_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END