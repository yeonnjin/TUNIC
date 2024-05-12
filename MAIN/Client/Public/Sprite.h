#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSprite abstract : public CUI
{
public:
	typedef struct Sprite_Desc : public CGameObject::GAMEOBJECT_DESC
	{
	}SPRITE_DESC;

protected:
	CSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSprite(const CSprite& rhs);
	virtual ~CSprite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

protected:
	virtual	HRESULT	Add_Components();
	virtual	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END

