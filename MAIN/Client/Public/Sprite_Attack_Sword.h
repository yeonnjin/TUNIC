#pragma once

#include "Sprite_Attack.h"

BEGIN(Client)

class CSprite_Attack_Sword final : public CSprite_Attack
{
private:
	CSprite_Attack_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSprite_Attack_Sword() = default;

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

public:
	static CSprite_Attack_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

