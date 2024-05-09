#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CMap_WaterEdge final : public CBlendObject
{
private:
	CMap_WaterEdge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_WaterEdge(const CMap_WaterEdge& rhs);
	virtual ~CMap_WaterEdge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fHeight = { 0.f };

	_float			m_fAccTime = { 0.f };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();
	void			Compute_Height(_float fTimeDelta);

public:
	static CMap_WaterEdge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

