#pragma once

#include "Player_Weapon.h"

BEGIN(Client)

class CWeapon_Wand final : public CPlayer_Weapon
{
private:
	CWeapon_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Wand(const CWeapon_Wand& rhs);
	virtual ~CWeapon_Wand() = default;

public:
	void	Set_Dir(_vector vDir) { m_vDir = vDir; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_vector m_vDir = {};

	_vector m_vMtrlDiffuse = { 1.f, 0.785f, 0.811f, 1.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Wand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CPartObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END