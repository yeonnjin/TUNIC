#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CMap_WaterDepth final : public CBlendObject
{
private:
	CMap_WaterDepth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_WaterDepth(const CMap_WaterDepth& rhs);
	virtual ~CMap_WaterDepth() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CMap_WaterDepth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END



