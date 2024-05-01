#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Telescope final : public CCamera
{
private:
	CCamera_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Telescope(const CCamera_Telescope& rhs);
	virtual ~CCamera_Telescope() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Set_Level(_uint iLevel);

	virtual void	OnEnter(void* pArg);
	virtual void	OnExit();

private:


public:
	static CCamera_Telescope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END