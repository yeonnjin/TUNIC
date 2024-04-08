#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CTrigger abstract : public CGameObject
{
public:
	typedef struct Trigger_Desc : public GAMEOBJECT_DESC
	{
	}TRIGGER_DESC;

protected:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END