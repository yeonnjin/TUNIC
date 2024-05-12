#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CObject_Bridge final : public CInteractiveObject
{
private:
	CObject_Bridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Bridge(const CObject_Bridge& rhs);
	virtual ~CObject_Bridge() = default;

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;

private:

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();
	virtual		void		Compute_ColliderMatrix();

public:
	static CObject_Bridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END



