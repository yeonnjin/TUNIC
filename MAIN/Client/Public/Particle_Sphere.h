#pragma once

#include "Particle.h"

BEGIN(Client)

class CParticle_Sphere final : public CParticle
{
private:
	CParticle_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Sphere(const CParticle_Sphere& rhs);
	virtual ~CParticle_Sphere() = default;

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
	static CParticle_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END