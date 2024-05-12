#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CParticle abstract: public CGameObject
{
public:
	typedef struct Particle_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_vector		vPosition;
		_vector		vScale;
		_float		fLiveTime;
	}PARTICLE_DESC;

protected:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	_float			m_fAccLiveTime = { 0.f };
	_float			m_fLiveTime = { 1.f };

protected:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

protected:
	HRESULT	Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END

