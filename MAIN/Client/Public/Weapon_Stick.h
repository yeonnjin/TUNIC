#pragma once

#include "Player_Weapon.h"

BEGIN(Client)

class CWeapon_Stick final : public CPlayer_Weapon
{
private:
	CWeapon_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Stick(const CWeapon_Stick& rhs);
	virtual ~CWeapon_Stick() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Stick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CPartObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END