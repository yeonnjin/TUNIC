#pragma once

#include "Player_Weapon.h"

BEGIN(Client)

class CWeapon_Laurel final : public CPlayer_Weapon
{
private:
	CWeapon_Laurel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Laurel(const CWeapon_Laurel& rhs);
	virtual ~CWeapon_Laurel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_vector			m_vMtrlDiffuse = { 0.858f, 0.504f, 1.f, 1.f };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CWeapon_Laurel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
